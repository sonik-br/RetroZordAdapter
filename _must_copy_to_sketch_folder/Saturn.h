/*******************************************************************************
 * Sega Saturn controllers to USB using an Arduino Leonardo.
 *
 * Works with digital pad and analog pad.
 *
 * By using the multitap it's possible to connect up to 7 controllers.
 *
 * Also works with MegaDrive controllers and mulltitaps.
 *
 * For details on Joystick Library, see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary
 *
*/

#include "src/SaturnLib/SaturnLib.h"
#include "src/ArduinoJoystickLibrary/Joy1.h"

//Saturn pins - Port 1
#define SAT1_TH A2
#define SAT1_TR 7
#define SAT1_TL A1
#define SAT1_D0 A4
#define SAT1_D1 A5
#define SAT1_D2 A3
#define SAT1_D3 A0

//Saturn pins - Port 2
#define SAT2_TH 8
#define SAT2_TR 9
#define SAT2_TL 5
#define SAT2_D0 2
#define SAT2_D1 3
#define SAT2_D2 4
#define SAT2_D3 6

SaturnPort<SAT1_D0, SAT1_D1, SAT1_D2, SAT1_D3, SAT1_TH, SAT1_TR, SAT1_TL> saturn1;
SaturnPort<SAT2_D0, SAT2_D1, SAT2_D2, SAT2_D3, SAT2_TH, SAT2_TR, SAT2_TL> saturn2;

void satResetJoyValues(const uint8_t i) {
    if (i >= totalUsb)
        return;

    usbStick[i]->resetState();
}

void saturnSetup() {
    //Init onboard led pin
    pinMode(LED_BUILTIN, OUTPUT);

    //Init the saturn class
    saturn1.begin();
    saturn2.begin();

    delayMicroseconds(10);

    //Detect multitap on first port
    saturn1.detectMultitap();
    uint8_t tap = saturn1.getMultitapPorts();
    if (tap == 0) { //Detect on second port
        saturn2.detectMultitap();
        tap = saturn2.getMultitapPorts();
    }

    if (tap == 0) //No multitap connected during boot
        totalUsb = 2;
    else //Multitap connected with 4 or 6 ports.
        totalUsb = MAX_USB_STICKS;//min(tap + 1, MAX_USB_STICKS);

    //Create usb controllers
    for (uint8_t i = 0; i < totalUsb; i++) {
        usbStick[i] = new Joy1_("RZordSat", JOYSTICK_DEFAULT_REPORT_ID + i, JOYSTICK_TYPE_GAMEPAD, totalUsb,
          true,//includeXAxis,
          true,//includeYAxis,
          false,//includeZAxis,
          false,//includeRxAxis,
          false,//includeRyAxis,
          true,//includeThrottle,
          true,//includeBrake,
          false);//includeSteering
    }

    //Set usb parameters and reset to default values
    for (uint8_t i = 0; i < totalUsb; i++) {
        //usbStick[i]->setXAxisRange(0, 255);
        //usbStick[i]->setYAxisRange(0, 255);
        //usbStick[i]->setThrottleRange(0, 255);
        //usbStick[i]->setBrakeRange(0, 255);
        //usbStick[i]->begin(false); //disable automatic sendState
        satResetJoyValues(i);
        usbStick[i]->sendState();
    }

    delay(50);

    dstart(115200);
    //debugln (F("Powered on!"));
}

inline bool __attribute__((always_inline))
saturnLoop() {
    static uint8_t lastControllerCount = 0;
    //const unsigned long start = micros();

    //Read each saturn port
    //It's not required to disable interrupts but it will gain some performance
    noInterrupts();
    saturn1.update();
    saturn2.update();
    interrupts();

    const unsigned long start = micros();

    //Get the number of connected controllers on each port
    const uint8_t joyCount1 = saturn1.getControllerCount();
    const uint8_t joyCount2 = saturn2.getControllerCount();
    const uint8_t joyCount = joyCount1 + joyCount2;

    for (uint8_t i = 0; i < joyCount; i++) {
        if (i == totalUsb)
            break;

        //Get the data for the specific controller from a port
        const SaturnController& sc = (i < joyCount1) ? saturn1.getSaturnController(i) : saturn2.getSaturnController(i - joyCount1);

        //Only process data if state changed from previous read
        if (sc.stateChanged()) {

            //Controller just connected. Also can happen when changing mode on 3d pad
            if (sc.deviceJustChanged())
                satResetJoyValues(i);

            //const bool isAnalog = sc.getIsAnalog();
            uint8_t hatData = sc.hat();

            ((Joy1_*)usbStick[i])->setButton(1, sc.digitalPressed(SAT_A));
            ((Joy1_*)usbStick[i])->setButton(2, sc.digitalPressed(SAT_B));
            ((Joy1_*)usbStick[i])->setButton(5, sc.digitalPressed(SAT_C));
            ((Joy1_*)usbStick[i])->setButton(0, sc.digitalPressed(SAT_X));
            ((Joy1_*)usbStick[i])->setButton(3, sc.digitalPressed(SAT_Y));
            ((Joy1_*)usbStick[i])->setButton(4, sc.digitalPressed(SAT_Z));
            ((Joy1_*)usbStick[i])->setButton(9, sc.digitalPressed(SAT_START));

            if (sc.isAnalog()) {
                ((Joy1_*)usbStick[i])->setAnalog0(sc.analog(SAT_ANALOG_X)); //x
                ((Joy1_*)usbStick[i])->setAnalog1(sc.analog(SAT_ANALOG_Y)); //y
                ((Joy1_*)usbStick[i])->setAnalog2(sc.analog(SAT_ANALOG_R)); //throttle
                ((Joy1_*)usbStick[i])->setAnalog3(sc.analog(SAT_ANALOG_L)); //brake

                //For racing wheel, don't report digital left and right of the dpad
                if (sc.deviceType() == SAT_DEVICE_WHEEL) {
                    hatData |= B1100;
                }

            }
            else {
                //Only report digital L and R on digital controllers.
                //The 3d pad will report both the analog and digital state for those when in analog mode.
                ((Joy1_*)usbStick[i])->setButton(7, sc.digitalPressed(SAT_R)); //R
                ((Joy1_*)usbStick[i])->setButton(6, sc.digitalPressed(SAT_L)); //L
            }

            //Get angle from hatTable and pass to joystick class
            ((Joy1_*)usbStick[i])->setHatSwitch(hatTable[hatData]);

            usbStick[i]->sendState();
        }
    }

    //Controller has been disconnected? Reset it's values!
    if (lastControllerCount > joyCount) {
        for (uint8_t i = joyCount; i < lastControllerCount; i++) {
            if (i == totalUsb)
                break;
            satResetJoyValues(i);
            usbStick[i]->sendState();
        }
    }

    //Keep count for next read
    lastControllerCount = joyCount;

    const uint8_t tap1 = saturn1.getMultitapPorts();
    const uint8_t tap2 = saturn2.getMultitapPorts();
    const uint8_t multitapPorts = tap1 + tap2;
    const bool isMegadriveTap = (tap1 == TAP_MEGA_PORTS) || (tap2 == TAP_MEGA_PORTS);

    if (isMegadriveTap) { //megadrive multitap
        sleepTime = 1000;//2500;//4000;
    } else {
        if (joyCount == 0)
            sleepTime = 1000;
        else if (multitapPorts != 0) //saturn multitap
            sleepTime = (joyCount + 1) * 500;
        else
            sleepTime = 50; //sleepTime = joyCount * 500;
    }

    //Sleep if total loop time was less than sleepTime
    unsigned long delta = micros() - start;
    if (delta < sleepTime) {
        //debugln(delta);
        delta = sleepTime - delta;
        //debug(F("\t"));
        //debugln(delta);
        delayMicroseconds(delta);
    }

    return joyCount != 0;
}

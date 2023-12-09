/*******************************************************************************
 * Snes controller input library.
 * https://github.com/sonik-br/SnesLib
*/

//Uncomment to enable multitap support. Requires wiring two additional pins.
//#define SNES_ENABLE_MULTITAP

#include "src/SnesLib/SnesLib.h"
#include "src/ArduinoJoystickLibrary/Joy1.h"

//Snes pins
#define SNESPIN_CLOCK  8
#define SNESPIN_LATCH  9
#define SNESPIN_DATA1  10

#ifdef SNES_ENABLE_MULTITAP
  #define SNESPIN_DATA2  5
  #define SNESPIN_SELECT 6
  SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1, SNESPIN_DATA2, SNESPIN_SELECT> snes;
#else
  SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1> snes;
#endif

void snesResetJoyValues(const uint8_t i) {
  if (i >= totalUsb)
    return;

  usbStick[i]->resetState();
}

void snesSetup() {
  //Init the class
  snes.begin();

  delayMicroseconds(10);

 //Multitap is connected?
  const uint8_t tap = snes.getMultitapPorts();
  if (tap == 0){ //No multitap connected during boot
    totalUsb = 1;
    sleepTime = 50;
  } else { //Multitap connected
    totalUsb = min(tap, MAX_USB_STICKS);
    sleepTime = 1000; //use longer interval between reads for multitap
  }

  //Create usb controllers
  for (uint8_t i = 0; i < totalUsb; i++) {
    usbStick[i] = new Joy1_("RZordSnes", JOYSTICK_DEFAULT_REPORT_ID + i, JOYSTICK_TYPE_GAMEPAD, totalUsb);
  }

  //Set usb parameters and reset to default values
  for (uint8_t i = 0; i < totalUsb; i++) {
    snesResetJoyValues(i);
    usbStick[i]->sendState();
  }
  
  dstart (115200);
}

inline bool __attribute__((always_inline))
snesLoop() {
  static uint8_t lastControllerCount = 0;

  //Read snes port
  snes.update();

  //Get the number of connected controllers
  const uint8_t joyCount = snes.getControllerCount();

  for (uint8_t i = 0; i < joyCount; i++) {
    if (i == totalUsb)
      break;
      
    //Get the data for the specific controller
    const SnesController& sc = snes.getSnesController(i);
    
    //Only process data if state changed from previous read
    if(sc.stateChanged()) {
      //Controller just connected.
      if (sc.deviceJustChanged())
        snesResetJoyValues(i);

      const SnesDeviceType_Enum padType = sc.deviceType();
      const uint8_t hatData = sc.hat();
      uint32_t buttonData = 0;

      if (padType == SNES_DEVICE_NES) {
        //Remaps as SNES B and A
        //bitWrite(buttonData, 1, sc.digitalPressed(SNES_Y));
        //bitWrite(buttonData, 2, sc.digitalPressed(SNES_B));

        //Same map as on a real snes console
        bitWrite(buttonData, 0, sc.digitalPressed(SNES_Y));
        bitWrite(buttonData, 1, sc.digitalPressed(SNES_B));
      } else {
        bitWrite(buttonData, 0, sc.digitalPressed(SNES_Y));
        bitWrite(buttonData, 1, sc.digitalPressed(SNES_B));
        bitWrite(buttonData, 2, sc.digitalPressed(SNES_A));
        bitWrite(buttonData, 3, sc.digitalPressed(SNES_X));
        bitWrite(buttonData, 4, sc.digitalPressed(SNES_L));
        bitWrite(buttonData, 5, sc.digitalPressed(SNES_R));
      }

      if(padType == SNES_DEVICE_NTT) {
        bitWrite(buttonData, 6, sc.nttPressed(SNES_NTT_DOT));
        bitWrite(buttonData, 7, sc.nttPressed(SNES_NTT_C));
        bitWrite(buttonData, 10, sc.nttPressed(SNES_NTT_0));
        bitWrite(buttonData, 11, sc.nttPressed(SNES_NTT_1));
        bitWrite(buttonData, 12, sc.nttPressed(SNES_NTT_2));
        bitWrite(buttonData, 13, sc.nttPressed(SNES_NTT_3));
        bitWrite(buttonData, 14, sc.nttPressed(SNES_NTT_4));
        bitWrite(buttonData, 15, sc.nttPressed(SNES_NTT_5));
        bitWrite(buttonData, 16, sc.nttPressed(SNES_NTT_6));
        bitWrite(buttonData, 17, sc.nttPressed(SNES_NTT_7));
        bitWrite(buttonData, 18, sc.nttPressed(SNES_NTT_8));
        bitWrite(buttonData, 19, sc.nttPressed(SNES_NTT_9));
        bitWrite(buttonData, 20, sc.nttPressed(SNES_NTT_STAR));
        bitWrite(buttonData, 21, sc.nttPressed(SNES_NTT_HASH));
        bitWrite(buttonData, 22, sc.nttPressed(SNES_NTT_EQUAL));
      }
      
      bitWrite(buttonData, 8, sc.digitalPressed(SNES_SELECT));
      bitWrite(buttonData, 9, sc.digitalPressed(SNES_START));

      ((Joy1_*)usbStick[i])->setButtons(buttonData);

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
      snesResetJoyValues(i);
      usbStick[i]->sendState();
    }
  }

  //Keep count for next read
  lastControllerCount = joyCount;
  
  return joyCount != 0;
}

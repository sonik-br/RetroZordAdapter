/*******************************************************************************
 * Snes controller input library.
 * https://github.com/sonik-br/SnesLib
*/

//Uncomment to enable multitap support. Requires wiring two additional pins.
//#define SNES_ENABLE_MULTITAP

#include "src/SnesLib/SnesLib.h"

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

#define SNES_USB_BUTTON_COUNT 10

void snesResetJoyValues(const uint8_t i) {
  if (i >= totalUsb)
    return;
    
  for (uint8_t x = 0; x < SNES_USB_BUTTON_COUNT; x++)
    usbStick[i]->releaseButton(x);
}

void snesSetup() {
  //Init the class
  snes.begin();

  delayMicroseconds(10);

 //Multitap is connected?
  const uint8_t tap = snes.getMultitapPorts();
  if (tap == 0){ //No multitap connected during boot
    totalUsb = 1;
    sleepTime = 500;
  } else { //Multitap connected
    totalUsb = min(tap, MAX_USB_STICKS);
    sleepTime = 1000; //use longer interval between reads for multitap
  }

  for (uint8_t i = 0; i < totalUsb; i++) {
    usbStick[i] = new Joystick_ (
      "RZordSnes",
      JOYSTICK_DEFAULT_REPORT_ID + i,
      JOYSTICK_TYPE_GAMEPAD,
      SNES_USB_BUTTON_COUNT      // buttonCount
    );
    usbStick[i]->begin (false);
  }
  
  dstart (115200);
}

inline void __attribute__((always_inline))
snesLoop() {
  static uint8_t lastControllerCount = 0;
  const unsigned long start = micros();

  //Read snes port
  //It's not required to disable interrupts but it will gain some performance
  noInterrupts();
  snes.update();
  interrupts();
  
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

      uint8_t hatData = sc.hat();

      if (sc.deviceType() == SNES_DEVICE_NES) {
        usbStick[i]->setButton(1, sc.digitalPressed(SNES_Y));
        usbStick[i]->setButton(2, sc.digitalPressed(SNES_B));
      } else {
        usbStick[i]->setButton(0, sc.digitalPressed(SNES_Y));
        usbStick[i]->setButton(1, sc.digitalPressed(SNES_B));
        usbStick[i]->setButton(2, sc.digitalPressed(SNES_A));
        usbStick[i]->setButton(3, sc.digitalPressed(SNES_X));
        usbStick[i]->setButton(4, sc.digitalPressed(SNES_L));
        usbStick[i]->setButton(5, sc.digitalPressed(SNES_R));
      }
      usbStick[i]->setButton(8, sc.digitalPressed(SNES_SELECT));
      usbStick[i]->setButton(9, sc.digitalPressed(SNES_START));

      //Get angle from hatTable and pass to joystick class
      usbStick[i]->setHatSwitch(0, hatTable[hatData]);

      usbStick[i]->sendState();
    }
  }

  //Controller has been disconnected? Reset it's values!
  if (lastControllerCount > joyCount) {
    for (uint8_t i = joyCount; i < lastControllerCount; i++) {
      if (i == totalUsb)
        break;
      snesResetJoyValues(i);
    }
  }

  //Keep count for next read
  lastControllerCount = joyCount;
  
  //sleep if total loop time was less than sleepTime
  unsigned long delta = micros() - start;
  //debugln(delta);
  if (delta < sleepTime) {
    delta = sleepTime - delta;
    delayMicroseconds(delta);
    //debugln(delta);
  }

  if (joyCount == 0) //blink led while no controller connected
    blinkLed();
}

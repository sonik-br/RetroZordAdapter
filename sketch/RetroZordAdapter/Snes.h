/*******************************************************************************
 * Snes controller input library.
 * https://github.com/sonik-br/SnesLib
*/

#include "src/SnesLib/SnesLib.h"

//Snes pins
#define SNESPIN_CLOCK  8
#define SNESPIN_LATCH  9
#define SNESPIN_DATA1  10

#define SNES_USB_BUTTON_COUNT 10

SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1> snes;

void snesResetJoyValues() {
  for (uint8_t x = 0; x < SNES_USB_BUTTON_COUNT; x++)
    usbStick[0]->releaseButton(x);
}

void snesSetup() {
  usbStick[0] = new Joystick_ (
    "RetroZordSnes",
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_JOYSTICK,
    SNES_USB_BUTTON_COUNT,      // buttonCount
    1,      // hatSwitchCount (0-2)
    false,   // includeXAxis
    false,   // includeYAxis
    false,    // includeZAxis
    false,    // includeRxAxis
    false,    // includeRyAxis
    false,    // includeRzAxis
    false,    // includeRudder
    false,    // includeThrottle
    false,    // includeAccelerator
    false,    // includeBrake
    false,   // includeSteering
    false,   // includeDial
    false   // includeWheel
  );

  usbStick[0]->begin (false);
  
  //REMEMBER TO DEFINE THE PINS IN LibSNES.H
  dstart (115200);
  
  //Init the class
  snes.begin();
  
  sleepTime = 500;
}

void snesLoop() {
  static uint8_t lastControllerCount = 0;
  const unsigned long start = micros();

  //Read snes port
  //It's not required to disable interrupts but it will gain some performance
  noInterrupts();
  snes.update();
  interrupts();
  
  //Get the number of connected controllers
  const uint8_t joyCount = snes.getControllerCount();

  if (joyCount > 0) {
    //Get the data for the specific controller
    const SnesController& sc = snes.getSnesController(0);
    
    //Only process data if state changed from previous read
    if(sc.stateChanged()) {
      //Controller just connected.
      if (sc.deviceJustChanged())
        snesResetJoyValues();

      uint8_t hatData = sc.hat();

      usbStick[0]->setButton(0, sc.digitalPressed(SNES_Y));
      usbStick[0]->setButton(1, sc.digitalPressed(SNES_B));
      usbStick[0]->setButton(2, sc.digitalPressed(SNES_A));
      usbStick[0]->setButton(3, sc.digitalPressed(SNES_X));
      usbStick[0]->setButton(4, sc.digitalPressed(SNES_L));
      usbStick[0]->setButton(5, sc.digitalPressed(SNES_R));
      usbStick[0]->setButton(8, sc.digitalPressed(SNES_SELECT));
      usbStick[0]->setButton(9, sc.digitalPressed(SNES_START));

      //Get angle from hatTable and pass to joystick class
      usbStick[0]->setHatSwitch(0, hatTable[hatData]);

      usbStick[0]->sendState();
    }
  }

  //Controller has been disconnected? Reset it's values!
  if (lastControllerCount > joyCount) {
    snesResetJoyValues();
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

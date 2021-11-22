#include "src/LibSNES/LibSNES.h"


uint16_t snesLastState = 0;

void snesSetup() {
  usbStick[0] = new Joystick_ (
    "RetroZordSnes",
    JOYSTICK_DEFAULT_REPORT_ID,
    JOYSTICK_TYPE_JOYSTICK,
    10,      // buttonCount
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
  //SNES.begin(11, 12, 13); //clock, latch, data
  SNES.begin();
    
/*
Pin       Desc.   Wire Color
1 +5v             White
2 Data    Clock   Yellow
3 Data    Latch   Orange
4 Serial  Data    Red
5 N/C -
6 N/C -
7 Ground          Brown
*/
  sleepTime = 1000;

}


void snesSetUsbValues() {
  uint8_t hat = ((snesLastState ^ B11110000) & B11110000) >> 4;

  usbStick[0]->setButton(0, (snesLastState & SNES_Y) != 0);
  usbStick[0]->setButton(1, (snesLastState & SNES_B) != 0);
  usbStick[0]->setButton(2, (snesLastState & SNES_A) != 0);
  usbStick[0]->setButton(3, (snesLastState & SNES_X) != 0);
  usbStick[0]->setButton(4, (snesLastState & SNES_L) != 0);
  usbStick[0]->setButton(5, (snesLastState & SNES_R) != 0);
  usbStick[0]->setButton(8, (snesLastState & SNES_SELECT) != 0);
  usbStick[0]->setButton(9, (snesLastState & SNES_START) != 0);
  
  usbStick[0]->setHatSwitch(0, hatTable[hat]);
  usbStick[0]->sendState();
}

void snesLoop() {
  unsigned long start = micros();
  
  SNES.update();
  uint16_t state = SNES.getState();

  //debugln(state, BIN);

  if(snesLastState != state) {
    //update lastState
    snesLastState = state;
    snesSetUsbValues();
  }
  
  //sleep if total loop time was less than sleepTime
  unsigned long delta = micros() - start;
  if (delta < sleepTime) {
    delta = sleepTime - delta;
    delayMicroseconds(delta);
    //debugln(delta);
  }
}

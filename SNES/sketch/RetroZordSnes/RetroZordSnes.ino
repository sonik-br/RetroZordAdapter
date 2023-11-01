/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

/*******************************************************************************
 * Optional settings
 *******************************************************************************/

//SNES Multitap
//#define SNES_ENABLE_MULTITAP

/******************************************************************************/

#include "Shared.h"
#include "Snes.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  snesSetup();
}

void loop() {
  static uint32_t last = 0;
  if (micros() - last >= sleepTime) {
    if (!snesLoop()) {
      //Blink led while no controller connected
      blinkLed();
    }
    last = micros();
  }
}

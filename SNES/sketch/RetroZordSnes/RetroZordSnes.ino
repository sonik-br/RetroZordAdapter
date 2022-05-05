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
//#SNES_ENABLE_MULTITAP

/******************************************************************************/

#include "Shared.h"
#include "Snes.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  snesSetup();
}

void loop() {
  if (!snesLoop()) {
    //Blink led while no controller connected
    blinkLed();
  }
}

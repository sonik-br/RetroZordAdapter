/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

#include "Shared.h"
#include "Psx.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  psxSetup();
}

void loop() {
 if (!psxLoop()) {
  //Blink led while no controller connected
  blinkLed();
 }
}

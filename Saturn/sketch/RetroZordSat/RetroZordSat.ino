/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

#include "Shared.h"
#include "Saturn.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  saturnSetup();
}

void loop() {
  if (!saturnLoop()) {
    //Blink led while no controller connected
    blinkLed();
  }
}

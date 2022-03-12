/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

#include "Shared.h"
#include "Snes.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  snesSetup();
}

void loop() {
  snesLoop();
}

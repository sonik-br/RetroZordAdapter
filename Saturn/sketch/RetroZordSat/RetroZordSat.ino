/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

/*******************************************************************************
 * Optional settings
 *******************************************************************************/

#define SATLIB_ENABLE_8BITDO_HOME_BTN // support for HOME button on 8bidto M30 2.4G.
#define SATLIB_ENABLE_MEGATAP //suport for 4p megatap
#define SATLIB_ENABLE_SATTAP //support for 6p multitap

#include "Shared.h"
#include "Saturn.h"

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  saturnSetup();
}

void loop() {
  static uint32_t last = 0;
  if (micros() - last >= sleepTime) {
    if (!saturnLoop()) {
      //Blink led while no controller connected
      blinkLed();
    }
    last = micros();
  }
}

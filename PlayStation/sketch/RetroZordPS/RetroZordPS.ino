/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

/*******************************************************************************
 * Optional settings
 *******************************************************************************/

// PS1 Guncon config
// 0=Mouse, 1=Joy, 2=Joy OffScreenEdge (MiSTer)
//#define GUNCON_FORCE_MODE 2

// PS1 NeGcon config
// 0=Default, 1=MiSTer Wheel format with paddle
//#define NEGCON_FORCE_MODE 1

/******************************************************************************/

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

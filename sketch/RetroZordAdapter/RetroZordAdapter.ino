/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/

#include "Shared.h"
#include "Saturn.h"
#include "Psx.h"
#include "Snes.h"

DeviceEnum deviceMode = SNK_NONE;

void setup() {
  int sensorValue = analogRead(A11);
  if (sensorValue > 850)//switch position left
    deviceMode = SNK_SATURN;
  else if (sensorValue < 150)//switch position right
    deviceMode = SNK_SNES;
  else//switch position middle
    deviceMode = SNK_PSX;

  pinMode(LED_BUILTIN, OUTPUT);

  switch(deviceMode){
    case SNK_SATURN:
      saturnSetup();
      break;
    case SNK_SNES:
      snesSetup();
      break;
    case SNK_PSX:
      psxSetup();
      break;
    default:
      break;
  }

}

void loop() {
 switch(deviceMode){
  case SNK_SATURN:
    saturnLoop();
    break;
  case SNK_SNES:
    snesLoop();
    break;
  case SNK_PSX:
    psxLoop();
    break;
  default:
    break;
 }
}

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

DeviceEnum deviceMode = RZORD_NONE;

void setup() {
  int sensorValue = analogRead(A11);
  if (sensorValue > 850)//switch position left
    deviceMode = RZORD_SATURN;
  else if (sensorValue < 150)//switch position right
    deviceMode = RZORD_SNES;
  else//switch position middle
    deviceMode = RZORD_PSX;

  pinMode(LED_BUILTIN, OUTPUT);

  switch(deviceMode){
    case RZORD_SATURN:
      saturnSetup();
      break;
    case RZORD_SNES:
      snesSetup();
      break;
    case RZORD_PSX:
      psxSetup();
      break;
    default:
      break;
  }
}

void loop() {
 switch(deviceMode){
  case RZORD_SATURN:
    saturnLoop();
    break;
  case RZORD_SNES:
    snesLoop();
    break;
  case RZORD_PSX:
    psxLoop();
    break;
  default:
    break;
 }
}

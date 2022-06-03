/*******************************************************************************
 * RetroZordAdapter Version 1.00
 * by Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
*/


/*******************************************************************************
 * Optional settings
 *******************************************************************************/

//Enable arduino atuoreset. Enable only if Leonardo with the default pin mapping
#define RZORD_ENABLE_AUTORESET

// PS1 Guncon config
// 0=Mouse, 1=Joy, 2=Joy OffScreenEdge (MiSTer)
//#define GUNCON_FORCE_MODE 2

// PS1 NeGcon config
// 0=Default, 1=MiSTer Wheel format with paddle
//#define NEGCON_FORCE_MODE 1

/******************************************************************************/

#include "Shared.h"
#include "Saturn.h"
#include "Psx.h"
#include "Snes.h"

DeviceEnum deviceMode = RZORD_NONE;

DeviceEnum getDeviceMode() {
  const int sensorValue = analogRead(A11);
  if (sensorValue > 850)//switch position left
    return RZORD_SATURN;
  else if (sensorValue < 150)//switch position right
    return RZORD_SNES;
  else//switch position middle
    return RZORD_PSX;
}

void setup() {
  deviceMode = getDeviceMode();

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
 bool connected = false;
 switch(deviceMode){
  case RZORD_SATURN:
    connected = saturnLoop();
    break;
  case RZORD_SNES:
    connected = snesLoop();
    break;
  case RZORD_PSX:
    connected = psxLoop();
    break;
  default:
    break;
 }

 if (!connected) {
  //Blink led while no controller connected
  blinkLed();

  //reset device if the switch changed while no controller connected
  #ifdef RZORD_ENABLE_AUTORESET
    if(deviceMode != getDeviceMode())
      resetDevice();
  #endif
 }

}

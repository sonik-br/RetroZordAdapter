/*******************************************************************************
 * RetroZordAdapter Version 2.00
 * By Matheus Fraguas (sonik-br)
 * 
 * https://github.com/sonik-br/RetroZordAdapter
 * 
 *
 * Uses a modified version of MPG
 * https://github.com/sonik-br/MPG
 *
*/

/*******************************************************************************
 * Optional settings
 *******************************************************************************/

// Enable arduino atutoreset. Enable only if Leonardo with the default pin mapping
#define RZORD_ENABLE_AUTORESET

// Sega MegaDrive/Saturn config
#define SATLIB_ENABLE_8BITDO_HOME_BTN // support for HOME button on 8bidto M30 2.4G.
#define SATLIB_ENABLE_MEGATAP //suport for 4p megatap
#define SATLIB_ENABLE_SATTAP //support for 6p multitap

// SNES config
#define SNES_ENABLE_MULTITAP


//PS1 Extra modes
#define GUNCON_SUPPORT
#define NEGCON_SUPPORT
#define JOGCON_SUPPORT

// PS1 Guncon config (DONT CHANGE. ONLY MODE 3 IS WORKING)
// 0=Mouse, 1=Joy, 2=Joy OffScreenEdge (MiSTer), 3=Raw (MiSTer)
#define GUNCON_FORCE_MODE 3

// PS1 NeGcon HID config
// 0=Default, 1=MiSTer Wheel format with paddle
#define NEGCON_FORCE_MODE 1

// Mouse output is used on guncon and jogcon modes.
// Can be disabled if only using on MiSTer
//#define ENABLE_PSX_GUNCON_MOUSE (NOT WORKING)
#define ENABLE_PSX_JOGCON_MOUSE

/******************************************************************************/

#include "Shared.h"
#include <LUFA.h>
#include "LUFADriver.h"
#include "RZInputModule.h"
#include "Input_Saturn.h"
#include "Input_Psx.h"
#include "Input_Snes.h"

RZInputModule* gamepadModule;

//VID:PID is defined on src/MPG/descriptors/HIDDescriptors.h

//Use the RetroZord manufacturer and product strings on personal projects only!
char USB_STRING_MANUFACTURER[] = "Matheus Fraguas";
char USB_STRING_PRODUCT[] = "RetroZord";

char USB_STRING_PS3_VERSION[] = "DUALSHOCK3";
char USB_SWITCH_STRING_MANUFACTURER[] = "HORI CO.,LTD.";
char USB_SWITCH_STRING_PRODUCT[] = "POKKEN CONTROLLER";

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

  //Init onboard led pin
  pinMode(LED_BUILTIN, OUTPUT);

  switch(deviceMode){
    case RZORD_SATURN:
      gamepadModule = new RZInputSaturn();
      break;
    case RZORD_SNES:
      gamepadModule = new RZInputSnes();
      break;
    case RZORD_PSX:
      gamepadModule = new RZInputPsx();
      break;
    default:
      while (1) { delay(1); }
      break;
  }

  gamepadModule->options.inputMode = INPUT_MODE_HID; // Default mode

  gamepadModule->setup(); // Runs your custom setup logic
  delay(20);
  gamepadModule->read();  // Perform an initial button read so we can set input mode
  delay(20);
  gamepadModule->read();

  if (gamepadModule->pressedB1(0)) {
    gamepadModule->options.inputMode = INPUT_MODE_SWITCH;
  } else if (gamepadModule->pressedB2(0)) {
    gamepadModule->options.inputMode = INPUT_MODE_XINPUT;
//  } else if (gamepadModule->pressedB2(0)) {
//    if(static_cast<uint8_t>(moduleInputMode) < static_cast<uint8_t>(INPUT_MODE_HID))
//      gamepadModule->options.inputMode = INPUT_MODE_HID;
  } else if (gamepadModule->pressedB3(0)) {
    gamepadModule->options.inputMode = INPUT_MODE_PS3;
  }

  gamepadModule->setup2(); //Second step of setup. Module can change the hid mode

  //gamepadModule->f1Mask = (GAMEPAD_MASK_B1 | GAMEPAD_MASK_S2); //(A+START)

  // Initialize USB device driver
  setupHardware(gamepadModule->options.inputMode, gamepadModule->totalUsb, gamepadModule->getUsbId());

}

void loop() {
  static uint32_t last = 0;
  static bool connected = false;

  if (micros() - last >= gamepadModule->sleepTime) {
    connected = gamepadModule->read();                              // Read raw inputs
    last = micros();
  }

  //gamepadModule->debounce();                                         // Run debouncing if enabled
  //hotkey = gamepadModule->hotkey(i);                                  // Check hotkey presses (D-pad mode, SOCD mode, etc.), hotkey enum returned

  for(uint8_t i = 0; i < gamepadModule->totalUsb; ++i) {
    gamepadModule->process(i);                                          // Perform final input processing (SOCD cleaning, LS/RS emulation, etc.)
    sendReport(gamepadModule->getReport(i), gamepadModule->getReportSize(i), &gamepadModule->rumble[i], i);             // Convert and send it!
    
    if(gamepadModule->options.inputMode == INPUT_MODE_XINPUT)         // Limit xinput to a single device
      break;
  }

  //Blink led while no controller connected
  blinkLed(connected);
  
  //reset device if the switch changed while no controller connected
  #ifdef RZORD_ENABLE_AUTORESET
    if (!connected && deviceMode != getDeviceMode())
      resetDevice();
  #endif

}

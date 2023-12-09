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

#define SATLIB_ENABLE_8BITDO_HOME_BTN // support for HOME button on 8bidto M30 2.4G.
#define SATLIB_ENABLE_MEGATAP //suport for 4p megatap
#define SATLIB_ENABLE_SATTAP //support for 6p multitap

/******************************************************************************/

#include "Shared.h"
#include <LUFA.h>
#include "LUFADriver.h"
#include "RZInputModule.h"
#include "Input_Saturn.h"

RZInputSaturn gamepadModule;

//VID:PID is defined on src/MPG/descriptors/HIDDescriptors.h

//Use the RetroZord manufacturer and product strings on personal projects only!
char USB_STRING_MANUFACTURER[] = "Matheus Fraguas";
char USB_STRING_PRODUCT[] = "RetroZord";

char USB_STRING_PS3_VERSION[] = "DUALSHOCK3";
char USB_SWITCH_STRING_MANUFACTURER[] = "HORI CO.,LTD.";
char USB_SWITCH_STRING_PRODUCT[] = "POKKEN CONTROLLER";

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
    
  gamepadModule.options.inputMode = INPUT_MODE_HID; // Default mode

  gamepadModule.setup(); // Runs your custom setup logic
  delay(20);
  gamepadModule.read();  // Perform an initial button read so we can set input mode
  delay(20);
  gamepadModule.read();

  if (gamepadModule.pressedB1(0)) {
    gamepadModule.options.inputMode = INPUT_MODE_SWITCH;
  } else if (gamepadModule.pressedB2(0)) {
    gamepadModule.options.inputMode = INPUT_MODE_XINPUT;
//  } else if (gamepadModule.pressedB2(0)) {
//    if(static_cast<uint8_t>(moduleInputMode) < static_cast<uint8_t>(INPUT_MODE_HID))
//      gamepadModule.options.inputMode = INPUT_MODE_HID;
  } else if (gamepadModule.pressedB3(0)) {
    gamepadModule.options.inputMode = INPUT_MODE_PS3;
  }

  gamepadModule.setup2(); //Second step of setup. Module can change the hid mode

  //gamepadModule.f1Mask = (GAMEPAD_MASK_B1 | GAMEPAD_MASK_S2); //(A+START)

  // Initialize USB device driver
  setupHardware(gamepadModule.options.inputMode, gamepadModule.totalUsb, gamepadModule.getUsbId());

}

void loop() {
  static uint32_t last = 0;
  static bool connected = false;

  if (micros() - last >= gamepadModule.sleepTime) {
    connected = gamepadModule.read();                              // Read raw inputs
    last = micros();
  }

  //gamepadModule.debounce();                                         // Run debouncing if enabled
  //hotkey = gamepadModule.hotkey(i);                                  // Check hotkey presses (D-pad mode, SOCD mode, etc.), hotkey enum returned

  for(uint8_t i = 0; i < gamepadModule.totalUsb; ++i) {
    gamepadModule.process(i);                                          // Perform final input processing (SOCD cleaning, LS/RS emulation, etc.)
    sendReport(gamepadModule.getReport(i), gamepadModule.getReportSize(i), &gamepadModule.rumble[i], i);             // Convert and send it!
    
    if(gamepadModule.options.inputMode == INPUT_MODE_XINPUT)         // Limit xinput to a single device
      break;
  }

  //Blink led while no controller connected
  blinkLed(connected);
}

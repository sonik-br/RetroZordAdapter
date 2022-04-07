//Arduino Joystick Library
#include "src/ArduinoJoystickLibrary/Joystick.h"

//Send debug messages to serial port
//#define ENABLE_SERIAL_DEBUG

//maximum 6 controllers per arduino
#define MAX_USB_STICKS 6

unsigned int sleepTime;//In micro seconds

Joystick_* usbStick[MAX_USB_STICKS];

uint8_t totalUsb = 1;//how many controller outputs via usb.

#ifdef ENABLE_SERIAL_DEBUG
  #define dstart(spd) do {Serial.begin (spd); while (!Serial) {digitalWrite (LED_BUILTIN, (millis () / 500) % 2);}} while (0);
  #define debug(...) Serial.print (__VA_ARGS__)
  #define debugln(...) Serial.println (__VA_ARGS__)
#else
  #define dstart(...)
  #define debug(...)
  #define debugln(...)
#endif

//hat table angles. RLDU
const int16_t hatTable[] = {
  0,0,0,0,0,//not used
  135, //0101
  45,  //0110
  90,  //0111
  0,//not used
  225, //1010
  315, //1010
  270, //1011
  0,//not used
  180, //1101
  0,   //1110
  JOYSTICK_HATSWITCH_RELEASE,  //1111
};

enum DeviceEnum {
  RZORD_NONE = 0,
  RZORD_SATURN,
  RZORD_SNES,
  RZORD_PSX
};

void blinkLed() {
  //bitWrite(PORTC, 7, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  //bitWrite(PORTC, 7, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

//Arduino Joystick Library
#include "src/ArduinoJoystickLibrary/Joystick.h"

#define ENABLE_AUTORESET

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

#ifdef ENABLE_AUTORESET
#include <avr/wdt.h>
void resetDevice(){
  //Set all used Leonardo pins as INPUT LOW reset arduino leonardo to initial port state. (initial state)
  //Comment if need to free space, not using a leonardo or not based on ATmega32U4
  DDRB &= B00000001; //bits .1234567
  PORTB &= B00000001;
  DDRC &= B10111111; //bits ......6.
  PORTC &= B10000000;
  DDRD &= B00101100; //bits 01..4.67  
  PORTD &= B00101100;
  DDRE &= B01000000; //bits ......6.
  PORTE &= B01000000;
  DDRF &= B00001100; //bits 01..4567
  PORTF &= B00001100;

  //use the watchdog to reset the arduino
  wdt_enable(WDTO_1S);
  while(1);
}
#endif

void blinkLed() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

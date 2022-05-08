//Arduino Joystick Library
#include "src/ArduinoJoystickLibrary/Joystick.h"

//#define RZORD_ENABLE_AUTORESET

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
const uint8_t hatTable[] = {
  JOYSTICK_HATSWITCH_RELEASE,JOYSTICK_HATSWITCH_RELEASE,JOYSTICK_HATSWITCH_RELEASE,JOYSTICK_HATSWITCH_RELEASE,JOYSTICK_HATSWITCH_RELEASE, //not used
  3,  //0101 RD
  1,  //0110 RU
  2,  //0111 R
  15, //not used
  5,  //1001 LD
  7,  //1010 LU
  6,  //1011 L
  JOYSTICK_HATSWITCH_RELEASE, //not used
  4,  //1101 D
  0,  //1110 U
  JOYSTICK_HATSWITCH_RELEASE  //1111
};

enum DeviceEnum {
  RZORD_NONE = 0,
  RZORD_SATURN,
  RZORD_SNES,
  RZORD_PSX
};

#ifdef RZORD_ENABLE_AUTORESET
#include <avr/wdt.h>
void resetDevice(){
  //Set all used Leonardo pins as INPUT LOW.
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
  bool ledmode = false;
  wdt_enable(WDTO_4S);
  while(1) {
    //blink led fast to indicate that mode will change
    digitalWrite(LED_BUILTIN, ledmode = !ledmode);
    delay(90);
  }
}
#endif

void blinkLed() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}

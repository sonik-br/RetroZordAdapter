#ifndef RZORD_SHARED_H_
#define RZORD_SHARED_H_

//#define RZORD_ENABLE_AUTORESET

//Send debug messages to serial port
//#define ENABLE_SERIAL_DEBUG

//maximum 6 controllers per arduino
#define MAX_USB_STICKS 5

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

void blinkLed(const bool connected) {
  static uint32_t last = 0;
  static uint8_t ledState = 0;

  if ((connected && ledState) || (!connected && millis() - last >= 500)) {
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);
    last = millis();
  }
}
#endif //RZORD_SHARED_H_

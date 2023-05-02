/*
  Derived class
  3 buttons
  2 analog absolute axes (0 to 32767)
*/

#ifndef GUNCON1_h
#define GUNCON1_h

#include "Joystick.h"

//typedef struct {
//  uint8_t id;
//  uint8_t buttons : 3;
//  uint16_t x;
//  uint16_t y;
//} GunconReport1;

class Guncon1_ : public Joystick_
{
  public:
    Guncon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers);
    void resetState();
    void setButtons(const uint8_t value) { setByte1(value); };
    void setXAxis(const uint16_t value) { setByte2(lowByte(value)); setByte3(highByte(value)); };
    void setYAxis(const uint16_t value) { setByte4(lowByte(value)); setByte5(highByte(value)); };
};

#endif // GUNCON1_h

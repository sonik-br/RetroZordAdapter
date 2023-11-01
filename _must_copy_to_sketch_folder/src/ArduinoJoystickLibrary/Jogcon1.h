/*
  Derived class
  16 buttons
  Hat
  If joystick: analog relative spinner and paddle
*/

#ifndef JOGCON1_h
#define JOGCON1_h

#include "Joystick.h"

//typedef struct {
//  uint8_t id;
//  uint16_t buttons;
//  uint8_t hat;
//  int8_t spinner;
//  int8_t paddle;
//} JogconReport1;

class Jogcon1_ : public Joystick_
{
  public:
    Jogcon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers);
    void resetState();
    void setButtons(const uint16_t value) { setByte1(lowByte(value)); setByte2(highByte(value)); };
    void setHatSwitch(const uint8_t value) { setByte3(value); };
    void setSpinner(const int8_t value) { setByte4(value); };
    void setPaddle(const int8_t value) { setByte5(value); };
    
};

#endif // JOGCON1_h

/*
  Derived class
  Mouse with 2 relative axes
*/

#ifndef MOUSEREL1_h
#define MOUSEREL1_h

#include "Joystick.h"

//typedef struct {
//  uint8_t id;
//  int8_t x;
//  int8_t y;
//} MouseRelReport1;

class MouseRel1_ : public Joystick_
{
  public:
    MouseRel1_(const char* serial, const uint8_t reportId, const uint8_t totalControllers);
    void resetState();

    void setXAxis(const int8_t value) { setByte1(value); };
    void setYAxis(const int8_t value) { setByte2(value); };
};

#endif // MOUSEREL_h

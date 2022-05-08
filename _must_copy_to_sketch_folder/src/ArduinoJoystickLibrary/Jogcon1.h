/*
  Derived class
  16 buttons
  Hat
  If joystick: analog relative spinner and paddle
*/

#ifndef JOGCON1_h
#define JOGCON1_h

#include "Joystick.h"

typedef struct {
  uint8_t id;
  uint16_t buttons;
  uint8_t hat;
  int8_t spinner;
  int8_t paddle;
} JogconReport1;

class Jogcon1_ : public Joystick_
{
  private:
    JogconReport1 _GamepadReport;
    uint8_t _hidReportSize;

  public:
    Jogcon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers);
    void setButton(const uint8_t index, const bool value);
    void setHatSwitch(const uint8_t value);
    void setSpinner(const int8_t value);
    void setPaddle(const int8_t value);
    void sendState();
    void resetState();
};

#endif // JOGCON1_h

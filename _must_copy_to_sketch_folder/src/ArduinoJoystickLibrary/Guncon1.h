/*
  Derived class
  3 buttons
  2 analog absolute axes (0 to 32767)
*/

#ifndef GUNCON1_h
#define GUNCON1_h

#include "Joystick.h"

typedef struct {
  uint8_t id;
  uint8_t buttons : 3;
  uint16_t x;
  uint16_t y;
} GunconReport1;

class Guncon1_ : public Joystick_
{
  private:
    GunconReport1 _GamepadReport;
    uint8_t _hidReportSize;

  public:
    Guncon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers);
    void setButton(const uint8_t index, const bool value);
    void setXAxis(const uint16_t value);
    void setYAxis(const uint16_t value);
    void sendState();
    void resetState();
};

#endif // GUNCON1_h

/*
  Derived class
  Dynamic HID with:
  16 buttons
  Hat
  Up to 5 analog absolute axes
*/

#ifndef JOY1_h
#define JOY1_h

#include "Joystick.h"

#define JOY1_ANALOG_COUNT 5

typedef struct {
  uint8_t id;
  uint16_t buttons;// : 24;
  uint8_t hat;
  uint8_t analog0;
  uint8_t analog1;
  uint8_t analog2;
  uint8_t analog3;
  uint8_t analog4;
} GamepadReport1;

class Joy1_ : public Joystick_
{
  private:
    GamepadReport1 _GamepadReport;
    uint8_t _hidReportSize;

  public:
    Joy1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers,
      bool includeXAxis = false,
      bool includeYAxis = false,
      bool includeZAxis = false,
      bool includeRxAxis = false,
      bool includeRyAxis = false,
      bool includeThrottle = false,
      bool includeBrake = false,
      bool includeSteering = false);
    void setButton(const uint8_t index, const bool value);
    void setHatSwitch(const uint8_t value);
    void setAnalog0(const uint8_t value);
    void setAnalog1(const uint8_t value);
    void setAnalog2(const uint8_t value);
    void setAnalog3(const uint8_t value);
    void setAnalog4(const uint8_t value);
    void sendState();
    void resetState();
};

#endif // JOY1_h

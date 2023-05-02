/*
  Derived class
  Dynamic HID with:
  24 buttons
  Hat
  Up to 5 analog absolute axes
*/

#ifndef JOY1_h
#define JOY1_h

#include "Joystick.h"

#define JOY1_ANALOG_COUNT 5

//typedef struct {
//  uint8_t id;
//  uint32_t buttons : 24;
//  uint8_t hat;
//  uint8_t analog0;
//  uint8_t analog1;
//  uint8_t analog2;
//  uint8_t analog3;
//  uint8_t analog4;
//} GamepadReport1;

class Joy1_ : public Joystick_
{
  public:
    Joy1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers,
      bool includeXAxis = false,
      bool includeYAxis = false,
      bool includeZAxis = false,  //used as right stick X
      bool includeRzAxis = false, //used as right stick Y
      bool includeThrottle = false,
      bool includeBrake = false,
      bool includeSteering = false);
    void resetState();
    //void setButton(const uint8_t index, const bool value) {
    //    if (index < 8) {
    //      if (value)
    //        _GamepadReport.byte1 |= 1 << index;
    //      else
    //        _GamepadReport.byte1 &= ~(1 << index);
    //    } else {
    //      if (value)
    //        _GamepadReport.byte2 |= 1 << (index-8);
    //      else
    //        _GamepadReport.byte2 &= ~(1 << (index-8));
    //    }
    //}; //bytes 1 and 2
    void setButtons(const uint32_t value) {
      //Currently using only 24 bits
      const uint16_t low = value;
      const uint8_t high = value >> 16;
      setByte1(lowByte(low)); setByte2(highByte(low));
      setByte3(high);// setByte4(highByte(high));
    };
    void setHatSwitch(const uint8_t value) { setByte4(value); };
    void setAnalog0(const uint8_t value) { setByte5(value); };
    void setAnalog1(const uint8_t value) { setByte6(value); };
    void setAnalog2(const uint8_t value) { setByte7(value); };
    void setAnalog3(const uint8_t value) { setByte8(value); };
    void setAnalog4(const uint8_t value) { setByte9(value); };
};

#endif // JOY1_h

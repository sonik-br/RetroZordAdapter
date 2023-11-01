/*
  Joystick.h

  Copyright (c) 2015-2017, Matthew Heironimus

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Modified by sonik-br
 * Just a base class. Report data must be implemented on derived class.
 * Can use a mix of usb endpoints and composite hid.
 * Added usb serial id. Based on code from Mikael Norrgård <http://daemonbite.com>.
*/

#ifndef JOYSTICK_h
#define JOYSTICK_h

#define MAX_ENDPOINTS 2

#include "DynamicHID/DynamicHID.h"

#if ARDUINO < 10606
#error The Joystick library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Joystick library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif // !defined(USBCON)
#endif // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Joystick (Gamepad)

#define JOYSTICK_DEFAULT_REPORT_ID         0x03
#define JOYSTICK_HATSWITCH_RELEASE         0x0F
#define JOYSTICK_TYPE_MOUSE                0x02
#define JOYSTICK_TYPE_JOYSTICK             0x04
#define JOYSTICK_TYPE_GAMEPAD              0x05
#define JOYSTICK_TYPE_MULTI_AXIS           0x08

typedef struct {
    uint8_t id;
    uint8_t byte1;
    uint8_t byte2;
    uint8_t byte3;
    uint8_t byte4;
    uint8_t byte5;
    uint8_t byte6;
    uint8_t byte7;
    uint8_t byte8;
    uint8_t byte9;
} GamepadReport9;

class Joystick_
{  
  protected:
    GamepadReport9 _GamepadReport;
    uint8_t _hidReportSize;
    const uint8_t _endpointIndex;
    static uint8_t getNextIndex(const char* serial, const uint8_t totalControllers);
    static bool _useComposite;
    static DynamicHID_* _endpointPool[MAX_ENDPOINTS];
    static uint8_t _endpointInitialized[MAX_ENDPOINTS];
    //void internalSetButton(const void* firstByte, const uint8_t index, const bool value) {
    //    uint8_t ammount = index / 8;

    //    //read byte button values
    //    uint8_t bytebutton = 0;
    //    memcpy(&bytebutton, &firstByte + ammount, 1);

    //    uint8_t shiftIndex = index - (ammount * 8);
    //    if (value)
    //        bytebutton |= 1 << shiftIndex;
    //    else
    //        bytebutton &= ~(1 << shiftIndex);

    //    //write byte button values
    //    memcpy(firstByte + ammount, &bytebutton, 1);
    //};
  public:
    Joystick_(const char* serial, const uint8_t totalControllers) : _endpointIndex(getNextIndex(serial, totalControllers))
    {
      _useComposite = totalControllers > MAX_ENDPOINTS;
    };
    void sendState() {
      if (_useComposite)
        _endpointPool[_endpointIndex]->SendReport(&_GamepadReport, _hidReportSize);
      else
        _endpointPool[_endpointIndex]->SendReport((uint8_t*)&_GamepadReport+1, _hidReportSize);
    };
    virtual void resetState() = 0;
    //void setByte1(const uint8_t index, const bool value);
    void setByte1(const uint8_t value) { _GamepadReport.byte1 = value; };
    void setByte2(const uint8_t value) { _GamepadReport.byte2 = value; };
    void setByte3(const uint8_t value) { _GamepadReport.byte3 = value; };
    void setByte4(const uint8_t value) { _GamepadReport.byte4 = value; };
    void setByte5(const uint8_t value) { _GamepadReport.byte5 = value; };
    void setByte6(const uint8_t value) { _GamepadReport.byte6 = value; };
    void setByte7(const uint8_t value) { _GamepadReport.byte7 = value; };
    void setByte8(const uint8_t value) { _GamepadReport.byte8 = value; };
    void setByte9(const uint8_t value) { _GamepadReport.byte9 = value; };
};

#endif // !defined(_USING_DYNAMIC_HID)
#endif // JOYSTICK_h

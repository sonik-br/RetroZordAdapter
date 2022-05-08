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

class Joystick_
{  
  protected:
    const uint8_t _endpointIndex;
    static uint8_t getNextIndex(const char* serial, const uint8_t totalControllers);
    static bool _useComposite;
    static DynamicHID_* _endpointPool[MAX_ENDPOINTS];
    static uint8_t _endpointInitialized[MAX_ENDPOINTS];
  public:
    Joystick_(const char* serial, const uint8_t totalControllers) : _endpointIndex(getNextIndex(serial, totalControllers))
    {
      _useComposite = totalControllers > MAX_ENDPOINTS;
    };
    virtual void sendState() = 0;
    virtual void resetState() = 0;
};

#endif // !defined(_USING_DYNAMIC_HID)
#endif // JOYSTICK_h

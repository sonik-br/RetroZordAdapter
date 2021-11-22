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

#ifndef MOUSE_h
#define MOUSE_h

#include "DynamicHID/DynamicHID.h"

#if ARDUINO < 10606
#error The Mouse library requires Arduino IDE 1.6.6 or greater. Please update your IDE.
#endif // ARDUINO < 10606

#if ARDUINO > 10606
#if !defined(USBCON)
#error The Mouse library can only be used with a USB MCU (e.g. Arduino Leonardo, Arduino Micro, etc.).
#endif // !defined(USBCON)
#endif // ARDUINO > 10606

#if !defined(_USING_DYNAMIC_HID)

#warning "Using legacy HID core (non pluggable)"

#else // !defined(_USING_DYNAMIC_HID)

//================================================================================
//  Mouse

#define MOUSE_DEFAULT_AXIS_MINIMUM      -128
#define MOUSE_DEFAULT_AXIS_MAXIMUM       127
class Mouse_
{
private:

    // Mouse State
  int16_t                  _xAxis;
  int16_t                  _yAxis;

    // Mouse Settings
    bool                     _autoSendState;

  uint8_t                  _hidReportId;
  uint8_t                  _hidReportSize;

protected:
  int buildAndSet16BitValue(int16_t value, int16_t valueMinimum, int16_t valueMaximum, uint8_t dataLocation[]);

public:
  Mouse_(const char* serial);

  void begin(bool initAutoSendState = true);
  void end();
  

  // Set Axis Values
  void setXAxis(int16_t value);
  void setYAxis(int16_t value);

  void sendState();
};

#endif // !defined(_USING_DYNAMIC_HID)
#endif // MOUSE_h

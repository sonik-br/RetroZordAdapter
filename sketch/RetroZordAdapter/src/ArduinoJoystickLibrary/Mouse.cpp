/*
  Joystick.cpp

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

#include "Mouse.h"

#if defined(_USING_DYNAMIC_HID)

Mouse_::Mouse_(
  const char* serial)
{
    // Set the USB HID Report ID
    _hidReportId = 1;

    // Build Mouse HID Report Description
  
    uint8_t tempHidReportDescriptor[150];
    int hidReportDescriptorSize = 0;

    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // USAGE (Mouse - 0x01)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;//pointer

    // COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // REPORT_ID (Default: 3)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
  

  if (true) { //(axisCount > 0) || (_hatSwitchCount > 0)
  
    // USAGE_PAGE (Generic Desktop)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
    
  }

  if (true) { //axisCount > 0
  
    // USAGE (Pointer)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // LOGICAL_MINIMUM (-128)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

    // LOGICAL_MAXIMUM (+127)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

    // REPORT_SIZE (16)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

    // REPORT_COUNT (2)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
            
    // COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    if (true) { //includeXAxis == true
      // USAGE (X)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;
    }

    if (true) { //includeYAxis == true
      // USAGE (Y)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31;
    }

    
    // INPUT (Data,Var,Rel)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x06;
    
    // END_COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    
  } // X, Y, Z, Rx, Ry, and Rz Axis 
  

    // END_COLLECTION
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;

  // Create a copy of the HID Report Descriptor template that is just the right size
  uint8_t *customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);
  
  // Register HID Report Description
  DynamicHIDSubDescriptor *node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  DynamicHID().AppendDescriptor(node);

  // Define Serial
  if (strlen(serial) > 0)
    DynamicHID().DefineSerial(serial);
  
    // Setup Mouse State
  
  // Calculate HID Report Size
  _hidReportSize = 4;
  
  // Initalize Mouse State
  _xAxis = 0;
  _yAxis = 0;
}

void Mouse_::begin(bool initAutoSendState)
{
  _autoSendState = initAutoSendState;
  sendState();
}

void Mouse_::end()
{
}


void Mouse_::setXAxis(int16_t value)
{
  _xAxis = value;
  if (_autoSendState) sendState();
}
void Mouse_::setYAxis(int16_t value)
{
  _yAxis = value;
  if (_autoSendState) sendState();
}

int Mouse_::buildAndSet16BitValue(int16_t value, int16_t valueMinimum, int16_t valueMaximum, uint8_t dataLocation[]) 
{
  //int16_t convertedValue;
  uint8_t highByte;
  uint8_t lowByte;
  int16_t realMinimum = min(valueMinimum, valueMaximum);
  int16_t realMaximum = max(valueMinimum, valueMaximum);

  //if (includeValue == false) return 0;

  if (value < realMinimum) {
    value = realMinimum;
  }
  if (value > realMaximum) {
    value = realMaximum;
  }

  if (valueMinimum > valueMaximum) {
    // Values go from a larger number to a smaller number (e.g. 1024 to 0)
    value = realMaximum - value + realMinimum;
  }

  //convertedValue = map(value, realMinimum, realMaximum, actualMinimum, actualMaximum);

  highByte = (uint8_t)(value >> 8);
  lowByte = (uint8_t)(value & 0x00FF);
  
  dataLocation[0] = lowByte;
  dataLocation[1] = highByte;
  
  return 2;
}


void Mouse_::sendState()
{
  uint8_t data[_hidReportSize];
  int index = 0;
  index += buildAndSet16BitValue(_xAxis, MOUSE_DEFAULT_AXIS_MINIMUM, MOUSE_DEFAULT_AXIS_MAXIMUM, &(data[index]));
  index += buildAndSet16BitValue(_yAxis, MOUSE_DEFAULT_AXIS_MINIMUM, MOUSE_DEFAULT_AXIS_MAXIMUM, &(data[index]));
  DynamicHID().SendReport(_hidReportId, data, _hidReportSize);
}

#endif

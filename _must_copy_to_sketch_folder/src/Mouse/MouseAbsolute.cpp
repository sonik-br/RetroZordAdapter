/*  Gamepad.cpp
 *   
 *  Based on the advanced HID library for Arduino: 
 *  https://github.com/NicoHood/HID
 *  Copyright (c) 2014-2015 NicoHood
 * 
 *  Copyright (c) 2020 Mikael Norrgård <http://daemonbite.com>
 *
 *  GNU GENERAL PUBLIC LICENSE
 *  Version 3, 29 June 2007
 *  
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  
 */

//Modified to output as a absolute position mouse.
//Matheus Fraguas (sonik-br)

#include "MouseAbsolute.h"

static const uint8_t _absMouseHidReportDescriptor[] PROGMEM = {
0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
0x09, 0x02,        // Usage (Mouse)
0xA1, 0x01,        // Collection (Application)
0x09, 0x01,        //   Usage (Pointer)
0xA1, 0x00,        //   Collection (Physical)
0x05, 0x09,        //     Usage Page (Button)
0x19, 0x01,        //     Usage Minimum (0x01)
0x29, 0x03,        //     Usage Maximum (0x03)
0x15, 0x00,        //     Logical Minimum (0)
0x25, 0x01,        //     Logical Maximum (1)
0x95, 0x03,        //     Report Count (3)
0x75, 0x01,        //     Report Size (1)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x55, 0x00,        //     Unit Exponent (0)
0x65, 0x00,        //     Unit (None)
0x95, 0x01,        //     Report Count (1) //button padding
0x75, 0x05,        //     Report Size (5)
0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
0x09, 0x30,        //     Usage (X)
0x09, 0x31,        //     Usage (Y)
0x16, 0x00, 0x00,  //     Logical Minimum (0)
0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
0x75, 0x10,        //     Report Size (16)
0x95, 0x02,        //     Report Count (2)
0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
0xC0,              //   End Collection
0xC0,              // End Collection
// 56 bytes
};

//joystick absolute
//static const uint8_t _absMouseHidReportDescriptor[] PROGMEM = {
//0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
//0x09, 0x04,        // Usage (Joystick)
//0xA1, 0x01,        // Collection (Application)
//0x05, 0x09,        //     Usage Page (Button)
//0x19, 0x01,        //     Usage Minimum (0x01)
//0x29, 0x03,        //     Usage Maximum (0x03)
//0x15, 0x00,        //     Logical Minimum (0)
//0x25, 0x01,        //     Logical Maximum (1)
//0x95, 0x03,        //     Report Count (3)
//0x75, 0x01,        //     Report Size (1)
//0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
//0x55, 0x00,        //     Unit Exponent (0)
//0x65, 0x00,        //     Unit (None)
//0x95, 0x01,        //     Report Count (1) //button padding
//0x75, 0x05,        //     Report Size (5)
//0x81, 0x03,        //     Input (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
//0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
//0x09, 0x01,        //   Usage (Pointer)
//0xA1, 0x00,        //   Collection (Physical)
//0x09, 0x30,        //     Usage (X)
//0x09, 0x31,        //     Usage (Y)
//0x16, 0x00, 0x00,  //     Logical Minimum (0)
//0x26, 0xFF, 0x7F,  //     Logical Maximum (32767)
//0x75, 0x10,        //     Report Size (16)
//0x95, 0x02,        //     Report Count (2)
//0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
//0xC0,              //   End Collection
//0xC0,              // End Collection
//// 56 bytes
//};

MouseAbsolute_::MouseAbsolute_(void) : PluggableUSBModule(1, 1, epType), protocol(HID_REPORT_PROTOCOL), idle(1)
{
  epType[0] = EP_TYPE_INTERRUPT_IN;
  PluggableUSB().plug(this);
}

int MouseAbsolute_::getInterface(uint8_t* interfaceCount)
{
  *interfaceCount += 1; // uses 1
  HIDDescriptor hidInterface = {
    D_INTERFACE(pluggedInterface, 1, USB_DEVICE_CLASS_HUMAN_INTERFACE, HID_SUBCLASS_NONE, HID_PROTOCOL_NONE),
    D_HIDREPORT(sizeof(_absMouseHidReportDescriptor)),
    D_ENDPOINT(USB_ENDPOINT_IN(pluggedEndpoint), USB_ENDPOINT_TYPE_INTERRUPT, USB_EP_SIZE, 0x01)
  };
  return USB_SendControl(0, &hidInterface, sizeof(hidInterface));
}

int MouseAbsolute_::getDescriptor(USBSetup& setup)
{
  // Check if this is a HID Class Descriptor request
  if (setup.bmRequestType != REQUEST_DEVICETOHOST_STANDARD_INTERFACE) { return 0; }
  if (setup.wValueH != HID_REPORT_DESCRIPTOR_TYPE) { return 0; }

  // In a HID Class Descriptor wIndex cointains the interface number
  if (setup.wIndex != pluggedInterface) { return 0; }

  // Reset the protocol on reenumeration. Normally the host should not assume the state of the protocol
  // due to the USB specs, but Windows and Linux just assumes its in report mode.
  protocol = HID_REPORT_PROTOCOL;

  return USB_SendControl(TRANSFER_PGM, _absMouseHidReportDescriptor, sizeof(_absMouseHidReportDescriptor));
}

bool MouseAbsolute_::setup(USBSetup& setup)
{
  if (pluggedInterface != setup.wIndex) {
    return false;
  }

  uint8_t request = setup.bRequest;
  uint8_t requestType = setup.bmRequestType;

  if (requestType == REQUEST_DEVICETOHOST_CLASS_INTERFACE)
  {
    if (request == HID_GET_REPORT) {
      // TODO: HID_GetReport();
      return true;
    }
    if (request == HID_GET_PROTOCOL) {
      // TODO: Send8(protocol);
      return true;
    }
  }

  if (requestType == REQUEST_HOSTTODEVICE_CLASS_INTERFACE)
  {
    if (request == HID_SET_PROTOCOL) {
      protocol = setup.wValueL;
      return true;
    }
    if (request == HID_SET_IDLE) {
      idle = setup.wValueL;
      return true;
    }
    if (request == HID_SET_REPORT)
    {
    }
  }

  return false;
}

void MouseAbsolute_::reset()
{
  _Report.x = 0;
  _Report.y = 0;
  _Report.buttons = 0;
  this->send();
}

void MouseAbsolute_::setXAxis(const uint16_t value) { _Report.x = value; }
void MouseAbsolute_::setYAxis(const uint16_t value) { _Report.y = value; }

void MouseAbsolute_::setButton(const uint8_t button, const uint8_t value)
{
  if (value == 0)
  {
    releaseButton(button);
  }
  else
  {
    pressButton(button);
  }
}
void MouseAbsolute_::pressButton(const uint8_t button)
{
    if (button > 2) return;
    bitSet(_Report.buttons, button);
}
void MouseAbsolute_::releaseButton(const uint8_t button)
{
    if (button > 2) return;
    bitClear(_Report.buttons, button);
}

void MouseAbsolute_::send()
{
  USB_Send(pluggedEndpoint | TRANSFER_RELEASE, &_Report, sizeof(MouseAbsReport));
}

uint8_t MouseAbsolute_::getShortName(char *name)
{
  if(!next) 
  {
    strcpy(name, gp_serial);
    return strlen(name);
  }
  return 0;
}

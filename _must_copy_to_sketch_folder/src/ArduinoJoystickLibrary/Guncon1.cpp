#include "Guncon1.h"

Guncon1_::Guncon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers) :
  Joystick_(serial, totalControllers)
{
  _GamepadReport.id = reportId;

  // Build Joystick HID Report Description

  uint8_t tempHidReportDescriptor[100]; //58
  uint8_t hidReportDescriptorSize = 0;

  _hidReportSize = sizeof(GunconReport1);


  // USAGE_PAGE (Generic Desktop)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // USAGE (Joystick - 0x04; Gamepad - 0x05; Multi-axis Controller - 0x08)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = deviceType;

  // COLLECTION (Application)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xa1;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_ID (Default: 3)
  if (_useComposite) {
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x85;
    tempHidReportDescriptor[hidReportDescriptorSize++] = reportId;
  } else {
    _hidReportSize -=1;
  }
  
  //Buttons

  // USAGE_PAGE (Button)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;

  // USAGE_MINIMUM (Button 1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x19;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // USAGE_MAXIMUM (Button 3)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x29;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

  // LOGICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // LOGICAL_MAXIMUM (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_SIZE (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_COUNT (3)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

  // UNIT_EXPONENT (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x55;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // UNIT (None)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // INPUT (Data,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

  // REPORT_SIZE (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_COUNT (5)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;

  // INPUT (Const,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

  // End Buttons


  // USAGE_PAGE (Generic Desktop)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // USAGE (Pointer)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // LOGICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
  //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x16;
  //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
  //tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  //LOGICAL_MAXIMUM (32767)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
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

  // USAGE (X)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;

  // USAGE (Y)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31;
  
  // INPUT (Data,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
  
  // END_COLLECTION (Physical)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    
  // END_COLLECTION
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;


  // Create a copy of the HID Report Descriptor template that is just the right size
  uint8_t* customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);

  // Register HID Report Description
  DynamicHIDSubDescriptor* node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  _endpointPool[_endpointIndex]->AppendDescriptor(node);
}

void Guncon1_::setButton(const uint8_t index, const bool value) {
  if(value)
    _GamepadReport.buttons |= 1 << index;
  else
    _GamepadReport.buttons &= ~(1 << index);
}

void Guncon1_::setXAxis(const uint16_t value) { _GamepadReport.x = value; }
void Guncon1_::setYAxis(const uint16_t value) { _GamepadReport.y = value; }

void Guncon1_::sendState() {
  if (_useComposite)
    _endpointPool[_endpointIndex]->SendReport(&_GamepadReport, _hidReportSize);
  else
    _endpointPool[_endpointIndex]->SendReport((uint8_t*)&_GamepadReport+1, _hidReportSize);
}
  
void Guncon1_::resetState() {
  _GamepadReport.buttons = 0;
  _GamepadReport.x = 0;
  _GamepadReport.y = 0;
}

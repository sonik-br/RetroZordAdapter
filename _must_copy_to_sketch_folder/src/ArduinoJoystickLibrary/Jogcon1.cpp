#include "Jogcon1.h"

Jogcon1_::Jogcon1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers) :
  Joystick_(serial, totalControllers)
{
  _GamepadReport.id = reportId;

  // Build Joystick HID Report Description

  uint8_t tempHidReportDescriptor[150]; //??
  uint8_t hidReportDescriptorSize = 0;

  _hidReportSize = sizeof(JogconReport1);

  //joystick used for MiSTer-A1 JogCon
  //gamepad used for RZordPsJogCon. Remove the analog values
  if(deviceType == JOYSTICK_TYPE_GAMEPAD)
    _hidReportSize -= 2;


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

  // USAGE_MAXIMUM (Button 16)            
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x29;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;

  // LOGICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // LOGICAL_MAXIMUM (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_SIZE (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // REPORT_COUNT (16)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x10;
      
  // INPUT (Const,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x03;

  // End Buttons


  // USAGE_PAGE (Generic Desktop)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // Hat Switch

  // USAGE (Hat Switch)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x39;

  // LOGICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // LOGICAL_MAXIMUM (7)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x07;

  // PHYSICAL_MINIMUM (0)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

  // PHYSICAL_MAXIMUM (315)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x46;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x3B;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

  // UNIT (Eng Rot:Angular Pos)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x65;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x14;

  // REPORT_SIZE (8)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

  // REPORT_COUNT (1)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
          
  // INPUT (Data,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

  // End Hat Switch

/*
  if (deviceType == JOYSTICK_TYPE_JOYSTICK) {
  
    // USAGE (Pointer)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // LOGICAL_MINIMUM (-128)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;

    //LOGICAL_MAXIMUM (127)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;

    // REPORT_SIZE (8)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

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
    
  } // X, Y Axis 
*/  


    if (deviceType == JOYSTICK_TYPE_JOYSTICK) {
      // USAGE (Dial)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x37;
      
      // LOGICAL_MINIMUM (-128)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x80;
      
      // LOGICAL_MAXIMUM (127)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x25;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x7F;
      
      // REPORT_COUNT (1)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
      
      // REPORT_SIZE (8)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;
      
      // INPUT (Data,Var,Rel)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x06;
      
      
      // USAGE (Wheel)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x38;
      
      // LOGICAL_MINIMUM (0)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
      
      // LOGICAL_MAXIMUM (255)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
      
      // REPORT_COUNT (1)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;
      
      // REPORT_SIZE (8)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;
      
      // INPUT (Data,Var,Abs)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
    }
    
    
    // END_COLLECTION (Application)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;


  // Create a copy of the HID Report Descriptor template that is just the right size
  uint8_t* customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);

  // Register HID Report Description
  DynamicHIDSubDescriptor* node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  _endpointPool[_endpointIndex]->AppendDescriptor(node);
}

void Jogcon1_::setButton(const uint8_t index, const bool value) {
  if(value)
    _GamepadReport.buttons |= 1 << index;
  else
    _GamepadReport.buttons &= ~(1 << index);
}

void Jogcon1_::setHatSwitch(const uint8_t value) {
  _GamepadReport.hat = value;
}

void Jogcon1_::setSpinner(const int8_t value) { _GamepadReport.spinner = value; }
void Jogcon1_::setPaddle(const int8_t value) { _GamepadReport.paddle = value; }

void Jogcon1_::sendState() {
  if (_useComposite)
    _endpointPool[_endpointIndex]->SendReport(&_GamepadReport, _hidReportSize);
  else
    _endpointPool[_endpointIndex]->SendReport((uint8_t*)&_GamepadReport+1, _hidReportSize);
}
  
void Jogcon1_::resetState() {
  _GamepadReport.buttons = 0;
  _GamepadReport.hat = JOYSTICK_HATSWITCH_RELEASE;
  _GamepadReport.spinner = 0;
  _GamepadReport.paddle = 0;
}

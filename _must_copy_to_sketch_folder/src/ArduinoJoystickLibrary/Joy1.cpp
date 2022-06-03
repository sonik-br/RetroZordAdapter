#include "Joy1.h"

Joy1_::Joy1_(const char* serial, const uint8_t reportId, const uint8_t deviceType, const uint8_t totalControllers,
  bool includeXAxis, bool includeYAxis, bool includeZAxis, bool includeRxAxis, bool includeRyAxis,
  bool includeThrottle, bool includeBrake, bool includeSteering) :
  Joystick_(serial, totalControllers)
{
  _GamepadReport.id = reportId;

  // Build Joystick HID Report Description
  
  // Axis Calculations
  const uint8_t axisCount = (includeXAxis == true)
    +  (includeYAxis == true)
    +  (includeZAxis == true)
    +  (includeRxAxis == true)
    +  (includeRyAxis == true);
    //+  (includeRzAxis == true);
    
  const uint8_t simulationCount = (includeThrottle == true)
    + (includeBrake == true)
    + (includeSteering == true);

  const uint8_t analogCount = axisCount + simulationCount;
  _hidReportSize = sizeof(GamepadReport1) - (JOY1_ANALOG_COUNT - analogCount);

  //if(!_useComposite)
  //  _hidReportSize -=1;


  uint8_t tempHidReportDescriptor[100];
  uint8_t hidReportDescriptorSize = 0;


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

  // INPUT (Data,Var,Abs)
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
  tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;

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

  if (axisCount > 0) {
  
    // USAGE (Pointer)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x01;

    // LOGICAL_MINIMUM (0)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    //LOGICAL_MAXIMUM (255)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    // REPORT_SIZE (8)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

    // REPORT_COUNT (axisCount)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
    tempHidReportDescriptor[hidReportDescriptorSize++] = axisCount;
            
    // COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    if (includeXAxis == true) {
      // USAGE (X)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x30;
    }

    if (includeYAxis == true) {
      // USAGE (Y)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x31;
    }
    
    if (includeZAxis == true) {
      // USAGE (Z)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x32;
    }
    
    if (includeRxAxis == true) {
      // USAGE (Rx)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x33;
    }
    
    if (includeRyAxis == true) {
      // USAGE (Ry)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x34;
    }
    
    /*if (includeRzAxis == true) {
      // USAGE (Rz)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x35;
    }*/
    
    // INPUT (Data,Var,Abs)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
    
    // END_COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
    
  } // X, Y, Z, Rx, Ry, and Rz Axis 
  
  if (simulationCount > 0) {
  
    // USAGE_PAGE (Simulation Controls)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x05;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
    
    // LOGICAL_MINIMUM (0)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x15;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;
    
    //LOGICAL_MAXIMUM (255)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x26;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xFF;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    // REPORT_SIZE (8)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x75;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x08;

    // REPORT_COUNT (simulationCount)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x95;
    tempHidReportDescriptor[hidReportDescriptorSize++] = simulationCount;

    // COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xA1;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x00;

    /*if (includeRudder == true) {
      // USAGE (Rudder)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBA;
    }*/

    if (includeThrottle == true) {
      // USAGE (Throttle)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xBB;
    }

    /*if (includeAccelerator == true) {
      // USAGE (Accelerator)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC4;
    }*/

    if (includeBrake == true) {
      // USAGE (Brake)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC5;
    }

    if (includeSteering == true) {
      // USAGE (Steering)
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0x09;
      tempHidReportDescriptor[hidReportDescriptorSize++] = 0xC8;
    }

    // INPUT (Data,Var,Abs)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x81;
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0x02;
    
    // END_COLLECTION (Physical)
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;
  
  } // Simulation Controls

    // END_COLLECTION
    tempHidReportDescriptor[hidReportDescriptorSize++] = 0xc0;



  // Create a copy of the HID Report Descriptor template that is just the right size
  uint8_t* customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  memcpy(customHidReportDescriptor, tempHidReportDescriptor, hidReportDescriptorSize);

  // Register HID Report Description
  DynamicHIDSubDescriptor* node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  _endpointPool[_endpointIndex]->AppendDescriptor(node);

/*
  const uint8_t hidReportDescriptorSize = sizeof(_hidReportDescriptor);
  uint8_t* customHidReportDescriptor = new uint8_t[hidReportDescriptorSize];
  //memcpy(customHidReportDescriptor, &_hidReportDescriptor, hidReportDescriptorSize);
  memcpy_P(customHidReportDescriptor, &_hidReportDescriptor, hidReportDescriptorSize);

  //Insert the correct device type
  customHidReportDescriptor[3] = deviceType;
  
  //Insert the correct report id
  customHidReportDescriptor[7] = reportId;

  DynamicHIDSubDescriptor* node = new DynamicHIDSubDescriptor(customHidReportDescriptor, hidReportDescriptorSize, false);
  _endpointPool[_endpointIndex]->AppendDescriptor(node);
*/
}

void Joy1_::setButton(const uint8_t index, const bool value) {
  if(value)
    _GamepadReport.buttons |= 1 << index;
  else
    _GamepadReport.buttons &= ~(1 << index);
}

void Joy1_::setHatSwitch(const uint8_t value) {
  _GamepadReport.hat = value;
}

void Joy1_::setAnalog0(const uint8_t value) { _GamepadReport.analog0 = value; }
void Joy1_::setAnalog1(const uint8_t value) { _GamepadReport.analog1 = value; }
void Joy1_::setAnalog2(const uint8_t value) { _GamepadReport.analog2 = value; }
void Joy1_::setAnalog3(const uint8_t value) { _GamepadReport.analog3 = value; }
void Joy1_::setAnalog4(const uint8_t value) { _GamepadReport.analog4 = value; }

void Joy1_::sendState() {
  if (_useComposite)
    _endpointPool[_endpointIndex]->SendReport(&_GamepadReport, _hidReportSize);
  else
    _endpointPool[_endpointIndex]->SendReport((uint8_t*)&_GamepadReport+1, _hidReportSize);
}
  
void Joy1_::resetState() {
  _GamepadReport.buttons = 0;
  _GamepadReport.hat = JOYSTICK_HATSWITCH_RELEASE;
  _GamepadReport.analog0 = 127;
  _GamepadReport.analog1 = 127;
  _GamepadReport.analog2 = 0;
  _GamepadReport.analog3 = 0;
  _GamepadReport.analog4 = 0;
}

#include <Arduino.h>
#include "RZInputModule.h"

void RZInputModule::resetState(const uint8_t index) {
  if (index >= totalUsb)
    return;
  state[index] = {};
}

bool RZInputModule::canChangeMode() {
  return options.inputMode == INPUT_MODE_HID;
}

uint16_t RZInputModule::convertAnalog(const uint8_t value) {
  return (value << 8) + value;
}

bool RZInputModule::canUseAnalogTrigger() {
  return options.inputMode == INPUT_MODE_XINPUT || options.inputMode == INPUT_MODE_PS3; //|| options.inputMode == INPUT_MODE_HID;
}

//Static members
uint8_t RZInputModule::totalUsb {1};
uint16_t RZInputModule::sleepTime {DEFAULT_SLEEP_TIME};

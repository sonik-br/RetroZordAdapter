/*******************************************************************************
 * Sega Saturn controllers to USB using an Arduino Leonardo.
 * 
 * Works with digital pad and analog pad.
 * 
 * By using the multitap it's possible to connect up to 7 controllers.
 * 
 * Also works with MegaDrive controllers and mulltitaps.
 * 
 * For details on Joystick Library, see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary
 * 
*/

//Saturn port 1
#define SAT1_TH A2 //PF5
#define SAT1_TR 7  //PE6
#define SAT1_TL A1 //PF6
#define SAT1_D0 A4
#define SAT1_D1 A5
#define SAT1_D2 A3
#define SAT1_D3 A0

#define SAT1_PINGROUP PINF
#define SAT1_TH_PORT PORTF
#define SAT1_TH_BIT 5
#define SAT1_TR_PORT PORTE
#define SAT1_TR_PIN PINE
#define SAT1_TR_BIT 6
#define SAT1_TL_PIN PINF
#define SAT1_TL_BIT 6

//Saturn port 2
#define SAT2_TH 8 //PB4
#define SAT2_TR 9 //PB5
#define SAT2_TL 5 //PC6
#define SAT2_D0 2
#define SAT2_D1 3
#define SAT2_D2 4
#define SAT2_D3 6

#define SAT2_PINGROUP PIND
#define SAT2_TH_PORT PORTB
#define SAT2_TH_BIT 4
#define SAT2_TR_PORT PORTB
#define SAT2_TR_PIN PINB
#define SAT2_TR_BIT 5
#define SAT2_TL_PIN PINC
#define SAT2_TL_BIT 6

#define ENABLE_8BITDO_HOME_BTN//coment to disable the use of HOME button on 8bidto M30 2.4G.

uint8_t lastState[MAX_USB_STICKS][8]= {}; //Store the state of each controller

uint8_t joyStateChanged = B00000000; //Store if each joystick has changed state

enum DB9_TR_Enum {
  DB9_TR_INPUT = 0,
  DB9_TR_OUTPUT
};

DB9_TR_Enum portState[2] = { DB9_TR_INPUT, DB9_TR_INPUT };

uint8_t totalUsb = SATURN_PORTS;
uint8_t joyCount = 0;
uint8_t currentPort = 1;

uint8_t readNibble();
void readDigitalPad(uint8_t nibble_0, uint8_t nibble_1);
void readThreeWire();
void readMegadrivePad(uint8_t nibble_0, uint8_t nibble_1);
void readMegaMultiTap();
void setUsbValues(uint8_t joyIndex, bool isAnalog, uint8_t page, uint8_t nibbleTMP);
void readMultitap();
void readThreeWireController(bool isAnalog);
void readUnhandledPeripheral(uint8_t len);

void setTR(uint8_t value) {
  if (currentPort == 1)
    bitWrite(SAT1_TR_PORT, SAT1_TR_BIT, value);
  else
    bitWrite(SAT2_TR_PORT, SAT2_TR_BIT, value);
}

void setTH(uint8_t value) {
  if (currentPort == 1)
    bitWrite(SAT1_TH_PORT, SAT1_TH_BIT, value);
  else
    bitWrite(SAT2_TH_PORT, SAT2_TH_BIT, value);
}

bool readTL() {
  if (currentPort == 1)
    return bitRead(SAT1_TL_PIN, SAT1_TL_BIT);
  else
    return bitRead(SAT2_TL_PIN, SAT2_TL_BIT);
}

bool readTR() {
  if (currentPort == 1)
    return bitRead(SAT1_TR_PIN, SAT1_TR_BIT);
  else
    return bitRead(SAT2_TR_PIN, SAT2_TR_BIT);
}

uint8_t waitTL(uint8_t state) { //returns 1 if reach timeout
  int8_t t_out = 100;
  uint8_t compare = !state;
  while (readTL() == compare) {
    delayMicroseconds(4);
    if (!t_out--)
      return 1;
  }
  return 0;
}

uint8_t setTRAndWaitTL(uint8_t trValue) {
  setTR(trValue);
  return waitTL(trValue);
}

void setTR_Mode(DB9_TR_Enum mode) {
  if(portState[currentPort-1] != mode) {
    portState[currentPort-1] = mode;
    if(mode == DB9_TR_OUTPUT) {
      if (currentPort == 1)
        pinMode(SAT1_TR, OUTPUT);
      else
        pinMode(SAT2_TR, OUTPUT);
      setTR(HIGH);
    } else {
      if (currentPort == 1)
        pinMode(SAT1_TR, INPUT_PULLUP);
      else
        pinMode(SAT2_TR, INPUT_PULLUP);
    }
  }
}

uint8_t readMegadriveBits() { //6 bits
  uint8_t nibble = readNibble();
  if(portState[currentPort-1] != DB9_TR_OUTPUT) {//saturn dont need these values
    bitWrite(nibble, 4, readTL());
    bitWrite(nibble, 5, readTR());
  }        
  return nibble;
}

uint8_t readNibble() {
  //Read from port 1 or port 2
  uint8_t nibble = currentPort == 1 ? SAT1_PINGROUP : SAT2_PINGROUP;
  return ((nibble >> 4) & B00001000) + // D3
         ((nibble >> 2) & B00000100) + // D2
         ((nibble << 1) & B00000010) + // D1
         ((nibble >> 1) & B00000001);  // D0
}

void readSatPort() {
  uint8_t nibble_0;
  uint8_t nibble_1;

  setTH(HIGH);
  delayMicroseconds(4);
  nibble_0 = readMegadriveBits();

  setTH(LOW);
  delayMicroseconds(4);
  nibble_1 = readMegadriveBits();

  if ((nibble_0 & B00000111) == B00000100) { //Saturn ID
    //debugln (F("DIGITAL-PAD"));
    setTR_Mode(DB9_TR_OUTPUT);
    readDigitalPad(nibble_0 & B00001111, nibble_1 & B00001111);
  } else if ((nibble_0 & B00001111) == B00000001 && (nibble_1 & B00001111) == B00000001) { //Saturn ID
    //debugln (F("3-WIRE-HANDSHAKE"));
    setTR_Mode(DB9_TR_OUTPUT);
    readThreeWire();
  } else if((nibble_1 & B00001100) == B00000000) { //Megadrive ID
    //debugln (F("MEGADRIVE"));
    setTR_Mode(DB9_TR_INPUT);
    readMegadrivePad(nibble_0, nibble_1);
  } else if ((nibble_0 & B00001111) == B00000011 && (nibble_1 & B00001111) == B00001111) { //Megadrive multitap
    setTR_Mode(DB9_TR_OUTPUT);
    //debugln (F("MEGADRIVE MULTITAP"));
    readMegaMultiTap();
  } else {
    setTR_Mode(DB9_TR_INPUT);
  }

  setTH(HIGH);

  if (portState[currentPort-1] == DB9_TR_OUTPUT)
    setTR(HIGH);
  
  delayMicroseconds(50);
}

void readMegaMultiTap() {
  uint8_t joyIndex;
  uint8_t nibble_0;
  uint16_t tapPortState = B0;
  uint8_t nibbles;
  uint8_t tr = 0;
  uint8_t tl_timeout;
  uint8_t i;

  //read first two nibbles. should be zero and zero
  for (i = 0; i < 2; i++) {
    tl_timeout = setTRAndWaitTL(i);
    if (tl_timeout)
      return;
    delayMicroseconds(4);
  }

  //read each port controller ID and stores the 4 values in the 16 bits variable
  for (i = 0; i < 16; i+=4) {
    tl_timeout = setTRAndWaitTL(tr);
    if (tl_timeout)
      return;
    delayMicroseconds(4);
    
    tapPortState |= (readNibble() << i);

    tr ^= 1;
  }

  //now read each port
  for (i = 0; i < 16; i+=4) {
    nibble_0 = (tapPortState >> i) & B1111;

    if (nibble_0 == B0000) //3 button. 2 nibbles
      nibbles = 2;
    else if (nibble_0 == B0001) //6 buttons. 3 nibbles
      nibbles = 3;
    else if (nibble_0 == B0010) //mouse. 6 nibbles
      nibbles = 6;
    else //non-connection. 0 reads
      continue;

    if (nibbles != 6)//ignore mouse
      joyIndex = joyCount++;
    
    for(uint8_t x = 0; x < nibbles; x++) {
      tl_timeout = setTRAndWaitTL(tr);
      if (tl_timeout)
        return;
      
      if (nibbles != 6)//ignore mouse
        setUsbValues(joyIndex, false, x, readNibble());
      
      tr ^= 1;
    }
    
    delayMicroseconds(40);
  }
}

void readThreeWire() {
  uint8_t nibble_0;
  uint8_t nibble_1;
  uint8_t tl_timeout;

  for (uint8_t i = 0; i < 2; i++) {
    tl_timeout = setTRAndWaitTL(i);
    if (tl_timeout)
      return;

    delayMicroseconds(4);

    if (i == 0)
      nibble_0 = readNibble();
    else
      nibble_1 = readNibble();
  }

  if (nibble_0 == B00000100 && nibble_1 == B00000001) {
    //debugln (F("6P MULTI-TAP"));
    readMultitap();
  } else if (nibble_0 == B00000000 && nibble_1 == B00000010) {
    //debugln (F("DIGITAL"));
    readThreeWireController(false);
  } else if (nibble_0 == B00000001 && nibble_1 == B00000110) {
    //debugln (F("ANALOG"));
    readThreeWireController(true);
  } else if (nibble_0 == B00001110 && nibble_1 < B00000011) { //megadrive 3 or 6 button pad. ignore mouse
    //debugln (F("MEGADRIVE"));
    readThreeWireController(false);
  } else if (nibble_0 == B00001111 && nibble_1 == B00001111) {
    //debugln (F("NONE"));
  } else {
    //debugln (F("UNKNOWN"));
    readUnhandledPeripheral(nibble_1);
  }
}

void readMultitap() {
  uint8_t i;
  uint8_t tl_timeout;

  //Multitap header is fixed: 6 (ports) and zero (len)
  for (i = 0; i < 2; i++) { //set TR low and high
    tl_timeout = setTRAndWaitTL(i);
    if (tl_timeout)
      return;
    delayMicroseconds(4);
  }
  //delayMicroseconds(5);

  for (i = 0; i < 6; i++) {// Read the 6 multitap ports
    readThreeWire();
    delayMicroseconds(4);
  }

  for (i = 0; i < 2; i++) { //set TR low and high
    tl_timeout = setTRAndWaitTL(i);
    if (tl_timeout)
      return;
    delayMicroseconds(4);
  }
  
}

void readUnhandledPeripheral(uint8_t len) {
  uint8_t nibbles = len * 2;
  uint8_t tr = 0;
  uint8_t tl_timeout;

  for (uint8_t i = 0; i < nibbles; i++) {
    tl_timeout = setTRAndWaitTL(tr);
    if (tl_timeout)
      return;
    delayMicroseconds(4);

    tr ^= 1;
  }
}

void setUsbValues(uint8_t joyIndex, bool isAnalog, uint8_t page, uint8_t nibbleTMP) {
  //Only report L and R for digital pads. analog will use brake and throttle.

  if (joyIndex >= totalUsb)
    return;

  //Skip if no change in state
  if(lastState[joyIndex][page] == nibbleTMP)
    return;

  if (page == 0) { //HAT RLDU
    usbStick[joyIndex]->setHatSwitch(0, hatTable[nibbleTMP]);
  } else if (page == 1) { //SACB
    usbStick[joyIndex]->setButton(1, !(nibbleTMP & B00000100)); //A
    usbStick[joyIndex]->setButton(2, !(nibbleTMP & B00000001)); //B
    usbStick[joyIndex]->setButton(5, !(nibbleTMP & B00000010)); //C
    usbStick[joyIndex]->setButton(9, !(nibbleTMP & B00001000)); //S
  } else if (page == 2) { //RXYZ
    usbStick[joyIndex]->setButton(0, !(nibbleTMP & B00000100)); //X
    usbStick[joyIndex]->setButton(3, !(nibbleTMP & B00000010)); //Y
    usbStick[joyIndex]->setButton(4, !(nibbleTMP & B00000001)); //Z
    if (!isAnalog)
      usbStick[joyIndex]->setButton(7, !(nibbleTMP & B00001000)); //R
  } else if (page == 3 && !isAnalog) {
    usbStick[joyIndex]->setButton(6, !(nibbleTMP & B00001000)); //L
  } else if (page == 4) { //X axis
    usbStick[joyIndex]->setXAxis(nibbleTMP);
  } else if (page == 5) { //Y axis
    usbStick[joyIndex]->setYAxis(nibbleTMP);
  } else if (page == 6) { //RT axis
    usbStick[joyIndex]->setThrottle(nibbleTMP);
  } else if (page == 7) { //LT axis
    usbStick[joyIndex]->setBrake(nibbleTMP);
  }
  
  //update lastState for current joyIndex
  lastState[joyIndex][page] = nibbleTMP;
  
  //state changed for current joyIndex
  bitWrite(joyStateChanged, joyIndex, 1);
}

void readThreeWireController(bool isAnalog) {
  uint8_t joyIndex = joyCount++;
  uint8_t i;
  uint8_t nibbles = (isAnalog ? 12 : 4);
  uint8_t nibbleTMP;
  uint8_t nibble_0 = B0;
  uint8_t nibble_1;
  uint8_t tr = 0;
  uint8_t tl_timeout;

  for (i = 0; i < nibbles; i++) {
    tl_timeout = setTRAndWaitTL(tr);
    if (tl_timeout)
      return;
      
    delayMicroseconds(2);

    nibbleTMP = readNibble();
    tr ^= 1;

    if (i < 3) {
      setUsbValues(joyIndex, isAnalog, i, nibbleTMP);
    } else if(i == 3) { //L
      nibbleTMP &= B00001000;
      setUsbValues(joyIndex, isAnalog, i, nibbleTMP);
    } else if (i == 4) { //X axis
      nibble_0 = nibbleTMP;
    } else if (i == 5) { //X axis
      nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
      setUsbValues(joyIndex, isAnalog, 4, nibble_1);
    } else if (i == 6) { //Y axis
      nibble_0 = nibbleTMP;
    } else if (i == 7) { //Y axis
      nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
      setUsbValues(joyIndex, isAnalog, 5, nibble_1);
    } else if (i == 8) { //RT analog
      nibble_0 = nibbleTMP;
    } else if (i == 9) { //RT analog
      nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
      setUsbValues(joyIndex, isAnalog, 6, nibble_1);
    } else if (i == 10) { //LT analog
      nibble_0 = nibbleTMP;
    } else if (i == 11) { //LT analog
      nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
      setUsbValues(joyIndex, isAnalog, 7, nibble_1);
    }
  }
}

void readMegadrivePad(uint8_t nibble_0, uint8_t nibble_1) {
  uint8_t joyIndex = joyCount++;

  // RLDU
  setUsbValues(joyIndex, false, 0, nibble_0 & B00001111);

  // SACB
  setUsbValues(joyIndex, false, 1, (nibble_0 >> 4) | (nibble_1 >> 2));

  //check if is 6-button pad;
  uint8_t th = 1;
  for (uint8_t i = 0; i < 4; i++) { //set TH high-low-high-low
    setTH(th);
    delayMicroseconds(4);
    th ^= 1;
  }
 
  nibble_0 = readMegadriveBits();
  
  if ((nibble_0 & B00001111) == B0) { //it is a 6-button pad
    setTH(HIGH);
    delayMicroseconds(4);
    nibble_0 = readMegadriveBits();

    setTH(LOW);
    delayMicroseconds(4);
    nibble_1 = readMegadriveBits();

    //11MXYZ
    setUsbValues(joyIndex, false, 2, nibble_0 & B00001111);

    //SA1111
    //8bitdo M30 extra buttons
    //111S1H STAR and HOME
    
    //use HOME button as the missing L from saturn
#ifdef ENABLE_8BITDO_HOME_BTN
    setUsbValues(joyIndex, false, 3, nibble_1 << 3);
#endif
  }
}

void readDigitalPad(uint8_t nibble_0, uint8_t nibble_1) {
  uint8_t joyIndex = joyCount++;
  uint8_t nibbleTMP;
  //uint8_t currentHatState;

  // L100
  //debugln (firstNibble, BIN);
  setUsbValues(joyIndex, false, 3, nibble_0);

  // RLDU
  //setTH(LOW);
  //setTR(HIGH);
  //delayMicroseconds(4);
  //nibbleTMP = readNibble();
  //debugln (nibble_1, BIN);
  setUsbValues(joyIndex, false, 0, nibble_1);

  // SACB
  setTH(HIGH);
  setTR(LOW);
  delayMicroseconds(4);
  nibbleTMP = readNibble();
  //debugln (nibbleTMP, BIN);
  setUsbValues(joyIndex, false, 1, nibbleTMP);

  // RYXZ
  setTH(LOW);
  setTR(LOW);
  delayMicroseconds(4);
  nibbleTMP = readNibble();
  //debugln (nibbleTMP, BIN);
  setUsbValues(joyIndex, false, 2, nibbleTMP);
}

void reportUsb() {
  for (uint8_t i = 0; i < joyCount; i++) {
    if (i >= totalUsb)
      break;
    if (bitRead(joyStateChanged, i))
      usbStick[i]->sendState();
  }
}


//Call only on arduino power-on to check for multitap connected
void detectMultitap() {
  uint8_t nibble_0;
  uint8_t nibble_1;
  uint8_t nibbleTMP;
  uint8_t nibbles = 2;
  uint8_t i;
  uint8_t tr = 0;
  uint8_t tl_timeout;

  //setTH(HIGH);
  //setTR(HIGH);
  //delayMicroseconds(4);

  tl_timeout = waitTL(1); //Device is not ready yet. Saturn and Mega multitap hold TL high while initializing.
  if (tl_timeout)
    return;

  //readMegadriveBits
  nibble_0 = readNibble();

  setTH(LOW);
  delayMicroseconds(4);
  nibble_1 = readNibble();

  if ((nibble_0 & B00001111) == B00000011 && (nibble_1 & B00001111) == B00001111) { //Megadrive multitap
    setTR_Mode(DB9_TR_OUTPUT);
    totalUsb += 3;
  } else if (nibble_0 == B00000001 && nibble_1 == B00000001) { //SATURN 3-WIRE-HANDSHAKE
    setTR_Mode(DB9_TR_OUTPUT);
    for (i = 0; i < nibbles; i++) {
      tl_timeout = setTRAndWaitTL(i);
      if (tl_timeout)
        return;
      delayMicroseconds(4);

      nibbleTMP = readNibble();

      if (i == 0)
        nibble_0 = nibbleTMP;
      else
        nibble_1 = nibbleTMP;

      tr ^= 1;
    }

    if (nibble_0 == B00000100 && nibble_1 == B00000001) //Is a multitap
      totalUsb += 5;
  }

  //reset TH and TR to high
  setTH(HIGH);
  
  if (portState[currentPort-1] == DB9_TR_OUTPUT)
    setTR(HIGH);

  delayMicroseconds(4);
}

void saturnSetup() {
  //init onboard led pin
  pinMode(LED_BUILTIN, OUTPUT);

  //Port 1 pins

  //init output pins
  pinMode(SAT1_TH, OUTPUT);
  digitalWrite(SAT1_TH, HIGH);

  //init input pins
  pinMode(SAT1_D0, INPUT_PULLUP);
  pinMode(SAT1_D1, INPUT_PULLUP);
  pinMode(SAT1_D2, INPUT_PULLUP);
  pinMode(SAT1_D3, INPUT_PULLUP);
  pinMode(SAT1_TL, INPUT_PULLUP);

  //init TR pin. Can change to input or output during execution. Defaults to input to support megadrive controllers
  pinMode(SAT1_TR, INPUT_PULLUP);


  //Port 2 pins
#if SATURN_PORTS > 1
  pinMode(SAT2_TH, OUTPUT);
  digitalWrite(SAT2_TH, HIGH);

  pinMode(SAT2_D0, INPUT_PULLUP);
  pinMode(SAT2_D1, INPUT_PULLUP);
  pinMode(SAT2_D2, INPUT_PULLUP);
  pinMode(SAT2_D3, INPUT_PULLUP);
  pinMode(SAT2_TL, INPUT_PULLUP);

  pinMode(SAT2_TR, INPUT_PULLUP);
#endif

  uint8_t i;
  for(i = 0; i < MAX_USB_STICKS; i++)
    for(uint8_t x = 0; x < 8; x++)
      lastState[i][x] = 1;

  delayMicroseconds(10);

  //detect multitap on ports
  for (i = 1; i <= SATURN_PORTS; i++) {
    currentPort = i;
    detectMultitap();
  }

  //limit to 7 usb controllers
  if (totalUsb > MAX_USB_STICKS)
    totalUsb = MAX_USB_STICKS;

  //create usb controllers
  for (i = 0; i < totalUsb; i++) {
    usbStick[i] = new Joystick_ (
      "RetroZordSat",
      JOYSTICK_DEFAULT_REPORT_ID + i,
      JOYSTICK_TYPE_GAMEPAD,
      10,     // buttonCount
      1,      // hatSwitchCount (0-2)
      true,   // includeXAxis
      true,   // includeYAxis
      false,  // includeZAxis
      false,  // includeRxAxis
      false,  // includeRyAxis
      false,  // includeRzAxis
      false,  // includeRudder
      true,   // includeThrottle
      false,  // includeAccelerator
      true,   // includeBrake
      false,   // includeSteering
      false,   // includeDial
      false   // includeWheel      
    );
  }

  //Set usb parameters and default values
  for (i = 0; i < totalUsb; i++) {
    usbStick[i]->begin(false);//disable automatic sendState
    usbStick[i]->setXAxisRange(0, 255);
    usbStick[i]->setYAxisRange(0, 255);
    usbStick[i]->setThrottleRange(0, 255);
    usbStick[i]->setBrakeRange(0, 255);

    usbStick[i]->setXAxis(128);
    usbStick[i]->setYAxis(128);
    usbStick[i]->setThrottle(0);
    usbStick[i]->setBrake(0);
    usbStick[i]->sendState();
  }
  
  delay(50);

  dstart (115200);
  //debugln (F("Powered on!"));
}

void saturnLoop() {
  unsigned long start = micros();
  joyCount = 0;

  //read saturn ports
  for (uint8_t i = 1; i <= SATURN_PORTS; i++) {
    currentPort = i;
    readSatPort();
  }

  //report usb hid
  reportUsb();

  //clear state changed
  joyStateChanged = B00000000;

  uint16_t sleepTime; //In micro seconds
  if (totalUsb > 2) //multitap mode
    sleepTime = 4000;
  else if (joyCount == 1) //one controller mode
    sleepTime = 1000;
  else //two controllers mode
    sleepTime = 2000;

  //sleep if total loop time was less than sleepTime
  unsigned long delta = micros() - start;
  if (delta < sleepTime) {
    //debugln(delta);
    delta = sleepTime - delta;
    delayMicroseconds(delta);
    //debugln(delta);
  }

  if (joyCount == 0) //blink led while no controller connected
    blinkLed();
}

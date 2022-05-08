/*******************************************************************************
 * Sega Saturn controller input library.
 * https://github.com/sonik-br/SaturnLib
 * 
 * The library depends on greiman's DigitalIO library.
 * https://github.com/greiman/DigitalIO
 * 
 * I recommend the usage of SukkoPera's fork of DigitalIO as it supports a few more platforms.
 * https://github.com/SukkoPera/DigitalIO
 * 
 * 
 * It works with saturn controllers (digital or analog) and multitaps.
 * Also works with MegaDrive controllers and mulltitaps.
*/

#include "../DigitalIO/DigitalIO.h"

#ifndef SATURNLIB_H_
#define SATURNLIB_H_

//Comment to disable the use of HOME button on 8bidto M30 2.4G.
//When present, it will report as saturn's L button.
#define ENABLE_8BITDO_HOME_BTN

//Max of 6 controllers per port (with a multitap)
#define MAX_CTRL 6

#define TAP_MEGA_PORTS 4
#define TAP_SAT_PORTS 6

#define SAT_ID_NONE B11111111
#define SAT_ID_DIGITALPAD B01001111
#define SAT_ID_MEGA B00001100

enum DB9_TR_Enum {
  DB9_TR_INPUT = 0,
  DB9_TR_OUTPUT
};

enum SatDeviceType_Enum {
  SAT_DEVICE_NONE = 0,
  SAT_DEVICE_NOTSUPPORTED,
  SAT_DEVICE_MEGA,
  SAT_DEVICE_PAD,
  SAT_DEVICE_3DPAD,
  SAT_DEVICE_WHEEL
};

enum SatDigital_Enum {
  SAT_PAD_UP    = 0x0001,
  SAT_PAD_DOWN  = 0x0002,
  SAT_PAD_LEFT  = 0x0004,
  SAT_PAD_RIGHT = 0x0008,
  SAT_B         = 0x0010,
  SAT_C         = 0x0020,
  SAT_A         = 0x0040,
  SAT_START     = 0x0080,
  SAT_Z         = 0x0100,
  SAT_Y         = 0x0200,
  SAT_X         = 0x0400,
  SAT_R         = 0x0800,
  SAT_L         = 0x8000
};

enum SatAnalog_Enum {
  SAT_ANALOG_X = 0,
  SAT_ANALOG_Y,
  SAT_ANALOG_L,
  SAT_ANALOG_R
};


struct SaturnControllerState {
  uint8_t id = SAT_ID_NONE;
  uint16_t digital = 0xFFFF; //Dpad and buttons
  uint8_t analogX = 0x80; //Wheel center is 0x7F ? I don't have one to test.
  uint8_t analogY = 0x80;
  uint8_t analogR = 0x00;
  uint8_t analogL = 0x00;
  /*bool operator==(const SaturnControllerState& b) const {
    return id == b.id &&
         digital == b.digital &&
         analogX == b.analogX &&
         analogY == b.analogY &&
         analogR == b.analogR &&
         analogL == b.analogL;
  }*/
  bool operator!=(const SaturnControllerState& b) const {
    return id != b.id ||
         digital != b.digital ||
         analogX != b.analogX ||
         analogY != b.analogY ||
         analogR != b.analogR ||
         analogL != b.analogL;
  }
};

class SaturnController {
  public:
        
    SaturnControllerState currentState;
    SaturnControllerState lastState;

    void reset(const bool resetId = false, bool resetPrevious = false) {
      if (resetId)
        currentState.id = 0xFF;

      currentState.digital = 0xFFFF;
      currentState.analogX = 0x80;
      currentState.analogY = 0x80;
      currentState.analogR = 0x00;
      currentState.analogL = 0x00;

      if (resetPrevious) {
        copyCurrentToLast();
      }
    }

    void copyCurrentToLast() {
      lastState.id = currentState.id;
      lastState.digital = currentState.digital;
      lastState.analogX = currentState.analogX;
      lastState.analogY = currentState.analogY;
      lastState.analogR = currentState.analogR;
      lastState.analogL = currentState.analogL;
    }


  bool deviceJustChanged() const { return currentState.id != lastState.id; }
  bool stateChanged() const { return currentState != lastState; }
  bool isAnalog() const { return (currentState.id & B00010000) == B00010000; }
  uint16_t digitalRaw() const { return currentState.digital; }  
  uint8_t hat() const { return currentState.digital & 0xF; }
  
  bool digitalPressed(const SatDigital_Enum s) const { return (currentState.digital & s) == 0; }
  bool digitalChanged (const SatDigital_Enum s) const { return ((lastState.digital ^ currentState.digital) & s) > 0; }
  bool digitalJustPressed (const SatDigital_Enum s) const { return digitalChanged(s) & digitalPressed(s); }
  bool digitalJustReleased (const SatDigital_Enum s) const { return digitalChanged(s) & !digitalPressed(s); }

  bool analogChanged (const SatAnalog_Enum s) const {
      switch (s) {
        case SAT_ANALOG_X:
          return currentState.analogX != lastState.analogX;
        case SAT_ANALOG_Y:
          return currentState.analogY != lastState.analogY;
        case SAT_ANALOG_R:
          return currentState.analogR != lastState.analogR;
        case SAT_ANALOG_L:
          return currentState.analogL != lastState.analogL;
        default:
          return false;
    }
  }

  uint8_t analog(const SatAnalog_Enum s) const {
    switch (s) {
      case SAT_ANALOG_X:
        return currentState.analogX;
      case SAT_ANALOG_Y:
        return currentState.analogY;
      case SAT_ANALOG_R:
        return currentState.analogR;
      case SAT_ANALOG_L:
        return currentState.analogL;
      default:
        return 0;
    }
  }


  SatDeviceType_Enum deviceType() const {
    if (currentState.id == SAT_ID_DIGITALPAD) { //L1001111 digital pad
      return SAT_DEVICE_PAD;
    } else if (currentState.id == B00000010) {//3d pad in digital mode
      return SAT_DEVICE_PAD;
    } else if (currentState.id == B00010110) {//3d pad in analog mode
      return SAT_DEVICE_3DPAD;
    } else if (currentState.id == B00010011) {//arcade racer wheel
      return SAT_DEVICE_WHEEL;
    } else if (currentState.id == SAT_ID_MEGA) { //megadrive pad directly connected
      return SAT_DEVICE_MEGA;
    } else if ((currentState.id >> 4) == B00001110 && (currentState.id & B00001111) < B00000011) { //mega on saturn multitap. 3 or 6 button. ignore mouse
      return SAT_DEVICE_MEGA;
    } else if (currentState.id == SAT_ID_NONE) {
      return SAT_DEVICE_NONE;
    } else {
      return SAT_DEVICE_NOTSUPPORTED;
    }
  }

};

template <uint8_t PIN_D0, uint8_t PIN_D1, uint8_t PIN_D2, uint8_t PIN_D3, uint8_t PIN_TH, uint8_t PIN_TR, uint8_t PIN_TL>
class SaturnPort {
  private:
    DigitalPin<PIN_D0> sat_D0; //U B Z .
    DigitalPin<PIN_D1> sat_D1; //D C Y .
    DigitalPin<PIN_D2> sat_D2; //L A X .
    DigitalPin<PIN_D3> sat_D3; //R S R L
    DigitalPin<PIN_TH> sat_TH;
    DigitalPin<PIN_TR> sat_TR;
    DigitalPin<PIN_TL> sat_TL;

    uint8_t joyCount = 0;
    uint8_t multitapPorts = 0;
    DB9_TR_Enum portState = DB9_TR_INPUT;
    SaturnController controllers[MAX_CTRL];

    inline void __attribute__((always_inline))
    setTR(const uint8_t value) { sat_TR.write(value); }
    
    inline void __attribute__((always_inline))
    setTH(const uint8_t value) { sat_TH.write(value); }
    
    inline bool __attribute__((always_inline))
    readTL() const { return sat_TL; }
    
    inline bool __attribute__((always_inline))
    readTR() const { return sat_TR; }
    
    inline uint8_t __attribute__((always_inline))
    waitTL(const uint8_t state) const { //returns 1 when reach timeout
      uint16_t t_out = 2000;
      const uint8_t compare = !state;
      while (readTL() == compare) {
        delayMicroseconds(4);
        if (t_out-- == 1)
          return 1;
      }
      return 0;
    }
    
    inline uint8_t __attribute__((always_inline))
    setTRAndWaitTL(const  uint8_t trValue) {
      setTR(trValue);
      return waitTL(trValue);
    }
    
    inline void __attribute__((always_inline))
    setTR_Mode(const DB9_TR_Enum mode) {
      if(portState != mode) {
        portState = mode;
        if(mode == DB9_TR_OUTPUT) {
          sat_TR.config(OUTPUT, HIGH);
        } else {
          sat_TR.config(INPUT, HIGH);
        }
      }
    }

    uint8_t readMegadriveBits() { //6 bits
      uint8_t nibble = readNibble();
      if(portState != DB9_TR_OUTPUT) {//only used on megadrive. not saturn
        bitWrite(nibble, 4, readTL());
        bitWrite(nibble, 5, readTR());
      }
      return nibble;
    }

    inline uint8_t __attribute__((always_inline))
    readNibble() const { return (sat_D3 << 3) + (sat_D2 << 2) + (sat_D1 << 1) + sat_D0; }

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
    
      if (portState == DB9_TR_OUTPUT)
        setTR(HIGH);
      
      delayMicroseconds(10);
    }
    
    void readMegaMultiTap() {
      uint8_t joyIndex = 0;
      uint8_t nibble_0;
      uint16_t tapPortState = B0;
      uint8_t nibbles;
      uint8_t tr = 0;
      uint8_t tl_timeout;
      uint8_t i;

      multitapPorts = TAP_MEGA_PORTS;
    
      //read first two nibbles. should be zero and zero
      for (i = 0; i < 2; i++) {
        tl_timeout = setTRAndWaitTL(i);
        if (tl_timeout)
          return;
        delayMicroseconds(4);
      }
    
      //read each port controller ID and store the 4 values in the 16 bits variable
      for (i = 0; i < 16; i+=4) {
        tl_timeout = setTRAndWaitTL(tr);
        if (tl_timeout)
          return;
        delayMicroseconds(4);
        
        tapPortState |= (readNibble() << i);
    
        tr ^= 1;
      }

      //SaturnController& sc = getSaturnController(joyIndex);
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
    
        if (nibbles == 6) {//ignore mouse. read and discard its data
            for (uint8_t x = 0; x < nibbles; x++) {
                tl_timeout = setTRAndWaitTL(tr);
                if (tl_timeout)
                    return;
                delayMicroseconds(4);
                tr ^= 1;
            }
        } else {
            joyIndex = joyCount++;
            SaturnController& sc = getSaturnController(joyIndex);
            sc.currentState.id = SAT_ID_MEGA;
            for (uint8_t x = 0; x < nibbles; x++) {
                tl_timeout = setTRAndWaitTL(tr);
                if (tl_timeout)
                    return;
                delayMicroseconds(4);

                setControlValues(sc, x, readNibble());

                tr ^= 1;
            }
        }
        
        //delayMicroseconds(40);
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
      } else {
        readThreeWireController(nibble_0, nibble_1);
      }
    }
    
    void readMultitap() {
      uint8_t i;
      uint8_t tl_timeout;

      multitapPorts = TAP_SAT_PORTS;
    
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
    
    void readUnhandledPeripheral(const uint8_t len) {
      const uint8_t nibbles = len * 2;
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

    void setControlValues(SaturnController& sc, const uint8_t page, const uint8_t nibbleTMP) const {
      if (page == 0) { //HAT RLDU
        sc.currentState.digital = (sc.currentState.digital & 0xFFF0) + nibbleTMP;
      } else if (page == 1) { //SACB
        sc.currentState.digital = (sc.currentState.digital & 0xFF0F) + (nibbleTMP << 4);
      } else if (page == 2) { //RXYZ
        sc.currentState.digital = (sc.currentState.digital & 0xF0FF) + (nibbleTMP << 8);
      } else if (page == 3) { //L
        sc.currentState.digital = (sc.currentState.digital & 0x0FFF) + (nibbleTMP << 12);
      } else if (page == 4) { //X axis
        sc.currentState.analogX = nibbleTMP;
      } else if (page == 5) { //Y axis
        sc.currentState.analogY = nibbleTMP;
      } else if (page == 6) { //RT axis
        sc.currentState.analogR = nibbleTMP;
      } else if (page == 7) { //LT axis
        sc.currentState.analogL = nibbleTMP;
      }
    }

    void readThreeWireController(const uint8_t controllerType, const uint8_t dataSize) {
      const uint8_t nibbles = dataSize * 2;
      uint8_t nibbleTMP;
      uint8_t nibble_0 = B0;
      uint8_t nibble_1;
      uint8_t tr = 0;
      uint8_t tl_timeout;
      //bool isAnalog = controllerType == B00000001;
    
      //check if it's a supported device
    
      if (controllerType == B00000000 && dataSize == B00000010) {
        //debugln (F("DIGITAL"));
      } else if (controllerType == B00000001 && dataSize == B00000110) {
        //debugln (F("ANALOG"));
      } else if (controllerType == B00000001 && dataSize == B00000011) {
        //debugln (F("WHEEL"));
      /*
        } else if (controllerType == B00000001 && dataSize == B00000101) {
          //debugln (F("MISSION STICK THREE-AXIS MODE"));
        } else if (controllerType == B00000001 && dataSize == B00001001) {
          //debugln (F("MISSION STICK SIX-AXIS MODE"));
      */
      } else if (controllerType == B00001110 && dataSize < B00000011) { //megadrive 3 or 6 button pad. ignore mouse
        //debugln (F("MEGADRIVE"));
      } else if (controllerType == B00001111 && dataSize == B00001111) {
        //debugln (F("NONE"));
        return;
      } else {
        //debugln (F("UNKNOWN"));
        readUnhandledPeripheral(dataSize);
        return;
      }

      const uint8_t joyIndex = joyCount++;
      SaturnController& sc = getSaturnController(joyIndex); //setControlValues
      
      sc.currentState.id = (controllerType << 4) + dataSize;
    
      for (uint8_t i = 0; i < nibbles; i++) {
        tl_timeout = setTRAndWaitTL(tr);
        if (tl_timeout)
          return;
          
        delayMicroseconds(2);
    
        nibbleTMP = readNibble();
        tr ^= 1;
    
        if (i < 3) { // RLDU SACB RXYZ
          setControlValues(sc, i, nibbleTMP);
        } else if(i == 3) { //L
          nibbleTMP &= B00001000;
          setControlValues(sc, i, nibbleTMP);
        } else if (i == 4) { //X axis
          nibble_0 = nibbleTMP;
        } else if (i == 5) { //X axis
          nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
          setControlValues(sc, 4, nibble_1);
        } else if (i == 6) { //Y axis
          nibble_0 = nibbleTMP;
        } else if (i == 7) { //Y axis
          nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
          setControlValues(sc, 5, nibble_1);
        } else if (i == 8) { //RT analog
          nibble_0 = nibbleTMP;
        } else if (i == 9) { //RT analog
          nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
          setControlValues(sc, 6, nibble_1);
        } else if (i == 10) { //LT analog
          nibble_0 = nibbleTMP;
        } else if (i == 11) { //LT analog
          nibble_1 = (nibble_0 & 0xF) << 4 | (nibbleTMP & 0xF);
          setControlValues(sc, 7, nibble_1);
        }
      }
    }

    void readMegadrivePad(uint8_t nibble_0, uint8_t nibble_1) {
      const uint8_t joyIndex = joyCount++;
      SaturnController& sc = getSaturnController(joyIndex);
      sc.currentState.id = SAT_ID_MEGA;

      //If on first read R and L are pressed then ignore reading.
      if((nibble_0 & B00001100) == B00000000)
        return;

      // RLDU
      setControlValues(sc, 0, nibble_0 & B00001111);
    
      // SACB
      setControlValues(sc, 1, (nibble_0 >> 4) | (nibble_1 >> 2));
    
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
        setControlValues(sc, 2, nibble_0 & B00001111);
    
        //SA1111
        //8bitdo M30 extra buttons
        //...S.H STAR and HOME
        
        //use HOME button as the missing L from saturn
        #ifdef ENABLE_8BITDO_HOME_BTN
          setControlValues(sc, 3, nibble_1 << 3);
        #endif
      }
    }

    void readDigitalPad(const uint8_t nibble_0, const uint8_t nibble_1) {
      const uint8_t joyIndex = joyCount++;
      uint8_t nibbleTMP;
      //uint8_t currentHatState;

      SaturnController& sc = getSaturnController(joyIndex);
      sc.currentState.id = SAT_ID_DIGITALPAD;

      // L100
      //debugln (nibble_0, BIN);
      setControlValues(sc, 3, nibble_0);
    
      // RLDU
      //setTH(LOW);
      //setTR(HIGH);
      //delayMicroseconds(4);
      //nibbleTMP = readNibble();
      //debugln (nibble_1, BIN);
      setControlValues(sc, 0, nibble_1);
    
      // SACB
      setTH(HIGH);
      setTR(LOW);
      delayMicroseconds(4);
      nibbleTMP = readNibble();
      //debugln (nibbleTMP, BIN);
      setControlValues(sc, 1, nibbleTMP);
    
      // RYXZ
      setTH(LOW);
      setTR(LOW);
      delayMicroseconds(4);
      nibbleTMP = readNibble();
      //debugln (nibbleTMP, BIN);
      setControlValues(sc, 2, nibbleTMP);
    }

  public:
    void begin(){
      //init output pins
      sat_TH.config(OUTPUT, HIGH);
    
      //init input pins with pull-up
      sat_D0.config(INPUT, HIGH);
      sat_D1.config(INPUT, HIGH);
      sat_D2.config(INPUT, HIGH);
      sat_D3.config(INPUT, HIGH);
      sat_TL.config(INPUT, HIGH);
    
      //init TR pin. Can change to input or output during execution.
      //Defaults to input (with pullup) to support megadrive controllers.
      sat_TR.config(INPUT, HIGH);

      multitapPorts = 0;
      //reset all devices to default values
      for (uint8_t i = 0; i < MAX_CTRL; i++) {
        getSaturnController(i).reset(true, true);
      }
    }
    
    void update(){
      //keep last data
      for (uint8_t i = 0; i < MAX_CTRL; i++) {
        getSaturnController(i).copyCurrentToLast();
      }
      
      uint8_t lastJoyCount = joyCount;
      joyCount = 0;
      multitapPorts = 0;
      
      //resetAll();
      
      readSatPort();

      //if device changed without connect/disconnect. eg: 3d pad analog <> digital
      for (uint8_t i = 0; i < joyCount; i++) {
        SaturnController& sc = getSaturnController(i);
        if (sc.currentState.id != sc.lastState.id) {
          //debugln(F("Resetting changed device"));
          sc.reset(false, false);
        }
      }

      //reset disconnected device status
      if (lastJoyCount > joyCount) {
        for (uint8_t i = joyCount; i < lastJoyCount; i++) {
          //debugln(F("Resetting disconnected device"));
          getSaturnController(i).reset(true, false);
        }
      }
      
    }

    //Call only on arduino power-on to check for multitap connected
    void detectMultitap() {
      const uint8_t nibbles = 2;
      uint8_t nibble_0;
      uint8_t nibble_1;
      uint8_t nibbleTMP;
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
        multitapPorts = TAP_MEGA_PORTS;
      } else if (nibble_0 == B00000001 && nibble_1 == B00000001) { //SATURN 3-WIRE-HANDSHAKE
        setTR_Mode(DB9_TR_OUTPUT);
        for (uint8_t i = 0; i < nibbles; i++) {
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
          multitapPorts = TAP_SAT_PORTS;
      }
    
      //reset TH and TR to high
      setTH(HIGH);
      
      if (portState == DB9_TR_OUTPUT)
        setTR(HIGH);
    
      delayMicroseconds(4);
    }

    SaturnController& getSaturnController(const uint8_t i) { return controllers[min(i, MAX_CTRL)]; }

    uint8_t getMultitapPorts() const { return multitapPorts; }
    uint8_t getControllerCount() const { return joyCount; }
};

#endif

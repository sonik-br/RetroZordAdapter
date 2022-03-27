/*******************************************************************************
 * Snes controller input library.
 * https://github.com/sonik-br/SnesLib
 * 
 * The library depends on greiman's DigitalIO library.
 * https://github.com/greiman/DigitalIO
 * 
 * I recommend the usage of SukkoPera's fork of DigitalIO as it supports a few more platforms.
 * https://github.com/SukkoPera/DigitalIO
 * 
 * 
 * Works with common snes pad and multitap.
 * Not tested with NES pad and NTT pad but should work.
 * DO NOT connect lightguns and other unsuported controllers
 * Multitap is detected only when powering the arduino.
*/

#include "../DigitalIO/DigitalIO.h"

#ifndef SNESLIB_H_
#define SNESLIB_H_

//#define SNES_ENABLE_MULTITAP

#ifdef SNES_ENABLE_MULTITAP
#define SNES_MAX_CTRL 4
#else
#define SNES_MAX_CTRL 1
#endif

enum SnesDeviceType_Enum {
  SNES_DEVICE_NONE = 0,
  SNES_DEVICE_NOTSUPPORTED,
  SNES_DEVICE_NES,
  SNES_DEVICE_PAD,
  SNES_DEVICE_NTT
};

enum SnesDigital_Enum {
  SNES_B      = 0x0001,
  SNES_Y      = 0x0002,
  SNES_SELECT = 0x0004,
  SNES_START  = 0x0008,
  SNES_UP     = 0x0010,
  SNES_DOWN   = 0x0020,
  SNES_LEFT   = 0x0040,
  SNES_RIGHT  = 0x0080,
  SNES_A      = 0x0100,
  SNES_X      = 0x0200,
  SNES_L      = 0x0400,
  SNES_R      = 0x0800
};

struct SnesControllerState {
  uint8_t id = SNES_DEVICE_NONE;
  uint16_t digital = 0x0; //Dpad and common buttons
  uint16_t extended = 0x0; //Extended data

  bool operator!=(const SnesControllerState& b) const {
    return id != b.id ||
         digital != b.digital ||
         extended != b.extended;
  }
};

class SnesController {
  public:
        
    SnesControllerState currentState;
    SnesControllerState lastState;

    void reset(const bool resetId = false, bool resetPrevious = false) {
      if (resetId)
        currentState.id = 0xFF;

      currentState.digital = 0x0;
      currentState.extended = 0x0;

      if (resetPrevious) {
        copyCurrentToLast();
      }
    }

    void copyCurrentToLast() {
      lastState.id = currentState.id;
      lastState.digital = currentState.digital;
      lastState.extended = currentState.extended;
    }

    bool deviceJustChanged() const { return currentState.id != lastState.id; }
    bool stateChanged() const { return currentState != lastState; }
    uint16_t digitalRaw() const { return currentState.digital; }
    uint16_t extendedRaw() const { return currentState.extended; }
    uint8_t hat() const { return ~currentState.digital >> 4 & 0xF; }
  
    bool digitalPressed(const SnesDigital_Enum s) const { return (currentState.digital & s) != 0; }
    bool digitalChanged (const SnesDigital_Enum s) const { return ((lastState.digital ^ currentState.digital) & s) > 0; }
    bool digitalJustPressed (const SnesDigital_Enum s) const { return digitalChanged(s) & digitalPressed(s); }
    bool digitalJustReleased (const SnesDigital_Enum s) const { return digitalChanged(s) & !digitalPressed(s); }
    
    SnesDeviceType_Enum deviceType() const {
      if (currentState.id == 0xF) {
        return SNES_DEVICE_PAD;
      } else if (currentState.id == 0xB) {
        return SNES_DEVICE_NTT;
      } else if (currentState.id == 0x0) {
        if (currentState.digital && 0xFF == 0xFF) //on NES controller ID is 0x0 and the non-existing buttons are pressed
          return SNES_DEVICE_NES;
        else
          return SNES_DEVICE_NONE;
      } else {
        return SNES_DEVICE_NOTSUPPORTED;
      }
    }
};


#ifdef SNES_ENABLE_MULTITAP
template <uint8_t PIN_CLOCK, uint8_t PIN_LATCH, uint8_t PIN_DATA1, uint8_t PIN_DATA2, uint8_t PIN_SELECT>
#else
template <uint8_t PIN_CLOCK, uint8_t PIN_LATCH, uint8_t PIN_DATA1>
#endif

class SnesPort {
  private:
    DigitalPin<PIN_CLOCK> SNES_CLOCK;
    DigitalPin<PIN_LATCH> SNES_LATCH;
    DigitalPin<PIN_DATA1> SNES_DATA1;

    #ifdef SNES_ENABLE_MULTITAP
      DigitalPin<PIN_DATA2> SNES_DATA2;
      DigitalPin<PIN_SELECT> SNES_MULTITAP;
    #endif

    uint8_t joyCount = 0;
    uint8_t multitapPorts = 0;
    SnesController controllers[SNES_MAX_CTRL];

    #ifdef SNES_ENABLE_MULTITAP
    void detectMultiTap() {
      //Do the strobe to start reading button values
      SNES_LATCH.write(HIGH);
      delayMicroseconds(4);
      
      if (SNES_DATA2 == 0x0)
        multitapPorts = 4;
      
      SNES_LATCH.write(LOW);
      delayMicroseconds(6);
    }
    #endif


    inline void __attribute__((always_inline))
    doStrobe() {
      SNES_LATCH.write(HIGH);
      delayMicroseconds(12);
      SNES_LATCH.write(LOW);
      delayMicroseconds(6);
    }

    
    inline void __attribute__((always_inline))
    doClockCicle() {
      SNES_CLOCK.write(LOW);
      delayMicroseconds(6);
      SNES_CLOCK.write(HIGH);
      delayMicroseconds(6);
    }

    inline uint8_t __attribute__((always_inline))
    getExtendCount(const uint8_t id1) const {
      if (id1 == 0xB) //NTT PAD
        return 16;
      else
        return 1;
    }

    inline void __attribute__((always_inline))
    setControllerValues(const unsigned int id, const unsigned int data, const unsigned int extented) {
        if (id == 0xF && extented == 0x1) { //no controller connected
          //sc.currentState.id = 0x0;
        } else {
          SnesController& sc = getSnesController(joyCount++);
          sc.currentState.id = id;
          sc.currentState.digital = ~data;
          sc.currentState.extended = ~extented;
        }
    }


    void readSingleController() {
      unsigned int fromController = 0x0;
      unsigned int fromControllerExtended = 0x0;
      uint8_t i;
      //uint8_t joyIndex = 0;
      
      //Do the strobe to start reading button values
      //doStrobe();

      for (i = 0; i < 16; i++) {
        //read the value, shift it and store it as a bit on fromController:
        fromController |= SNES_DATA1 << i;

        //More one cycle on the clock pin...
        doClockCicle();
      }

      const uint8_t id1 = (fromController >> 12);
      const uint8_t extend = getExtendCount(id1);
      
      for (i = 0; i < extend; i++) {
        //read the value, shift it and store it as a bit on fromController:
        fromControllerExtended |= SNES_DATA1 << i;

        //More one cycle on the clock pin...
        doClockCicle();
      }

      setControllerValues(id1, fromController, fromControllerExtended);
    }

    #ifdef SNES_ENABLE_MULTITAP
    void readMultitap() {
      uint8_t i;
      
      //Do the strobe to start reading button values
      //doStrobe();

      for (uint8_t multitappair = 0; multitappair < 2; multitappair++) {
        unsigned int fromController1 = 0x00;
        unsigned int fromControllerExtended1 = 0x00;
        unsigned int fromController2 = 0x00;
        unsigned int fromControllerExtended2 = 0x00;
        
        //select the controller pair to read
        if(multitappair == 0) //first pair of controllers
          SNES_MULTITAP.write(HIGH);
        else //next pair of controllers
          SNES_MULTITAP.write(LOW);
        delayMicroseconds(6);

        for (i = 0; i < 16; i++) {
          fromController1 |= SNES_DATA1 << i;
          fromController2 |= SNES_DATA2 << i;
          doClockCicle();
        }
  
        uint8_t id1 = (fromController1 >> 12);
        uint8_t id2 = (fromController2 >> 12);
        uint8_t extend1 = getExtendCount(id1);
        uint8_t extend2 = getExtendCount(id2);

        for (i = 0; i < max(extend1, extend2); i++) {
          fromControllerExtended1 |= SNES_DATA1 << i;
          fromControllerExtended2 |= SNES_DATA2 << i;
          doClockCicle();
        }

        setControllerValues(id1, fromController1, fromControllerExtended1);
        setControllerValues(id2, fromController2, fromControllerExtended2);
      }

      //leave it as default high
      SNES_MULTITAP.write(HIGH);
    }
    #endif


  public:
    void begin() {
      multitapPorts = 0;
      
      //init pins
      SNES_LATCH.config(OUTPUT, LOW);
      SNES_CLOCK.config(OUTPUT, HIGH);
      SNES_DATA1.config(INPUT, HIGH);
      
      #ifdef SNES_ENABLE_MULTITAP
        SNES_MULTITAP.config(OUTPUT, HIGH);
        SNES_DATA2.config(INPUT, HIGH);
        detectMultiTap();
      #endif
    }

    void update() {
      //keep last data
      for (uint8_t i = 0; i < SNES_MAX_CTRL; i++) {
        getSnesController(i).copyCurrentToLast();
      }

      uint8_t lastJoyCount = joyCount;
      joyCount = 0;
      //multitapPorts = 0;

      #ifdef SNES_ENABLE_MULTITAP
        //detectMultiTap(); //This will do the initial strobe
        doStrobe();
        if (multitapPorts == 0) {
          readSingleController();
        } else {
          readMultitap();
        }
      #else
        doStrobe();
        readSingleController();
      #endif

      //reset disconnected device status
      if (lastJoyCount > joyCount) {
        for (uint8_t i = joyCount; i < lastJoyCount; i++) {
          //debugln(F("Resetting disconnected device"));
          getSnesController(i).reset(true, false);
        }
      }
      
    }

    SnesController& getSnesController(const uint8_t i) { return controllers[min(i, SNES_MAX_CTRL)]; }

    uint8_t getMultitapPorts() const { return multitapPorts; }
    uint8_t getControllerCount() const { return joyCount; }
};

#endif

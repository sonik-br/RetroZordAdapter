/*******************************************************************************
 * Snes input module for RetroZord / Reflex-Adapt.
 * By Matheus Fraguas (sonik-br)
 * 
 * Handles a single input port.
 * 
 * Works with digital pad and analog pad.
 * By using the multitap it's possible to connect multiple controllers.
 * Also works with MegaDrive controllers and mulltitaps.
 *
 * Uses SnesLib
 * https://github.com/sonik-br/SnesLib
 *
 * Uses a modified version of MPG
 * https://github.com/sonik-br/MPG
 *
*/

//Uncomment to enable multitap support. Requires wiring two additional pins.
//#define SNES_ENABLE_MULTITAP

#include "RZInputModule.h"
#include "src/SnesLib/SnesLib.h"

//Snes pins
#define SNESPIN_CLOCK  8
#define SNESPIN_LATCH  9
#define SNESPIN_DATA1  10

#ifdef SNES_ENABLE_MULTITAP
  #define SNESPIN_DATA2  5
  #define SNESPIN_SELECT 6
  SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1, SNESPIN_DATA2, SNESPIN_SELECT> snes;
#else
  SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1> snes;
#endif

class RZInputSnes : public RZInputModule {
  private:
    #ifdef SNES_ENABLE_MULTITAP
      SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1, SNESPIN_DATA2, SNESPIN_SELECT> snes;
    #else
      SnesPort<SNESPIN_CLOCK, SNESPIN_LATCH, SNESPIN_DATA1> snes;
    #endif

    bool isVirtualBoy {false};
    
  public:
    RZInputSnes() : RZInputModule() { }

    const char* getUsbId() override {
      static const char* usbId1 { "RZMSnes" };
      static const char* usbId2 { "RZMVboy" };
      return isVirtualBoy ? usbId2 : usbId1;
    }

    void setup() override {
      //Init the class
      snes.begin();
    
      delayMicroseconds(10);
    
      //Multitap is connected?
       const uint8_t tap = snes.getMultitapPorts();
      if (tap == 0){ //No multitap connected during boot
        totalUsb = 2;
        //sleepTime = 200;
    
        //check if theres a virtualboy controller
        #ifdef SNES_ENABLE_VBOY
          snes1.update();
          for(uint8_t i = 0; i < snes1.getControllerCount(); ++i) {
            if (i == 0)
              isVirtualBoy = snes1.getSnesController(i).deviceType() == SNES_DEVICE_VB;
            snes.getSnesController(i).reset(true, true);
          }
        #endif
        
      } else { //Multitap connected
        totalUsb = MAX_USB_STICKS; //min(tap, MAX_USB_STICKS);
        sleepTime = 1000; //use longer interval between reads for multitap
      }

      delayMicroseconds(sleepTime);
    }

    void setup2() override { }
    
    bool read() override {
      static uint8_t lastControllerCount = 0;
      
      bool stateChanged = false;
    
      //Read snes port
      snes.update();
      
      //Get the number of connected controllers
      const uint8_t joyCount = snes.getControllerCount();
    
      for (uint8_t i = 0; i < joyCount; ++i) {
        if (i == totalUsb)
          break;
          
        //Get the data for the specific controller
        const SnesController& sc = snes.getSnesController(i);
        
        //Only process data if state changed from previous read
        if(sc.stateChanged()) {
          stateChanged = true;
          
          //Controller just connected.
          if (sc.deviceJustChanged()) {
            resetState(i);
          }
    
          const SnesDeviceType_Enum padType = sc.deviceType();
    
          state[i].dpad = 0
            | (sc.digitalPressed(SNES_UP)    ? GAMEPAD_MASK_UP    : 0)
            | (sc.digitalPressed(SNES_DOWN)  ? GAMEPAD_MASK_DOWN  : 0)
            | (sc.digitalPressed(SNES_LEFT)  ? GAMEPAD_MASK_LEFT  : 0)
            | (sc.digitalPressed(SNES_RIGHT) ? GAMEPAD_MASK_RIGHT : 0)
          ;
    
          if (padType == SNES_DEVICE_NES) {
            state[i].buttons = 0
              | (sc.digitalPressed(SNES_B) ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
              //| (sc.digitalPressed(SNES_A) ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
              | (sc.digitalPressed(SNES_Y) ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
              //| (sc.digitalPressed(SNES_X) ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
            ;
          } else {
            state[i].buttons = 0
              | (sc.digitalPressed(SNES_B) ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
              | (sc.digitalPressed(SNES_A) ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
              | (sc.digitalPressed(SNES_Y) ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
              | (sc.digitalPressed(SNES_X) ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
              | (sc.digitalPressed(SNES_L) ? GAMEPAD_MASK_L1 : 0) // Generic: P4, Switch: L, Xbox: LB
              | (sc.digitalPressed(SNES_R) ? GAMEPAD_MASK_R1 : 0) // Generic: P3, Switch: R, Xbox: RB
              //| (sc.digitalPressed(L2) ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
              //| (sc.digitalPressed(R2) ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
              //| (sc.digitalPressed(SNES_SELECT) ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
              //| (sc.digitalPressed(SNES_START) ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
              //| (sc.digitalPressed(LCLICK) ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
              //| (sc.digitalPressed(RCLICK) ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
            ;
    
            if(padType == SNES_DEVICE_NTT) {
//              bitWrite(buttonData, 6, sc.nttPressed(SNES_NTT_DOT));
//              bitWrite(buttonData, 7, sc.nttPressed(SNES_NTT_C));
//              bitWrite(buttonData, 10, sc.nttPressed(SNES_NTT_0));
//              bitWrite(buttonData, 11, sc.nttPressed(SNES_NTT_1));
//              bitWrite(buttonData, 12, sc.nttPressed(SNES_NTT_2));
//              bitWrite(buttonData, 13, sc.nttPressed(SNES_NTT_3));
//              bitWrite(buttonData, 14, sc.nttPressed(SNES_NTT_4));
//              bitWrite(buttonData, 15, sc.nttPressed(SNES_NTT_5));
//              bitWrite(buttonData, 16, sc.nttPressed(SNES_NTT_6));
//              bitWrite(buttonData, 17, sc.nttPressed(SNES_NTT_7));
//              bitWrite(buttonData, 18, sc.nttPressed(SNES_NTT_8));
//              bitWrite(buttonData, 19, sc.nttPressed(SNES_NTT_9));
//              bitWrite(buttonData, 20, sc.nttPressed(SNES_NTT_STAR));
//              bitWrite(buttonData, 21, sc.nttPressed(SNES_NTT_HASH));
//              bitWrite(buttonData, 22, sc.nttPressed(SNES_NTT_EQUAL));
            } else if(padType == SNES_DEVICE_VB) {
              state[i].buttons |=
                  (sc.nttPressed(SNES_NTT_0) ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
                | (sc.nttPressed(SNES_NTT_1) ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
              ;
            }
          }

          state[i].buttons |=
              (sc.digitalPressed(SNES_SELECT) ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
            | (sc.digitalPressed(SNES_START) ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
          ;
        }
      }
    
      //Controller has been disconnected? Reset it's values!
      if (lastControllerCount > joyCount) {
        for (uint8_t i = joyCount; i < lastControllerCount; ++i) {
          if (i == totalUsb)
            break;
          resetState(i);
        }
      } 
    
      //Keep count for next read
      lastControllerCount = joyCount;
      
      return joyCount != 0;
    } //end read

};

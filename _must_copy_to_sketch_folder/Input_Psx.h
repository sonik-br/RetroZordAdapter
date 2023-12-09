/*******************************************************************************
 * PlayStation input module for RetroZord / Reflex-Adapt.
 * By Matheus Fraguas (sonik-br)
 * 
 * Handles a single input port.
 *
 * Uses PsxNewLib
 * https://github.com/SukkoPera/PsxNewLib
 *
 * Uses a modified version of MPG
 * https://github.com/sonik-br/MPG
 *
*/

#include "RZInputModule.h"
#include "src/DigitalIO/DigitalIO.h"
#include "src/PsxNewLib/PsxControllerHwSpi.h"

//Guncon config
//0=Mouse, 1=Joy, 2=Joy OffScreenEdge (MiSTer)
//#define GUNCON_FORCE_MODE 2

//NeGcon config
//0=Default, 1=MiSTer Wheel format with paddle
//#define NEGCON_FORCE_MODE 1
//If you dont want to force a mode but instead change the default:
//Don't enable the force mode and edit the isNeGconMiSTer variable below as you desire.

//PlayStation pins
const byte PIN_PS1_ATT = 11;

const uint8_t SPECIALMASK_POPN = 0xE;
const uint8_t SPECIALMASK_JET  = 0xC;

class RZInputPsx : public RZInputModule {
  private:
    PsxControllerHwSpi<PIN_PS1_ATT> psx;

    bool isNeGcon { false };
    bool isJogcon { false };
    bool isGuncon { false };
    bool isNeGconMiSTer { false };
    uint8_t specialDpadMask { 0x0 };
    bool haveController { false };
    PsxControllerProtocol lastProto { PSPROTO_UNKNOWN };
    bool enableMouseMove { false }; //used on guncon and jogcon modes

    void tryEnableRumble() {
      if (options.inputMode == INPUT_MODE_XINPUT && !isJogcon){ //try to enable rumble
        if (psx.enterConfigMode ()) {
          psx.enableRumble ();
          psx.exitConfigMode ();
        }
      }
    }

    //Include sub-modules as private
    #ifdef GUNCON_SUPPORT
      #include "Input_Psx_Guncon.h"
    #endif
    #ifdef NEGCON_SUPPORT
      #include "Input_Psx_Negcon.h"
    #endif
    #ifdef JOGCON_SUPPORT
      #include "Input_Psx_Jogcon.h"
    #endif

    void handleDpad() {
      state[0].dpad = 0
        | (psx.buttonPressed(PSB_PAD_UP)    ? GAMEPAD_MASK_UP    : 0)
        | (psx.buttonPressed(PSB_PAD_DOWN)  ? GAMEPAD_MASK_DOWN  : 0)
        | (psx.buttonPressed(PSB_PAD_LEFT)  ? GAMEPAD_MASK_LEFT  : 0)
        | (psx.buttonPressed(PSB_PAD_RIGHT) ? GAMEPAD_MASK_RIGHT : 0)
      ;
      if (specialDpadMask)
        state[0].dpad = (state[0].dpad | specialDpadMask) & 0xF;
    }

    void loopDualShock() {
      static byte lastLX = { ANALOG_IDLE_VALUE };
      static byte lastLY = { ANALOG_IDLE_VALUE };
      static byte lastRX = { ANALOG_IDLE_VALUE };
      static byte lastRY = { ANALOG_IDLE_VALUE };
    
      byte analogX = ANALOG_IDLE_VALUE;
      byte analogY = ANALOG_IDLE_VALUE;
      //word convertedX, convertedY;
    
      //const bool digitalStateChanged = psx.buttonsChanged();//check if any digital value changed (dpad and buttons)
      
      const PsxControllerProtocol proto = psx.getProtocol();
    
      switch (proto) {
      case PSPROTO_DIGITAL:
      case PSPROTO_NEGCON:
      case PSPROTO_DUALSHOCK:
      case PSPROTO_DUALSHOCK2:
      case PSPROTO_FLIGHTSTICK:
      {
        handleDpad();
    
        //controller buttons
        state[0].buttons = 0
          | (psx.buttonPressed(PSB_CROSS)    ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
          | (psx.buttonPressed(PSB_CIRCLE)   ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
          | (psx.buttonPressed(PSB_SQUARE)   ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
          | (psx.buttonPressed(PSB_TRIANGLE) ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
          | (psx.buttonPressed(PSB_L1)       ? GAMEPAD_MASK_L1 : 0) // Generic: P4, Switch: L, Xbox: LB
          | (psx.buttonPressed(PSB_R1)       ? GAMEPAD_MASK_R1 : 0) // Generic: P3, Switch: R, Xbox: RB
          | (psx.buttonPressed(PSB_L2)       ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
          | (psx.buttonPressed(PSB_R2)       ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
          | (psx.buttonPressed(PSB_SELECT)   ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
          | (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
          | (psx.buttonPressed(PSB_L3)       ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
          | (psx.buttonPressed(PSB_R3)       ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
        ;
    
        //analog sticks
        if (psx.getLeftAnalog(analogX, analogY) && proto != PSPROTO_NEGCON) {
          state[0].lx = convertAnalog(analogX);
          state[0].ly = convertAnalog(analogY);
        } else {
          analogX = ANALOG_IDLE_VALUE;
          analogY = ANALOG_IDLE_VALUE;
          state[0].lx = convertAnalog(analogX);
          state[0].ly = convertAnalog(analogY);
        }
        
        lastLX = analogX;
        lastLY = analogY;
    
        if (psx.getRightAnalog(analogX, analogY) && proto != PSPROTO_NEGCON) {
          if (specialDpadMask == SPECIALMASK_JET)
            analogX = ANALOG_IDLE_VALUE;
          state[0].rx = convertAnalog(analogX);
          state[0].ry = convertAnalog(analogY);
        } else {
          analogX = ANALOG_IDLE_VALUE;
          analogY = ANALOG_IDLE_VALUE;
          state[0].rx = convertAnalog(analogX);
          state[0].ry = convertAnalog(analogY);
        }
        
        lastRX = analogX;
        lastRY = analogY;
    
        break;
      }
      default:
        break;
      }
    }


  public:
    RZInputPsx() : RZInputModule() { }

    const char* getUsbId() override {
      static const char* usbId_ds1       { "RZMPsDS1" };
      static const char* usbId_guncon    { "RZordPsGun" };
      static const char* usbId_negcon    { "RZMPsNeGcon" };
      static const char* usbId_negwheel  { "RZordPsWheel" };
      static const char* usbId_misterjog { "MiSTer-A1 RZJog" }; // "MiSTer-A1 JogCon"
      static const char* usbId_mousejog  { "RZMPSJogCon" };
      
      if (isGuncon)
        return usbId_guncon;
      else if (isNeGcon)
        return isNeGconMiSTer ? usbId_negwheel : usbId_negcon;
      else if (isJogcon)
        return enableMouseMove ? usbId_mousejog : usbId_misterjog;
      else
        return usbId_ds1;
    }

    void setup() override {
      //initialize the "attention" pin as OUTPUT HIGH.
      fastPinMode(PIN_PS1_ATT, OUTPUT);
    
      fastDigitalWrite(PIN_PS1_ATT, HIGH);
    
      PsxControllerProtocol proto = PSPROTO_UNKNOWN;
    
      if (psx.begin ()) {
        //delay(150);//200
        //haveController = true;
        haveController = true;
        //const PsxControllerProtocol proto = psx.getProtocol();
    
    
        //if not forced a mode, then read from currenct connected controller
        if(proto == PSPROTO_UNKNOWN)
          proto = psx.getProtocol();

        lastProto = proto;
    
        if (proto == PSPROTO_GUNCON) {
          isGuncon = true;
        } else if (proto == PSPROTO_NEGCON) {
          isNeGcon = true;
    
          //Configure NeGcon mode
          #if defined(NEGCON_FORCE_MODE) && NEGCON_FORCE_MODE >= 0 && NEGCON_FORCE_MODE < 2
            #if NEGCON_FORCE_MODE == 1
              isNeGconMiSTer = true;
            #endif
          #else //NEGCON_FORCE_MODE
            if (psx.buttonPressed(PSB_CIRCLE)) //NeGcon A / Volume B
              isNeGconMiSTer = !isNeGconMiSTer;
          #endif //NEGCON_FORCE_MODE
        } else { //jogcon can't be detected during boot as it needs to be in analog mode
    
    #ifdef JOGCON_SUPPORT
        //Try to detect by it's id
        if(proto == PSPROTO_DIGITAL) {
          if (psx.enterConfigMode ()) {
            if (psx.getControllerType () == PSCTRL_JOGCON) {
              isJogcon = true;
              if (psx.buttonPressed(PSB_L2))
                enableMouseMove = true;
            }
            psx.exitConfigMode ();
          }
        }
    #endif

          if (psx.buttonPressed(PSB_SELECT)) { //dualshock used in guncon mode to help map axis on emulators.
            isGuncon = true;
          }
          /*else if (proto == PSPROTO_JOGCON || psx.buttonPressed(PSB_L1)) {
            isJogcon = true;
          } else if (psx.buttonPressed(PSB_L2)) {
            isJogcon = true;
            enableMouseMove = true;
          }*/
        }

        tryEnableRumble();


      } else { //no controller connected
        if (proto == PSPROTO_JOGCON)
          isJogcon = true;
      }
    
      if (isNeGcon) {
        #ifdef NEGCON_SUPPORT
          negconSetup();
        #endif
      } else if (isJogcon) {
        #ifdef JOGCON_SUPPORT
          jogconSetup();
        #endif
      } else {
        if (isGuncon) {
          #ifdef GUNCON_SUPPORT
            gunconSetup();
          #endif
        } else { //dualshock [default]
          
          if (proto == PSPROTO_DIGITAL
          && psx.buttonPressed(PSB_PAD_DOWN)
          && psx.buttonPressed(PSB_PAD_LEFT)
          && psx.buttonPressed(PSB_PAD_RIGHT))
            specialDpadMask = SPECIALMASK_POPN;

          if (proto == PSPROTO_DUALSHOCK
          && psx.buttonPressed(PSB_PAD_LEFT)
          && psx.buttonPressed(PSB_PAD_RIGHT)) {
            byte analogX;
            byte analogY;
            if (psx.getRightAnalog(analogX, analogY) && analogX == 0xFF) {
              sleepTime = 1000;
              specialDpadMask = SPECIALMASK_JET;
            }
          }
            
          totalUsb = 1;//MAX_USB_STICKS;
          for (uint8_t i = 0; i < totalUsb; i++) {
            hasLeftAnalogStick[i] = true;
            hasRightAnalogStick[i] = true;
          }
        }
      }
    }//end setup

    void setup2() override {
      if (isNeGcon) {
        #ifdef NEGCON_SUPPORT
          negconSetup2();
        #endif
      } else if (isJogcon) {
        #ifdef JOGCON_SUPPORT
          jogconSetup2();
        #endif
      } else if (isGuncon) {
        #ifdef GUNCON_SUPPORT
          gunconSetup2();
        #endif
      }
    }

    bool read() override {
      static bool isReadSuccess = false;
      bool stateChanged = false;
    
      if(isJogcon) {
        #ifdef JOGCON_SUPPORT
          if (!haveController) {
            init_jogcon();
          } else {
            if(!psx.read()){
              haveController = false;
            } else {
              handleJogconData();
            }
          }
        #endif
        return haveController;
      }
    
   
      //Read ps port
  
      isReadSuccess = false;
      
      if (!haveController) {
        if (psx.begin()) {
          haveController = true;
          tryEnableRumble();
        }
      } else {
        const PsxControllerProtocol proto = psx.getProtocol();
        if (lastProto != proto)
          tryEnableRumble();
        lastProto = proto;
        psx.setRumble (rumble[0].right_power != 0x0, rumble[0].left_power);
        isReadSuccess = psx.read();
        if (!isReadSuccess){ //debug (F("Controller lost.")); debug (F(" last values: x = ")); debug (lastX); debug (F(", y = ")); debugln (lastY);
          haveController = false;
        }
      }
//        if(isGuncon)//only use first port for guncon
//          break;
  
  
      if (haveController && isReadSuccess) {
        if (isNeGcon) {
          #ifdef NEGCON_SUPPORT
            loopNeGcon();
          #endif
        } else if (isGuncon) {
          #ifdef GUNCON_SUPPORT
            loopGuncon();
          #endif
        } else {
          loopDualShock();
        }
      }

      return haveController;
    }//end read

};

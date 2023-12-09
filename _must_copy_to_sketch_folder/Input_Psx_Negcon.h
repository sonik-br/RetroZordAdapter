/*******************************************************************************
 * PlayStation (NEGCON) input module for RetroZord / Reflex-Adapt.
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
    
    void negconSetup() {
      totalUsb = 1;//MAX_USB_STICKS;
    }
    
    void negconSetup2() {
      //change hid mode
      if (isNeGconMiSTer && canChangeMode()) {
        options.inputMode = INPUT_MODE_HID_NEGCON;
      }
    }

    void loopNeGcon() {
      static byte lastX = { ANALOG_IDLE_VALUE };
      static uint8_t lastZ = { ANALOG_MAX_VALUE };
      static uint8_t lastCross = { ANALOG_MAX_VALUE };
      static uint8_t lastSquare = { ANALOG_MAX_VALUE };
      
//      bool stateChanged = psx.buttonsChanged();//check if any digital value changed (dpad and buttons)
    
      byte lx, ly;
      psx.getLeftAnalog (lx, ly);

      const bool isXinput = options.inputMode == INPUT_MODE_XINPUT || options.inputMode == INPUT_MODE_HID;
      const bool analogTriggers = canUseAnalogTrigger();
    
      //uses inversed values
      const uint8_t z = (uint8_t)~psx.getAnalogButton(PSAB_L1);
      const uint8_t cross =  isXinput ? psx.getAnalogButton(PSAB_CROSS)  : (uint8_t)~psx.getAnalogButton(PSAB_CROSS);
      const uint8_t square = isXinput ? psx.getAnalogButton(PSAB_SQUARE) : (uint8_t)~psx.getAnalogButton(PSAB_SQUARE);
    
      state[0].lx = convertAnalog(lx); //steering

      hasLeftAnalogStick[0] = true;

      if (options.inputMode == INPUT_MODE_HID_NEGCON) {
        hasAnalogTriggers[0] = true;
        hasRightAnalogStick[0] = true;
        
        state[0].ly = convertAnalog(z);//z

        state[0].rx = convertAnalog(cross);  //I throttle
        state[0].ry = convertAnalog(square); //II brake

        state[0].buttons = 0
          | (psx.buttonPressed(PSB_TRIANGLE) ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
          | (psx.buttonPressed(PSB_R1)       ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
          | (psx.buttonPressed(PSB_CIRCLE)   ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
          | (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
          //| (psx.buttonPressed(PSB_L1)       ? GAMEPAD_MASK_L1 : 0) // Generic: P4, Switch: L, Xbox: LB
          //| (psx.buttonPressed(PSB_R1)       ? GAMEPAD_MASK_R1 : 0) // Generic: P3, Switch: R, Xbox: RB
          //| (psx.buttonPressed(PSB_L2)       ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
          //| (psx.buttonPressed(PSB_R2)       ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
          //| (psx.buttonPressed(PSB_SELECT)   ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
          //| (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
          //| (psx.buttonPressed(PSB_L3)       ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
          //| (psx.buttonPressed(PSB_R3)       ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
        ;
        
      } else { //xinput,switch,ps3...
        hasRightAnalogStick[0] = false;
        
        if (analogTriggers) {//xinput,ps3
          hasAnalogTriggers[0] = true;
          state[0].rt = cross;  //I throttle
          state[0].lt = square; //II brake

          state[0].buttons = 0
            | (psx.buttonPressed(PSB_CIRCLE)   ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
            | (psx.buttonPressed(PSB_TRIANGLE)   ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
            //| (psx.buttonPressed(PSB_CIRCLE)   ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
            //| (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
            | (psx.buttonPressed(PSB_L1)       ? GAMEPAD_MASK_L1 : 0) // Generic: P4, Switch: L, Xbox: LB
            | (psx.buttonPressed(PSB_R1)       ? GAMEPAD_MASK_R1 : 0) // Generic: P3, Switch: R, Xbox: RB
            //| (psx.buttonPressed(PSB_L2)       ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
            //| (psx.buttonPressed(PSB_R2)       ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
            //| (psx.buttonPressed(PSB_SELECT)   ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
            | (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
            //| (psx.buttonPressed(PSB_L3)       ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
            //| (psx.buttonPressed(PSB_R3)       ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
          ;

        } else { //swtich
          hasAnalogTriggers[0] = false;

          state[0].buttons = 0
            | (psx.buttonPressed(PSB_TRIANGLE)    ? GAMEPAD_MASK_B1 : 0) // Generic: K1, Switch: B, Xbox: A
            | (psx.buttonPressed(PSB_CIRCLE)   ? GAMEPAD_MASK_B2 : 0) // Generic: K2, Switch: A, Xbox: B
            //| (psx.buttonPressed(PSB_SQUARE)   ? GAMEPAD_MASK_B3 : 0) // Generic: P1, Switch: Y, Xbox: X
            //| (psx.buttonPressed(PSB_CROSS) ? GAMEPAD_MASK_B4 : 0) // Generic: P2, Switch: X, Xbox: Y
            | (psx.buttonPressed(PSB_L1)       ? GAMEPAD_MASK_L1 : 0) // Generic: P4, Switch: L, Xbox: LB
            | (psx.buttonPressed(PSB_R1)       ? GAMEPAD_MASK_R1 : 0) // Generic: P3, Switch: R, Xbox: RB
            | (psx.buttonPressed(PSB_SQUARE)   ? GAMEPAD_MASK_L2 : 0) // Generic: K4, Switch: ZL, Xbox: LT (Digital)
            | (psx.buttonPressed(PSB_CROSS)    ? GAMEPAD_MASK_R2 : 0) // Generic: K3, Switch: ZR, Xbox: RT (Digital)
            //| (psx.buttonPressed(PSB_SELECT)   ? GAMEPAD_MASK_S1 : 0) // Generic: Select, Switch: -, Xbox: View
            | (psx.buttonPressed(PSB_START)    ? GAMEPAD_MASK_S2 : 0) // Generic: Start, Switch: +, Xbox: Menu
            //| (psx.buttonPressed(PSB_L3)       ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
            //| (psx.buttonPressed(PSB_R3)       ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
          ;
        }
        
        state[0].ly = convertAnalog(0x80);
      }



//todo use this:
//      ((Joy1_*)usbStick[outputIndex])->setAnalog0(lx); //steering
//      ((Joy1_*)usbStick[outputIndex])->setAnalog1(z); //z
//      ((Joy1_*)usbStick[outputIndex])->setAnalog2(cross); //I throttle
//      ((Joy1_*)usbStick[outputIndex])->setAnalog3(square); //II brake
//      if(isNeGconMiSTer)
//        ((Joy1_*)usbStick[0])->setAnalog4(lx); //paddle

      
      
      //state[outputIndex].rx = convertAnalog(cross);//I throttle
      //state[outputIndex].ry = convertAnalog(square);//II brake

    
      //combine the two axes and use only half precision for each
      //const int btnI_II = psx.getAnalogButton(PSAB_SQUARE) - psx.getAnalogButton(PSAB_CROSS);
      //const uint8_t btnI_II = ((psx.getAnalogButton(PSAB_SQUARE) - psx.getAnalogButton(PSAB_CROSS)) >> 1) + ANALOG_IDLE_VALUE;
    
//      uint8_t buttonData = 0;
//      bitWrite(buttonData, 0, psx.buttonPressed(PSB_CIRCLE)); //A
//      bitWrite(buttonData, 1, psx.buttonPressed(PSB_TRIANGLE)); //B
//      bitWrite(buttonData, 2, psx.buttonPressed(PSB_R1));
//      bitWrite(buttonData, 3, psx.buttonPressed(PSB_START)); //Start


      handleDpad();
    
//      if (lastX != lx || lastZ != z || lastCross != cross || lastSquare != square)
//        stateChanged = true;
    
      lastX = lx;
      lastZ = z;
      lastCross = cross;
      lastSquare = square;
    }

/*******************************************************************************
 * PlayStation (JOGCON) input module for RetroZord / Reflex-Adapt.
 * By Matheus Fraguas (sonik-br)
 * 
 * Handles a single input port.
 * 
 * Based on MiSTer-A1 JogCon (JogConUSB) by sorgelig.
 * https://github.com/MiSTer-devel/Retro-Controllers-USB-MiSTer/tree/master/JogConUSB
 *
 * Uses PsxNewLib
 * https://github.com/SukkoPera/PsxNewLib
 *
 * Uses a modified version of MPG
 * https://github.com/sonik-br/MPG
 *
*/

byte ff;
byte mode;
byte force;
int16_t sp_step;

uint8_t sp_div;
int16_t sp_max;
int16_t sp_half;

#define SP_MAX  160

void init_jogcon() {
  if (psx.begin()) {
    haveController = true;

    //delay(300);
    
    if (!psx.enterConfigMode ()) {
      //Serial.println (F("Cannot enter config mode"));
    } else {
      //must enable analog mode to use jogcon's paddle
      psx.enableAnalogSticks ();
      //must enable rumble to use the jogcon's motor
      psx.enableRumble ();
      psx.exitConfigMode ();
    }
    psx.read ();    // Make sure the protocol is up to date
  }
}

bool handleJogconData()
{
  static uint16_t counter = 0, newcnt = 0, cleancnt = 0;
  //static uint16_t cleancnt = 0;
  static uint16_t newbtn = 0, oldbtn = 0;
  static int8_t oldspinner = 0;
  static uint8_t oldpaddle = 0;
  static int32_t pdlpos = sp_half;
  static uint16_t prevcnt = 0;
  static int8_t btntimeout = 0;//for fake spinner mode
  static JogconDirection lastDirection = JOGCON_DIR_NONE;

  bool stateChanged = false;
  
  uint8_t jogPosition = 0;
  uint8_t jogRevolutions = 0;
  JogconDirection jogDirection = JOGCON_DIR_NONE;
  JogconCommand cmdResult = JOGCON_CMD_NONE;
  
  bool gotJogconData = psx.getJogconData(jogPosition, jogRevolutions, jogDirection, cmdResult);

  JogconCommand nextCmd = JOGCON_CMD_NONE;
  if(jogDirection == JOGCON_DIR_MAX) { //reached max internal counter
    nextCmd = JOGCON_CMD_DROP_REVOLUTIONS;
    jogDirection = lastDirection;
    if (jogDirection == JOGCON_DIR_CW) {
      jogPosition = 254;
      jogRevolutions = 255;
    } else { //CCW
      jogPosition = 254;
      jogRevolutions = 0;
    }
    prevcnt = 0;
    cleancnt = 0;
    counter = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
  }
  
  if(jogDirection != JOGCON_DIR_NONE)
    lastDirection = jogDirection;

  newcnt = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
  newbtn = psx.getButtonWord();//(data[3] << 8) | data[2];
  newbtn = (newbtn & ~3) | ((newbtn&1)<<2);

  //if(jogStatus == JOGCON_STATE_OTHER) {//(data[0] == 0xF3)
  if(!gotJogconData) {//(data[0] == 0xF3)
    // Mode switch by pressing "mode" button while holding:
    // L2 - paddle mode (with FF stoppers)
    // R2 - steering mode (FF always enabled)
    // L2+R2 - spinner mode (no FF)
    // L1+R1 - fake spinner mode. send as buttons (no FF)
    if(psx.buttonPressed(PSB_L2) && psx.buttonPressed(PSB_R2))
      mode = 0;
    else if(psx.buttonPressed(PSB_L1) && psx.buttonPressed(PSB_R1))
      mode = 3;
    else if(psx.buttonPressed(PSB_L2))
      mode = 1;
    else if(psx.buttonPressed(PSB_R2))
      mode = 2;


    // Force Feedback adjust
    // by pressing "mode" button while holding /\,O,X,[]
    if(psx.buttonPressed(PSB_TRIANGLE))
      force = 1;
    else if(psx.buttonPressed(PSB_CIRCLE))
      force = 3;
    else if(psx.buttonPressed(PSB_CROSS))
      force = 7;
    else if(psx.buttonPressed(PSB_SQUARE))
      force = 15;

    // Spinner pulses per step adjust
    // by pressing "mode" button while holding up,right,down,left
    if(psx.buttonPressed(PSB_PAD_UP))
      sp_step = 1;
    else if(psx.buttonPressed(PSB_PAD_RIGHT))
      sp_step = 2;
    else if(psx.buttonPressed(PSB_PAD_DOWN))
      sp_step = 4;
    else if(psx.buttonPressed(PSB_PAD_LEFT))
      sp_step = 8;

    // Paddle range switch by pressing "mode" button while holding:
    // L1 - 270 degree
    // R1 - 135 degree
    if(psx.buttonPressed(PSB_L1) ^ psx.buttonPressed(PSB_R1)) { //L 2, R 1
      sp_div = psx.buttonPressed(PSB_L1) ? 2 : 1;
      sp_max = SP_MAX/sp_div;
      sp_half = sp_max/2;
    }

    // some time for visual confirmation
    delay(200);

    // reset zero position
    init_jogcon();
    
    gotJogconData = psx.getJogconData(jogPosition, jogRevolutions, jogDirection, cmdResult);

    prevcnt = 0;
    cleancnt = 0;
    counter = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
    pdlpos = sp_half;
  } else {
    
    if(jogDirection != JOGCON_DIR_NONE) {
      cleancnt += newcnt - counter;
      if(mode == 0 || mode == 3)//!mode
      {
        ff = 0;
        pdlpos += (int16_t)(newcnt - counter);
        if(pdlpos<0) pdlpos = 0;
        if(pdlpos>sp_max) pdlpos = sp_max;
      }
    }

    if(mode == 1 || mode == 2)//mode
    {
      if(((int16_t)newcnt) < -sp_half)
      {
        pdlpos = 0;
        if(mode == 1) ff = 1;
      }
      else if(((int16_t)newcnt) > sp_half)
      {
        pdlpos = sp_max;
        if(mode == 1) ff = 1;
      }
      else
      {
        if(mode == 1) ff = 0;
        pdlpos = (uint16_t)(newcnt + sp_half);
      }
    }

    if(mode == 2) ff = 1;

    if (ff == 1)
      psx.setJogconMotorMode(JOGCON_DIR_START, nextCmd, force);
    else 
      psx.setJogconMotorMode(JOGCON_DIR_NONE, nextCmd, 0);

    int16_t val = ((int16_t)(cleancnt - prevcnt))/sp_step;
    if(val>127) val = 127; else if(val<-127) val = -127;
    prevcnt += val*sp_step;
    const int8_t spinner = val;
    
    const uint8_t paddle = ((pdlpos*255)/sp_max);

    if (mode == 3) { //fake spinner mode. send pulses as button press
      if(spinner < 0)
        btntimeout = -64;
      else if(spinner > 0)
        btntimeout = 63;
      bitWrite(newbtn, 0, btntimeout < 0); //LEFT
      bitWrite(newbtn, 1, btntimeout > 0); //RIGHT
    }



    {
      if(oldbtn != newbtn || oldpaddle != paddle || oldspinner != spinner) {
        stateChanged = true;
        oldbtn = newbtn;
        oldpaddle = paddle;
        oldspinner = spinner;

        //const int16_t btn = (newbtn & 0xF) | ((newbtn>>4) & ~0xF);
        //for (int8_t i = 2; i < 14; i++)
        //  ((Jogcon1_*)usbStick[0])->setButton(i, bitRead(btn, i));

        handleDpad();
        //((Jogcon1_*)usbStick[0])->setButtons(newbtn & 0xFF0F); //buttons except dpad
        //state[outputIndex].buttons = newbtn & 0xFF0F;MiSTer-A1
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
          | (newbtn & 0x01                    ? GAMEPAD_MASK_L3 : 0) // All: Left Stick Click
          | (newbtn & 0x02                    ? GAMEPAD_MASK_R3 : 0) // All: Right Stick Click
        ;

        if(enableMouseMove) {
#ifdef ENABLE_PSX_JOGCON_MOUSE
          //RelMouse->setXAxis(spinner);
          //RelMouse->sendState();
          state[0].aux = spinner; //spinner
#endif
        } else {
          hasLeftAnalogStick[0] = true;
          //((Jogcon1_*)usbStick[0])->setPaddle(paddle);
          //((Jogcon1_*)usbStick[0])->setSpinner(spinner);
          state[0].lx = convertAnalog(paddle); //paddle
          state[0].aux = spinner; //spinner
        }
        //usbStick[0]->sendState();
      }
    }
  }
  counter = newcnt;

  //time out the spinner as button
  if (mode == 3) {
    if(btntimeout < 0)
      btntimeout ++;
    else if(btntimeout > 0)
      btntimeout --;
  }
  return stateChanged;
}

void jogconSetup() {
  mode = 0;
  force = 7;
  sp_step = 4;
  sp_div = 1;
  sp_max = SP_MAX/sp_div;
  sp_half = sp_max/2;

  totalUsb = 1;//MAX_USB_STICKS;

  //enableMouseMove = haveController && psx.buttonPressed(PSB_SELECT);


//#ifdef ENABLE_PSX_JOGCON_MOUSE
//  if(enableMouseMove) {
//      RelMouse = new MouseRel1_("RZordPSPSJogCon", JOYSTICK_DEFAULT_REPORT_ID, 2);
//      //RelMouse->begin (false);
//      RelMouse->resetState();//todo send?
//      RelMouse->sendState();
//    usbStick[0] = new Jogcon1_("RZordPSPSJogCon", JOYSTICK_DEFAULT_REPORT_ID + 1, JOYSTICK_TYPE_GAMEPAD, 2);
//  } else
//#endif
//  {
//    usbStick[0] = new Jogcon1_("MiSTer-A1 JogCon", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 1);
//  }
//  usbStick[0]->resetState();
//  usbStick[0]->sendState();
//
//#ifdef ENABLE_PSX_JOGCON_MOUSE
//  if (enableMouseMove)
//    delay(200);
//#endif
}

void jogconSetup2() {
  //change hid mode
  if (canChangeMode()) {
    options.inputMode = INPUT_MODE_HID_JOGCON;
    #ifdef ENABLE_PSX_JOGCON_MOUSE
      if (enableMouseMove) {
        options.inputMode = INPUT_MODE_HID_JOGCON_MOUSE;
        totalUsb = 2; //mouse mode uses gamepad + mouse devices
      }
    #endif
  }
  
  #ifdef ENABLE_PSX_JOGCON_MOUSE
    if (enableMouseMove)
      delay(200);
  #endif
}

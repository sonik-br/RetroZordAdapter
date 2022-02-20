/*******************************************************************************
 * This sketch turns a PSX Guncon controller into a USB absolute mouse
 * or Joystick, using an Arduino Leonardo.
 *
 * It uses the PsxNewLib, ArduinoJoystickLibrary
 * and an edited version of AbsMouse Library.
 *
 * For details on PsxNewLib, see
 * https://github.com/SukkoPera/PsxNewLib
 *
 * For details on ArduinoJoystickLibrary, see
 * https://github.com/MHeironimus/ArduinoJoystickLibrary
 *
 * For details on AbsMouse, see
 * https://github.com/jonathanedgecombe/absmouse
 *
 * The guncon needs to "scan" the entire screen before it can properly send
 * the coorinates. Just point it at the screen and move slowly from side to side
 * and top to botom. The values will be stored as min and max, and will be used
 * to calculate the absolute mouse position.
 * It's recommended to use a full screen white image when doing this.
 *
 * When connected it will be in a not enabled state.
 * It can emulate a Mouse or a Joystick:
 *
 * Press Gun Trigger / Circle to enable mouse emulation.
 * Press Gun A (Left side) / Start to enable joystick emulation.
 *
 * To disable, point it off screen and press A + B + Trigger
 *
 * Buttons are mapped as follows:
 * A (Left side) / Start -> Mouse Right / Joystick btn 1
 * B (Right side) / Cross -> Mouse Middle / Joystick btn 2
 * Trigger / Circle -> Mouse Left / Joystick btn 0
*/

//#include <PsxControllerHwSpi.h>
#include "src/PsxNewLib/PsxControllerHwSpi.h"
#include "src/AbsMouse/AbsMouse.h" //absolute mouse
#include "src/ArduinoJoystickLibrary/Mouse.h" //relative mouse

const byte PIN_PS2_ATT = 11;

const unsigned long POLLING_INTERVAL = 1000U / 400U;//needs more testing

// Send debug messages to serial port
//#define ENABLE_SERIAL_DEBUG

PsxControllerHwSpi<PIN_PS2_ATT> psx;

const byte ANALOG_DEAD_ZONE = 25U;

const word maxMouseValue = 32767;

//min and max possible values
//from document at http://problemkaputt.de/psx-spx.htm#controllerslightgunsnamcoguncon
//x is 77 to 461
//y is 25 to 248 (ntsc). y is 32 to 295 (pal)
//from personal testing on a pvm
//x is 72 to 450. with underscan x 71 to 453
//y is 22 to 248. with underscan y is 13 to 254 (ntsc)

const unsigned short int minPossibleX = 77;
const unsigned short int maxPossibleX = 461;
const unsigned short int minPossibleY = 25;
const unsigned short int maxPossibleY = 295;

const byte maxNoLightCount = 10;

boolean haveController = false;

short calibrationStep = 0;

// Minimum and maximum detected values. Varies from tv to tv.
// Values will be detected when pointing at the screen.
word minX = 1000;
word maxX = 0;
word minY = 1000;
word maxY = 0;

word offsetX = 0;
word offsetY = 0;

unsigned char noLightCount = 0;

// Last successful read coordinates
word lastX = -1;
word lastY = -1;

boolean isNeGcon = false;
boolean isJogcon = false;
boolean isGuncon = false;

boolean enableReport = false;
boolean enableMouseMove = false;
boolean enableJoystick = false;

AbsMouse_* AbsMouse;
Mouse_* Mouse;

word convertRange(double gcMin, double gcMax, double value) {
    double scale = maxMouseValue / (gcMax - gcMin);
    return (word)((value - gcMin) * scale);
}

/*word convertRange(word gcMin, word gcMax, word value) {
  word scale = (word)(maxMouseValue / (gcMax - gcMin));
  return (word)((value - gcMin) * scale);
}*/

//Begin Jogcon things

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

//End Jogcon things





void moveToCoords(word x, word y) {
    if (enableMouseMove)
        AbsMouse->move(x, y);

    if (enableJoystick) {
        usbStick[0]->setXAxis(x);
        usbStick[0]->setYAxis(y);
    }
}

void releaseAllButtons() {
    if (enableMouseMove) {
        AbsMouse->release(MOUSE_LEFT);
        AbsMouse->release(MOUSE_RIGHT);
        AbsMouse->release(MOUSE_MIDDLE);
        AbsMouse->report();
    }

    if (enableJoystick) {
      for (int i = 0; i < 10; i++)
        usbStick[0]->releaseButton(i);
      usbStick[0]->sendState();
    }
}

void readGuncon() {
    word x, y, convertedX, convertedY;
    GunconStatus gcStatus;
    gcStatus = psx.getGunconCoordinates(x, y); //use coords from guncon

  /*
    if (gcStatus == GUNCON_OK)
      debugln (F("STATUS: GUNCON_OK!"));
    else if (gcStatus == GUNCON_UNEXPECTED_LIGHT)
      debugln (F("STATUS: GUNCON_UNEXPECTED_LIGHT!"));
    else if (gcStatus == GUNCON_NO_LIGHT)
      debugln (F("STATUS: GUNCON_NO_LIGHT!"));
    else
      debugln (F("STATUS: GUNCON_OTHER_ERROR!"));
  */

    if (gcStatus == GUNCON_OK) {
        noLightCount = 0;
        // is inside possible range?
        if (x >= minPossibleX && x <= maxPossibleX && y >= minPossibleY && y <= maxPossibleY) {
            //x += offsetX;
            //y += offsetY;
            lastX = x;
            lastY = y;

            //got new min or max values?
            if (x < minX)
                minX = x;
            else if (x > maxX)
                maxX = x;

            if (y < minY)
                minY = y;
            else if (y > maxY)
                maxY = y;

            if (enableMouseMove || enableJoystick) {
                convertedX = convertRange(minX, maxX, x);
                convertedY = convertRange(minY, maxY, y);
                moveToCoords(convertedX + offsetX, convertedY + offsetY);
            }
        }
    }
    else if (gcStatus == GUNCON_NO_LIGHT) {

        //up to 10 no_light reads will report the last good values
        if (lastX != 0 && lastY != 0) {
            convertedX = convertRange(minX, maxX, lastX);
            convertedY = convertRange(minY, maxY, lastY);

            moveToCoords(convertedX, convertedY);

            noLightCount++;

            if (noLightCount > maxNoLightCount) {
                noLightCount = 0;
                lastX = 0;
                lastY = 0;

                //set it offscreen (bottom left). need to test
                //also release all buttons
                if (enableMouseMove)
                    AbsMouse->move(0, maxMouseValue);

                //put joystick to the center position
                if (enableJoystick) {
                    usbStick[0]->setXAxis(16383);
                    usbStick[0]->setYAxis(16383);
                }

                releaseAllButtons();
            }
        }
        else if (psx.buttonPressed(PSB_CIRCLE) && psx.buttonPressed(PSB_START) && psx.buttonPressed(PSB_CROSS)) {//only when using guncon. dualshock wont work
            enableReport = false;
            releaseAllButtons();
            delay(1000);
        }

    }
}

void analogDeadZone(byte& value) {
    int8_t delta = value - ANALOG_IDLE_VALUE;
    if (abs(delta) < ANALOG_DEAD_ZONE)
        value = ANALOG_IDLE_VALUE;
}

void readDualShock() {
    word x, y;
    byte analogX = ANALOG_IDLE_VALUE;
    byte analogY = ANALOG_IDLE_VALUE;
    if (psx.getLeftAnalog(analogX, analogY)) { //use coords from analog controller
        analogDeadZone(analogX);
        analogDeadZone(analogY);
    }
    x = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
    y = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
    moveToCoords(x, y);

    //right analog stick
    if (enableJoystick) {
        analogX = ANALOG_IDLE_VALUE;
        analogY = ANALOG_IDLE_VALUE;
        if (psx.getRightAnalog(analogX, analogY)) { //use coords from analog controller
            analogDeadZone(analogX);
            analogDeadZone(analogY);
        }
        x = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
        y = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
        usbStick[0]->setRxAxis(x);
        usbStick[0]->setRyAxis(y);
    }

    /*if (psx.buttonPressed(PSB_SELECT)) {
        enableReport = false;
        releaseAllButtons();
        delay(1000);
    }*/
}

void handleDpad() {
  int16_t dpad = B0;
  bitWrite(dpad, 0, !psx.buttonPressed (PSB_PAD_UP));
  bitWrite(dpad, 1, !psx.buttonPressed (PSB_PAD_DOWN));
  bitWrite(dpad, 2, !psx.buttonPressed (PSB_PAD_LEFT));
  bitWrite(dpad, 3, !psx.buttonPressed (PSB_PAD_RIGHT));
  usbStick[0]->setHatSwitch(0, hatTable[dpad]);
}

void handleButtons() {
    if (psx.buttonJustPressed(PSB_CIRCLE)) { //trigger press
        AbsMouse->press(MOUSE_LEFT);
        usbStick[0]->pressButton(2);
    }
    else if (psx.buttonJustReleased(PSB_CIRCLE)) { //trigger release
        AbsMouse->release(MOUSE_LEFT);
        usbStick[0]->releaseButton(2);
    }

    if (psx.buttonJustPressed(PSB_START)) { //A button press
        AbsMouse->press(MOUSE_RIGHT);
        usbStick[0]->pressButton(9);
    }
    else if (psx.buttonJustReleased(PSB_START)) { //A button release
        AbsMouse->release(MOUSE_RIGHT);
        usbStick[0]->releaseButton(9);
    }

    if (psx.buttonJustPressed(PSB_CROSS)) { //B button press
        AbsMouse->press(MOUSE_MIDDLE);
        usbStick[0]->pressButton(1);
    }
    else if (psx.buttonJustReleased(PSB_CROSS)) { //B button release
        AbsMouse->release(MOUSE_MIDDLE);
        usbStick[0]->releaseButton(1);
    }
}

void runCalibration() {
  if (calibrationStep == 1) { //calibrate x axis
    if (psx.buttonPressed(PSB_START)) //A button press
      offsetX--;
    else if (psx.buttonPressed(PSB_CROSS)) //B button press
      offsetX++;
  } else if (calibrationStep == 2) { //calibrate y axis
    if (psx.buttonPressed(PSB_START)) //A button press
      offsetY--;
    else if (psx.buttonPressed(PSB_CROSS)) //B button press
      offsetY++;
  } else if (calibrationStep == 3) { //exit calibration mode
      calibrationStep = 0;
  }

  if (psx.buttonJustPressed(PSB_CIRCLE)) //trigger press
    calibrationStep++;

  word x, y;
  if (psx.getGunconCoordinates(x, y) == GUNCON_OK)
    moveToCoords(convertRange(minX, maxX, x) + offsetX, convertRange(minY, maxY, y) + offsetY);

  if (enableMouseMove)
      AbsMouse->report();
  else if (enableJoystick)
      usbStick[0]->sendState();
}

void loopNeGcon() {
  byte lx, ly;
  psx.getLeftAnalog (lx, ly);
  
  usbStick[0]->setThrottle(psx.getAnalogButton(PSAB_CROSS)); //I
  usbStick[0]->setZAxis(psx.getAnalogButton(PSAB_L1));
  usbStick[0]->setBrake(psx.getAnalogButton(PSAB_SQUARE)); //II
  usbStick[0]->setSteering(lx);
  
  usbStick[0]->setButton(0, psx.buttonPressed(PSB_CIRCLE)); //A
  usbStick[0]->setButton(1, psx.buttonPressed(PSB_TRIANGLE)); //B
  usbStick[0]->setButton(2, psx.buttonPressed(PSB_R1));
  usbStick[0]->setButton(3, psx.buttonPressed(PSB_START)); //Start
  
  //uint8_t hat = (psx.getButtonWord() >> 4) & B00001111;
  //usbStick[0]->setHatSwitch(0, hatTable[hat]);
  handleDpad();
  
  usbStick[0]->sendState();
}

void loopGuncon() {
  if (calibrationStep != 0) {
    runCalibration();
    return;
  }
  
  if (!enableReport) {
      if (!enableMouseMove && !enableJoystick) {
          if (psx.buttonJustPressed(PSB_CIRCLE)) {
              enableReport = true;
              enableMouseMove = true;
              //delay(300);
              return;
          }
          else if (psx.buttonJustPressed(PSB_START)) {
              enableReport = true;
              enableJoystick = true;
              //delay(300);
              return;
          }
      }
      else if (psx.buttonJustPressed(PSB_CIRCLE) || psx.buttonJustPressed(PSB_START)) {
          enableReport = true;
          //delay(300);
          return;
      }
      else if (psx.buttonJustPressed(PSB_CROSS)) { //ender calibration mode
        enableReport = true;
        calibrationStep = 1;
        delay(300);
        return;
      }
  }
  
  if (enableReport) {
      handleButtons();
      PsxControllerProtocol proto = psx.getProtocol();
      switch (proto) {
      case PSPROTO_GUNCON:
          readGuncon();
          break;
      case PSPROTO_DUALSHOCK:
      case PSPROTO_DUALSHOCK2:
          readDualShock();
          handleDpad();
          //controller buttons
          usbStick[0]->setButton (0, psx.buttonPressed (PSB_SQUARE));
          // shared with gun: 1 PSB_CROSS; 2 PSB_CIRCLE, 9 PSB_START;
          usbStick[0]->setButton (3, psx.buttonPressed (PSB_TRIANGLE));
          usbStick[0]->setButton (4, psx.buttonPressed (PSB_L1));
          usbStick[0]->setButton (5, psx.buttonPressed (PSB_R1));
          usbStick[0]->setButton (6, psx.buttonPressed (PSB_L2));
          usbStick[0]->setButton (7, psx.buttonPressed (PSB_R2));
          usbStick[0]->setButton (8, psx.buttonPressed (PSB_SELECT));
          usbStick[0]->setButton (10, psx.buttonPressed (PSB_L3));
          usbStick[0]->setButton (11, psx.buttonPressed (PSB_R3));
          break;
      default:
          return;
      }
  }
  
  //todo else
  if (enableReport) {
      if (enableMouseMove)
          AbsMouse->report();
      else if (enableJoystick)
          usbStick[0]->sendState();
  }
}

void loopDualShock() {
  byte analogX = ANALOG_IDLE_VALUE;
  byte analogY = ANALOG_IDLE_VALUE;
  word convertedX, convertedY;
  
  //PsxControllerProtocol proto = psx.getProtocol();
  switch (psx.getProtocol()) {
  case PSPROTO_DIGITAL:
  case PSPROTO_DUALSHOCK:
  case PSPROTO_DUALSHOCK2:
  case PSPROTO_FLIGHTSTICK:
      handleDpad();

      //controller buttons
      usbStick[0]->setButton (0, psx.buttonPressed (PSB_SQUARE));
      usbStick[0]->setButton (1, psx.buttonPressed (PSB_CROSS));
      usbStick[0]->setButton (2, psx.buttonPressed (PSB_CIRCLE));
      usbStick[0]->setButton (3, psx.buttonPressed (PSB_TRIANGLE));
      usbStick[0]->setButton (4, psx.buttonPressed (PSB_L1));
      usbStick[0]->setButton (5, psx.buttonPressed (PSB_R1));
      usbStick[0]->setButton (6, psx.buttonPressed (PSB_L2));
      usbStick[0]->setButton (7, psx.buttonPressed (PSB_R2));
      usbStick[0]->setButton (8, psx.buttonPressed (PSB_SELECT));
      usbStick[0]->setButton (9, psx.buttonPressed (PSB_START));
      usbStick[0]->setButton (10, psx.buttonPressed (PSB_L3));
      usbStick[0]->setButton (11, psx.buttonPressed (PSB_R3));

      //analog sticks
      if (psx.getLeftAnalog(analogX, analogY)) {
        convertedX = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
        convertedY = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
        usbStick[0]->setXAxis(convertedX);
        usbStick[0]->setYAxis(convertedY);
      } else {
        usbStick[0]->setXAxis(16384);
        usbStick[0]->setYAxis(16384);
      }

      if (psx.getRightAnalog(analogX, analogY)) {
        convertedX = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
        convertedY = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
        usbStick[0]->setRxAxis(convertedX);
        usbStick[0]->setRyAxis(convertedY);
      } else {
        usbStick[0]->setRxAxis(16384);
        usbStick[0]->setRyAxis(16384);
      }

      usbStick[0]->sendState();
      
      break;
  default:
      break;
  }
  
}



void handleJogconData()
{
  static uint16_t counter = 0, newcnt = 0, cleancnt = 0;
  //static uint16_t cleancnt = 0;
  static uint16_t newbtn = 0, oldbtn = 0;
  static int8_t oldspinner = 0;
  static uint8_t oldpaddle = 0;
  static int32_t pdlpos = sp_half;
  static uint16_t prevcnt = 0;
  
  uint8_t jogPosition = 0;
  uint8_t jogRevolutions = 0;
  JogconState jogStatus = psx.getJogconState(jogPosition, jogRevolutions);

  newcnt = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
  newbtn = psx.getButtonWord();//(data[3] << 8) | data[2];
  newbtn = (newbtn & ~3) | ((newbtn&1)<<2);

  if(jogStatus == JOGCON_STATE_OTHER) {//(data[0] == 0xF3)
    // Mode switch by pressing "mode" button while holding:
    // L2 - paddle mode (with FF stoppers)
    // R2 - steering mode (FF always enabled)
    // L2+R2 - spinner mode (no FF)
    if(psx.buttonPressed(PSB_L2) && psx.buttonPressed(PSB_R2))
      mode = 0;
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
    if(psx.buttonPressed(PSB_L1) || psx.buttonPressed(PSB_R1)) { //L 2, R 1
      sp_div = psx.buttonPressed(PSB_L1) ? 2 : 1;
      sp_max = SP_MAX/sp_div;
      sp_half = sp_max/2;
    }

    // some time for visual confirmation
    delay(200);

    // reset zero position
    init_jogcon();
    
    jogStatus = psx.getJogconState(jogPosition, jogRevolutions);

    prevcnt = 0;
    cleancnt = 0;
    counter = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
    pdlpos = sp_half;
  } else {
    
    if(jogStatus != JOGCON_STATE_NONE) {
      cleancnt += newcnt - counter;
      if(!mode)
      {
        ff = 0;
        pdlpos += (int16_t)(newcnt - counter);
        if(pdlpos<0) pdlpos = 0;
        if(pdlpos>sp_max) pdlpos = sp_max;
      }
    }

    if(mode)
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
      psx.setJogconMode(JOGCON_MODE_HOLD, force);
    else 
      psx.setJogconMode(JOGCON_MODE_STOP, 0);

    int16_t val = ((int16_t)(cleancnt - prevcnt))/sp_step;
    if(val>127) val = 127; else if(val<-127) val = -127;
    prevcnt += val*sp_step;
    int8_t spinner = val;
    
    uint8_t paddle = ((pdlpos*255)/sp_max);

    if (enableMouseMove) {
      if (oldpaddle != paddle || oldspinner != spinner) {
        oldpaddle = paddle;
        oldspinner = spinner;
        Mouse->setXAxis(spinner);
        Mouse->sendState();
      }
      if(oldbtn != newbtn) {
        oldbtn = newbtn;

        handleDpad();
        
        int16_t btn = (newbtn & 0xF) | ((newbtn>>4) & ~0xF);
        for (int8_t i = 2; i < 14; i++)
          usbStick[0]->setButton(i, bitRead(btn, i));
        //usbStick[0]->setHatSwitch(0, dpad2hat(newbtn>>4));
        usbStick[0]->sendState();
      }
    } else {
      if(oldbtn != newbtn || oldpaddle != paddle || oldspinner != spinner) {
        oldbtn = newbtn;
        oldpaddle = paddle;
        oldspinner = spinner;

        int16_t btn = (newbtn & 0xF) | ((newbtn>>4) & ~0xF);
        for (int8_t i = 2; i < 14; i++)
          usbStick[0]->setButton(i, bitRead(btn, i));

        handleDpad();

        usbStick[0]->setWheel(paddle);
        usbStick[0]->setDial(spinner);
        usbStick[0]->sendState();
      }
    }
  }
  counter = newcnt;
}


void psxSetup() {
  if (psx.begin ()) {
    delay(200);
    haveController = true;
    PsxControllerProtocol proto = psx.getProtocol();

    if (proto == PSPROTO_GUNCON) {
      isGuncon = true;
    } else if (proto == PSPROTO_NEGCON) {
      isNeGcon = true;
    } else {
      if(psx.buttonPressed(PSB_SELECT)) { //dualshock used in guncon mode to help map axis on emulators.
        isGuncon = true;
      } else if(psx.buttonPressed(PSB_L1)) {
        isJogcon = true;
      } else if(psx.buttonPressed(PSB_L2)) {
        isJogcon = true;
        enableMouseMove = true;
      }
    }
  }

  if (isNeGcon) {
    usbStick[0] = new Joystick_ (
      "RetroZordPsNeGcon",
      JOYSTICK_DEFAULT_REPORT_ID,
      JOYSTICK_TYPE_JOYSTICK,
      4,      // buttonCount
      1,      // hatSwitchCount (0-2)
      false,   // includeXAxis
      false,   // includeYAxis
      true,    // includeZAxis
      false,    // includeRxAxis
      false,    // includeRyAxis
      false,    // includeRzAxis
      false,    // includeRudder
      true,    // includeThrottle
      false,    // includeAccelerator
      true,    // includeBrake
      true,   // includeSteering
      false,   // includeDial
      false   // includeWheel
    );
    usbStick[0]->begin(false);
    usbStick[0]->setThrottleRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
    usbStick[0]->setZAxisRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
    usbStick[0]->setBrakeRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
    usbStick[0]->setSteeringRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE);
    
  } else if (isJogcon) {
    mode = 0;
    force = 7;
    sp_step = 4;
    sp_div = 1;
    sp_max = SP_MAX/sp_div;
    sp_half = sp_max/2;

    //enableMouseMove = haveController && psx.buttonPressed(PSB_SELECT);

    if(enableMouseMove) {
      Mouse = new Mouse_("RetroZordPsJogCon");
      Mouse->begin (false);
    }
  
    usbStick[0] = new Joystick_ (
      enableMouseMove ? "RetroZordPsJogCon" : "MiSTer-A1 JogCon",
      JOYSTICK_DEFAULT_REPORT_ID,
      JOYSTICK_TYPE_JOYSTICK,
      16,      // buttonCount
      1,      // hatSwitchCount (0-2)
      true,   // includeXAxis
      true,   // includeYAxis
      false,    // includeZAxis
      false,    // includeRxAxis
      false,    // includeRyAxis
      false,    // includeRzAxis
      false,    // includeRudder
      false,    // includeThrottle
      false,    // includeAccelerator
      false,    // includeBrake
      false,   // includeSteering
      true,   // includeDial
      true   // includeWheel
    );
    usbStick[0]->begin (false);
    usbStick[0]->setXAxisRange(-128, 127);
    usbStick[0]->setYAxisRange(-128, 127);
    usbStick[0]->setXAxis(0);
    usbStick[0]->setYAxis(0);
    
  
    if (enableMouseMove)
      delay(200);
    
  } else {
    if (isGuncon) {
      AbsMouse = new AbsMouse_();
    }
      
    usbStick[0] = new Joystick_ (
      isGuncon ? "RetroZordPsGun" : "RetroZordPsDS1",
      JOYSTICK_DEFAULT_REPORT_ID,
      JOYSTICK_TYPE_JOYSTICK,
      12,      // buttonCount
      1,      // hatSwitchCount (0-2)
      true,   // includeXAxis
      true,   // includeYAxis
      false,    // includeZAxis
      true,    // includeRxAxis
      true,    // includeRyAxis
      false,    // includeRzAxis
      false,    // includeRudder
      false,    // includeThrottle
      false,    // includeAccelerator
      false,    // includeBrake
      false,   // includeSteering
      false,   // includeDial
      false   // includeWheel
    );
    usbStick[0]->begin(false);
    usbStick[0]->setXAxisRange(0, maxMouseValue);
    usbStick[0]->setYAxisRange(0, maxMouseValue);
    usbStick[0]->setRxAxisRange(0, maxMouseValue);
    usbStick[0]->setRyAxisRange(0, maxMouseValue);
  }

    dstart(115200);
    debugln(F("Ready!"));
}

void psxLoop() {
    static unsigned long last = 0;

    if(isJogcon) {
      if (!haveController) {
        init_jogcon();
      } else {
        if(!psx.read()){
          haveController = false;
        } else {
          handleJogconData();
        }
      }
      return;
    }

    if (millis() - last >= POLLING_INTERVAL) {
        last = millis();

        if (!haveController) {
            if (psx.begin()) {
                debugln(F("Controller found!"));

                haveController = true;
            } else {
              blinkLed();
            }
        }
        else {
            noInterrupts();
            boolean isReadSuccess = psx.read();
            interrupts();

            if (!isReadSuccess) {
                //debug (F("Controller lost."));
                //debug (F(" last values: x = "));
                //debug (lastX);
                //debug (F(", y = "));
                //debugln (lastY);

                haveController = false;
            }
            else {
              // Read was successful
              if (isNeGcon)
                loopNeGcon();
              else if (isGuncon)
                loopGuncon();
              else
                loopDualShock();
            }
        }
    }
}

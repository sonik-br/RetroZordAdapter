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
 * Press Gun Trigger / Circle to enable mouse emulation. Off-screen: bottom-left.
 * Press Gun A (Left side) / Start to enable joystick emulation. Off-screen: center.
 * Press Gun B (Right side) / Start to enable joystick emulation. Off-screen: bottom-left.
 *
 * To disable, point it off screen and press A + B + Trigger
 *
 * Buttons are mapped as follows:
 * A (Left side) / Start -> Mouse Right / Joystick btn 1
 * B (Right side) / Cross -> Mouse Middle / Joystick btn 2
 * Trigger / Circle -> Mouse Left / Joystick btn 0
*/

#include "src/PsxNewLib/PsxControllerHwSpi.h"

#include "src/ArduinoJoystickLibrary/Joy1.h"
#include "src/ArduinoJoystickLibrary/Guncon1.h"
#include "src/ArduinoJoystickLibrary/MouseRelative1.h"
#include "src/ArduinoJoystickLibrary/Jogcon1.h"

//Guncon config
//0=Mouse, 1=Joy, 2=Joy OffScreenEdge (MiSTer)
//#define GUNCON_FORCE_MODE 2

//NeGcon config
//0=Default, 1=MiSTer Wheel format with paddle
//#define NEGCON_FORCE_MODE 1
//If you dont want to force a mode but instead change the default:
//Don't enable the force mode and edit the isNeGconMiSTer variable below as you desire.

const byte PIN_PS2_ATT = 11;

// Send debug messages to serial port
//#define ENABLE_SERIAL_DEBUG

PsxControllerHwSpi<PIN_PS2_ATT> psx;

const byte ANALOG_DEAD_ZONE = 25U;

const uint16_t maxMouseValue = 1023;//32767;

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
word lastX = 0;//-1;
word lastY = 0;//-1;

boolean isNeGcon = false;
boolean isJogcon = false;
boolean isGuncon = false;

boolean isNeGconMiSTer = false;

boolean enableReport = false;
boolean enableMouseMove = false;
boolean enableJoystick = false;
boolean joyOffScreenEdge = false; //what to do when pointing out of screen? false: center. true: bottom left (MAME standard)

Guncon1_* AbsMouse;
MouseRel1_* RelMouse;

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
  if (enableMouseMove) {
    AbsMouse->setXAxis(x);
    AbsMouse->setYAxis(y);
  }
  
  if (enableJoystick) {
    ((Guncon1_*)usbStick[0])->setXAxis(x);
    ((Guncon1_*)usbStick[0])->setYAxis(y);
  }
}

void releaseAllButtons() { //guncon
  uint8_t i;
  if (enableMouseMove) {
    AbsMouse->setButtons(0);
    AbsMouse->sendState();
  }

  if (enableJoystick) {
    ((Guncon1_*)usbStick[0])->setButtons(0);
    usbStick[0]->sendState();
  }
}

void readGuncon() {
  word x, y, convertedX, convertedY;
  const GunconStatus gcStatus = psx.getGunconCoordinates(x, y); //use coords from guncon

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
        //convertedX = convertRange(minX, maxX, x);
        //convertedY = convertRange(minY, maxY, y);
        //moveToCoords(convertedX + offsetX, convertedY + offsetY);
        convertedX = convertRange(minX + offsetX, maxX + offsetX, x);
        convertedY = convertRange(minY + offsetY, maxY + offsetY, y);
        moveToCoords(convertedX, convertedY);
      }
    }
  }
  else if (gcStatus == GUNCON_NO_LIGHT) {

    //up to 10 no_light reads will report the last good values
    if (lastX != 0 && lastY != 0) {
      //convertedX = convertRange(minX, maxX, lastX);
      //convertedY = convertRange(minY, maxY, lastY);
      convertedX = convertRange(minX + offsetX, maxX + offsetX, lastX);
      convertedY = convertRange(minY + offsetY, maxY + offsetY, lastY);

      moveToCoords(convertedX, convertedY);
      //moveToCoords(convertedX + offsetX, convertedY + offsetY);

      noLightCount++;

      if (noLightCount > maxNoLightCount) {
        noLightCount = 0;
        lastX = 0;
        lastY = 0;

        //set it offscreen (bottom left). need to test
        //also release all buttons
        if (enableMouseMove) {
          AbsMouse->setXAxis(0);
          AbsMouse->setYAxis(maxMouseValue);
        }

        //put joystick to the center position
        if (enableJoystick) {
          if (joyOffScreenEdge) {
            ((Guncon1_*)usbStick[0])->setXAxis(0);
            ((Guncon1_*)usbStick[0])->setYAxis(maxMouseValue);
          } else {
            ((Guncon1_*)usbStick[0])->setXAxis(16383);
            ((Guncon1_*)usbStick[0])->setYAxis(16383);
          }
        }

        //releasing buttons breaks compatibility with time crisis style of reload
        //releaseAllButtons();
      }
    }
    else if (psx.buttonPressed(PSB_CIRCLE) && psx.buttonPressed(PSB_START) && psx.buttonPressed(PSB_CROSS)) {//only when using guncon. dualshock wont work
      enableReport = false;
      releaseAllButtons();
      delay(1000);
    }

  }
}

//used only when in dualshock as guncon mode
void analogDeadZone(byte& value) {
  const int8_t delta = value - ANALOG_IDLE_VALUE;
  if (abs(delta) < ANALOG_DEAD_ZONE)
    value = ANALOG_IDLE_VALUE;
}

void readDualShock() {//dualshock as guncon.
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
  /*
  if (enableJoystick) {
    analogX = ANALOG_IDLE_VALUE;
    analogY = ANALOG_IDLE_VALUE;
    if (psx.getRightAnalog(analogX, analogY)) { //use coords from analog controller
      //analogDeadZone(analogX);
      //analogDeadZone(analogY);
    }
    //x = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
    //y = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
    //usbStick[0]->setRxAxis(x);
    //usbStick[0]->setRyAxis(y);
    usbStick[0]->setRxAxis(analogX);
    usbStick[0]->setRyAxis(analogY);
  }
  */

  /*if (psx.buttonPressed(PSB_SELECT)) {
      enableReport = false;
      releaseAllButtons();
      delay(1000);
  }*/
}

void handleDpad(const bool isjog = false) {
  int16_t dpad = B0;
  bitWrite(dpad, 0, !psx.buttonPressed (PSB_PAD_UP));
  bitWrite(dpad, 1, !psx.buttonPressed (PSB_PAD_DOWN));
  bitWrite(dpad, 2, !psx.buttonPressed (PSB_PAD_LEFT));
  bitWrite(dpad, 3, !psx.buttonPressed (PSB_PAD_RIGHT));
  //usbStick[0]->setHatSwitch(0, hatTable[dpad]);
  if (isjog)
    ((Jogcon1_*)usbStick[0])->setHatSwitch(hatTable[dpad]);
  else
    ((Joy1_*)usbStick[0])->setHatSwitch(hatTable[dpad]);
}


void handleButtons() { //guncon
  /*if (psx.buttonJustPressed(PSB_CIRCLE)) { //trigger press
    AbsMouse->pressButton(0);//MOUSE_LEFT
    usbStick[0]->pressButton(0);
  }
  else if (psx.buttonJustReleased(PSB_CIRCLE)) { //trigger release
    AbsMouse->releaseButton(0);//MOUSE_LEFT
    usbStick[0]->releaseButton(0);
  }

  if (psx.buttonJustPressed(PSB_START)) { //A button press
    AbsMouse->pressButton(1);//MOUSE_RIGHT
    usbStick[0]->pressButton(1);
  }
  else if (psx.buttonJustReleased(PSB_START)) { //A button release
    AbsMouse->releaseButton(1);//MOUSE_RIGHT
    usbStick[0]->releaseButton(1);
  }

  if (psx.buttonJustPressed(PSB_CROSS)) { //B button press
    AbsMouse->pressButton(2);//MOUSE_MIDDLE
    usbStick[0]->pressButton(2);
  }
  else if (psx.buttonJustReleased(PSB_CROSS)) { //B button release
    AbsMouse->releaseButton(2);//MOUSE_MIDDLE
    usbStick[0]->releaseButton(2);
  }*/

  uint8_t buttonData = 0;
  bitWrite(buttonData, 0, psx.buttonPressed(PSB_CIRCLE)); //trigger
  bitWrite(buttonData, 1, psx.buttonPressed(PSB_START)); //A
  bitWrite(buttonData, 2, psx.buttonPressed(PSB_CROSS)); //B

  AbsMouse->setButtons(buttonData);
  ((Guncon1_*)usbStick[0])->setButtons(buttonData);
}

void runCalibration() {
  if (calibrationStep == 1) { //calibrate x axis
    if (psx.buttonJustPressed(PSB_START)) //A button press
      offsetX--;
    else if (psx.buttonJustPressed(PSB_CROSS)) //B button press
      offsetX++;
  } else if (calibrationStep == 2) { //calibrate y axis
    if (psx.buttonJustPressed(PSB_START)) //A button press
      offsetY--;
    else if (psx.buttonJustPressed(PSB_CROSS)) //B button press
      offsetY++;
  } else if (calibrationStep == 3) { //exit calibration mode
    calibrationStep = 0;
    return;
  }

  if (psx.buttonJustPressed(PSB_CIRCLE)) //trigger press
    calibrationStep++;

  word x, y;
  if (psx.getGunconCoordinates(x, y) == GUNCON_OK) {
    //moveToCoords(convertRange(minX, maxX, x + offsetX), convertRange(minY, maxY, y + offsetY));
    moveToCoords(convertRange(minX + offsetX, maxX + offsetX, x), convertRange(minY + offsetY, maxY + offsetY, y));
  }

  if (enableMouseMove)
    AbsMouse->sendState();
  else if (enableJoystick)
    usbStick[0]->sendState();
}

void loopNeGcon() {
  byte lx, ly;
  psx.getLeftAnalog (lx, ly);

  ((Joy1_*)usbStick[0])->setAnalog0(lx); //steering
  
  //uses inversed values
  ((Joy1_*)usbStick[0])->setAnalog1((uint8_t) ~ psx.getAnalogButton(PSAB_L1)); //z
  ((Joy1_*)usbStick[0])->setAnalog2((uint8_t) ~ psx.getAnalogButton(PSAB_CROSS)); //I throttle
  ((Joy1_*)usbStick[0])->setAnalog3((uint8_t) ~ psx.getAnalogButton(PSAB_SQUARE)); //II brake

  if(isNeGconMiSTer)
    ((Joy1_*)usbStick[0])->setAnalog4(lx); //paddle 

  //combine the two axes and use only half precision for each
  //const int btnI_II = psx.getAnalogButton(PSAB_SQUARE) - psx.getAnalogButton(PSAB_CROSS);
  //const uint8_t btnI_II = ((psx.getAnalogButton(PSAB_SQUARE) - psx.getAnalogButton(PSAB_CROSS)) >> 1) + ANALOG_IDLE_VALUE;

  uint8_t buttonData = 0;
  bitWrite(buttonData, 0, psx.buttonPressed(PSB_CIRCLE)); //A
  bitWrite(buttonData, 1, psx.buttonPressed(PSB_TRIANGLE)); //B
  bitWrite(buttonData, 2, psx.buttonPressed(PSB_R1));
  bitWrite(buttonData, 3, psx.buttonPressed(PSB_START)); //Start

  ((Joy1_*)usbStick[0])->setButtons(buttonData);
  
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
    if (!enableMouseMove && !enableJoystick) { //will only configure here on the first time.
      #if defined(GUNCON_FORCE_MODE) && GUNCON_FORCE_MODE >= 0 && GUNCON_FORCE_MODE < 3
        enableReport = true;
        #if GUNCON_FORCE_MODE == 0
          enableMouseMove = true;
        #elif GUNCON_FORCE_MODE == 1
          enableJoystick = true;
        #elif GUNCON_FORCE_MODE == 2
          enableJoystick = true;
          joyOffScreenEdge = true;
        #endif
      #else //GUNCON_FORCE_MODE
        if (psx.buttonJustPressed(PSB_CIRCLE)) { //trigger
          enableReport = true;
          enableMouseMove = true;
          //delay(300);
          return;
        } else if (psx.buttonJustPressed(PSB_START)) { //A - left
          enableReport = true;
          enableJoystick = true;
          //delay(300);
          return;
        } else if (psx.buttonJustPressed(PSB_CROSS)) { //B - right
          //joystick mode with offscreen report at the edges.
          //good for use with mister. bad for mame.
          enableReport = true;
          enableJoystick = true;
          joyOffScreenEdge = true;
          //delay(300);
          return;
        }
      #endif //GUNCON_FORCE_MODE
    } else if (psx.buttonJustPressed(PSB_CIRCLE) || psx.buttonJustPressed(PSB_START)) { //re-enable the configured report mode
      enableReport = true;
      //delay(300);
      return;
    } else if (psx.buttonJustPressed(PSB_CROSS)) { //enter calibration mode
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
      /*
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
      usbStick[0]->setButton (11, psx.buttonPressed (PSB_R3));*/
      break;
    default:
      return;
    }
  }
  
  //todo else
  if (enableReport) {
    if (enableMouseMove)
      AbsMouse->sendState();
    else if (enableJoystick)
      usbStick[0]->sendState();
  }
}

void loopDualShock() {
  byte analogX = ANALOG_IDLE_VALUE;
  byte analogY = ANALOG_IDLE_VALUE;
  //word convertedX, convertedY;
  
  //PsxControllerProtocol proto = psx.getProtocol();
  switch (psx.getProtocol()) {
  case PSPROTO_DIGITAL:
  case PSPROTO_DUALSHOCK:
  case PSPROTO_DUALSHOCK2:
  case PSPROTO_FLIGHTSTICK:
    handleDpad();

    uint16_t buttonData = 0;

    //controller buttons
    bitWrite(buttonData, 0, psx.buttonPressed (PSB_SQUARE));
    bitWrite(buttonData, 1, psx.buttonPressed (PSB_CROSS));
    bitWrite(buttonData, 2, psx.buttonPressed (PSB_CIRCLE));
    bitWrite(buttonData, 3, psx.buttonPressed (PSB_TRIANGLE));
    bitWrite(buttonData, 4, psx.buttonPressed (PSB_L1));
    bitWrite(buttonData, 5, psx.buttonPressed (PSB_R1));
    bitWrite(buttonData, 6, psx.buttonPressed (PSB_L2));
    bitWrite(buttonData, 7, psx.buttonPressed (PSB_R2));
    bitWrite(buttonData, 8, psx.buttonPressed (PSB_SELECT));
    bitWrite(buttonData, 9, psx.buttonPressed (PSB_START));
    bitWrite(buttonData, 10, psx.buttonPressed (PSB_L3));
    bitWrite(buttonData, 11, psx.buttonPressed (PSB_R3));

    //analog sticks
    if (psx.getLeftAnalog(analogX, analogY)) {
      //convertedX = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
      //convertedY = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
      //usbStick[0]->setXAxis(convertedX);
      //usbStick[0]->setYAxis(convertedY);
      ((Joy1_*)usbStick[0])->setAnalog0(analogX); //x
      ((Joy1_*)usbStick[0])->setAnalog1(analogY); //y
    } else {
      //usbStick[0]->setXAxis(16384);
      //usbStick[0]->setYAxis(16384);
      ((Joy1_*)usbStick[0])->setAnalog0(ANALOG_IDLE_VALUE);
      ((Joy1_*)usbStick[0])->setAnalog1(ANALOG_IDLE_VALUE);
    }

    if (psx.getRightAnalog(analogX, analogY)) {
      //convertedX = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogX);
      //convertedY = convertRange(ANALOG_MIN_VALUE, ANALOG_MAX_VALUE, analogY);
      //usbStick[0]->setRxAxis(convertedX);
      //usbStick[0]->setRyAxis(convertedY);
      ((Joy1_*)usbStick[0])->setAnalog2(analogX); //rx
      ((Joy1_*)usbStick[0])->setAnalog3(analogY); //ry
    } else {
      //usbStick[0]->setRxAxis(16384);
      //usbStick[0]->setRyAxis(16384);
      ((Joy1_*)usbStick[0])->setAnalog2(ANALOG_IDLE_VALUE);
      ((Joy1_*)usbStick[0])->setAnalog3(ANALOG_IDLE_VALUE);
    }

    ((Joy1_*)usbStick[0])->setButtons(buttonData);

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
  static JogconDirection lastDirection = JOGCON_DIR_NONE;
  
  uint8_t jogPosition = 0;
  uint8_t jogRevolutions = 0;
  JogconDirection jogDirection = JOGCON_DIR_NONE;
  JogconCommand cmdResult = JOGCON_CMD_NONE;
  //JogconState jogStatus = psx.getJogconState(jogPosition, jogRevolutions);
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
    
    //jogStatus = psx.getJogconState(jogPosition, jogRevolutions);
    gotJogconData = psx.getJogconData(jogPosition, jogRevolutions, jogDirection, cmdResult);

    prevcnt = 0;
    cleancnt = 0;
    counter = (jogRevolutions << 8) | jogPosition;//(data[5] << 8) | data[4];
    pdlpos = sp_half;
  } else {
    
    //if(jogStatus != JOGCON_STATE_NONE) {
    if(jogDirection != JOGCON_DIR_NONE) {
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
      psx.setJogconMotorMode(JOGCON_DIR_START, nextCmd, force);
    else 
      psx.setJogconMotorMode(JOGCON_DIR_NONE, nextCmd, 0);

    int16_t val = ((int16_t)(cleancnt - prevcnt))/sp_step;
    if(val>127) val = 127; else if(val<-127) val = -127;
    prevcnt += val*sp_step;
    const int8_t spinner = val;
    
    const uint8_t paddle = ((pdlpos*255)/sp_max);

    if (enableMouseMove) {
      if (oldpaddle != paddle || oldspinner != spinner) {
        oldpaddle = paddle;
        oldspinner = spinner;
        RelMouse->setXAxis(spinner);
        RelMouse->sendState();
      }
      if(oldbtn != newbtn) {
        oldbtn = newbtn;

        handleDpad(true);
        
        ((Jogcon1_*)usbStick[0])->setButtons(newbtn & 0xFF0F); //buttons except dpad
        usbStick[0]->sendState();
      }
    } else {
      if(oldbtn != newbtn || oldpaddle != paddle || oldspinner != spinner) {
        oldbtn = newbtn;
        oldpaddle = paddle;
        oldspinner = spinner;

        ((Jogcon1_*)usbStick[0])->setButtons(newbtn & 0xFF0F); //buttons except dpad

        handleDpad(true);

        ((Jogcon1_*)usbStick[0])->setPaddle(paddle);
        ((Jogcon1_*)usbStick[0])->setSpinner(spinner);
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
    const PsxControllerProtocol proto = psx.getProtocol();

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

      //Try to detect by jogcon it's id.
      //Too slow to run during boot. One option is to detect, put it in analog mode and force a reset.
//      if(proto == PSPROTO_DIGITAL) {
//        if (psx.enterConfigMode ()) {
//          if (psx.getControllerType () == PSCTRL_JOGCON) {
//            isJogcon = true;
//            if (psx.buttonPressed(PSB_L2))
//              enableMouseMove = true;
//          }
//          psx.exitConfigMode ();
//        }
//      }

      if (psx.buttonPressed(PSB_SELECT)) { //dualshock used in guncon mode to help map axis on emulators.
        isGuncon = true;
      } else if (psx.buttonPressed(PSB_L1)) {
        isJogcon = true;
      } else if (psx.buttonPressed(PSB_L2)) {
        isJogcon = true;
        enableMouseMove = true;
      }
    }
  }

  if (isNeGcon) {
    usbStick[0] = new Joy1_(isNeGconMiSTer ? "RZordPsWheel" : "RZordPsNeGcon", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 1,
      true,//includeXAxis,
      false,//includeYAxis,
      true,//includeZAxis,
      false,//includeRzAxis,
      true,//includeThrottle,
      true,//includeBrake,
      isNeGconMiSTer);//includeSteering

    //initial values
    usbStick[0]->resetState();
    ((Joy1_*)usbStick[0])->setAnalog0(ANALOG_IDLE_VALUE); //x
    ((Joy1_*)usbStick[0])->setAnalog1(ANALOG_MAX_VALUE); //z
    ((Joy1_*)usbStick[0])->setAnalog2(ANALOG_MAX_VALUE); //I throttle
    ((Joy1_*)usbStick[0])->setAnalog3(ANALOG_MAX_VALUE); //II brake
    ((Joy1_*)usbStick[0])->setAnalog4(ANALOG_IDLE_VALUE); //paddle
    
    usbStick[0]->sendState();
  
  } else if (isJogcon) {
    mode = 0;
    force = 7;
    sp_step = 4;
    sp_div = 1;
    sp_max = SP_MAX/sp_div;
    sp_half = sp_max/2;

    //enableMouseMove = haveController && psx.buttonPressed(PSB_SELECT);

    if(enableMouseMove) {
      RelMouse = new MouseRel1_("RZordPsJogCon", JOYSTICK_DEFAULT_REPORT_ID, 2);
      //RelMouse->begin (false);
      RelMouse->resetState();//todo send?
      RelMouse->sendState();

      usbStick[0] = new Jogcon1_("RZordPsJogCon", JOYSTICK_DEFAULT_REPORT_ID + 1, JOYSTICK_TYPE_GAMEPAD, 2);
    } else {
      usbStick[0] = new Jogcon1_("MiSTer-A1 JogCon", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK, 1);
    }
    usbStick[0]->resetState();
    usbStick[0]->sendState();
    
    if (enableMouseMove)
      delay(200);
    
  } else {
    if (isGuncon) {
      AbsMouse = new Guncon1_("RZordPsGun", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_MOUSE, 2);
      AbsMouse->resetState();
      AbsMouse->sendState();

      usbStick[0] = new Guncon1_("RZordPsGun", JOYSTICK_DEFAULT_REPORT_ID + 1, JOYSTICK_TYPE_GAMEPAD, 2);
      
    } else {
      usbStick[0] = new Joy1_("RZordPsDS1", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD, 1,
        true,//includeXAxis,
        true,//includeYAxis,
        true,//includeZAxis,
        true,//includeRzAxis,
        false,//includeThrottle,
        false,//includeBrake,
        false);//includeSteering
    }
    
    usbStick[0]->resetState();
    usbStick[0]->sendState();
  }

  sleepTime = 100;

  dstart(115200);
  debugln(F("Ready!"));
}

inline bool __attribute__((always_inline))
psxLoop() {

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
    return haveController;
  }

  if (!haveController) {
      if (psx.begin()) {
        debugln(F("Controller found!"));

        haveController = true;
      } /*else {
        blinkLed();
      }*/
  } else {
    if (!psx.read()) {
      //debug (F("Controller lost."));
      //debug (F(" last values: x = "));
      //debug (lastX);
      //debug (F(", y = "));
      //debugln (lastY);

      haveController = false;
    } else {
      // Read was successful
      if (isNeGcon)
        loopNeGcon();
      else if (isGuncon)
        loopGuncon();
      else
        loopDualShock();
    }
  }

  return haveController;
}

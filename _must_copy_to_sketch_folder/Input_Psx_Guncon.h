/*******************************************************************************
 * PlayStation (GUNCON) input module for RetroZord / Reflex-Adapt.
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

const uint16_t maxMouseValue = 1023;//32767;

//min and max possible values
//from document at http://problemkaputt.de/psx-spx.htm#controllerslightgunsnamcoguncon
//x is 77 to 461
//y is 25 to 248 (ntsc). y is 32 to 295 (pal)
//from personal testing on a pvm
//x is 72 to 450. with underscan x 71 to 453
//y is 22 to 248. with underscan y is 13 to 254 (ntsc)

const uint8_t ANALOG_DEAD_ZONE = 25;

const uint8_t minPossibleX = 77;
const uint16_t maxPossibleX = 461;
const uint8_t minPossibleY = 25;
const uint16_t maxPossibleY = 295;

#if GUNCON_FORCE_MODE == 3
  const uint8_t maxNoLightCount = 254;//80
#else
  const uint8_t maxNoLightCount = 10;
#endif

uint8_t calibrationStep = 0;

bool enableReport = false;
bool enableJoystick = false;
bool joyOffScreenEdge = false; //what to do when pointing out of screen? false: center. true: bottom left (MAME standard)

// Minimum and maximum detected values. Varies from tv to tv.
// Values will be detected when pointing at the screen.
#if GUNCON_FORCE_MODE != 3
  uint16_t minX = 1000;
  uint16_t maxX = 0;
  uint16_t minY = 1000;
  uint16_t maxY = 0;
#endif

int8_t offsetX = 0;
int8_t offsetY = 0;

uint8_t noLightCount = 0;

// Last successful read coordinates
uint16_t lastX = 0;
uint16_t lastY = 0;

#ifdef ENABLE_PSX_GUNCON_MOUSE
  Guncon1_* AbsMouse;
#endif

uint16_t convertRange(const uint16_t gcMin, const uint16_t gcMax, const uint16_t value) { return map(value, gcMin, gcMax, 0, maxMouseValue); }

/*word convertRange(const double gcMin, const double gcMax, const double value) {
  const double scale = maxMouseValue / (gcMax - gcMin);
  return (word)((value - gcMin) * scale);
}*/


/*word convertRange(word gcMin, word gcMax, word value) {
  word scale = (word)(maxMouseValue / (gcMax - gcMin));
  return (word)((value - gcMin) * scale);
}*/


void moveToCoords(uint16_t x, uint16_t y) {
#ifdef ENABLE_PSX_GUNCON_MOUSE
  if (enableMouseMove) {
      AbsMouse->setXAxis(x);
      AbsMouse->setYAxis(y);
  }
#endif
  
  if (enableJoystick) {
//    ((Guncon1_*)usbStick[0])->setXAxis(x);
//    ((Guncon1_*)usbStick[0])->setYAxis(y);
    state[0].lx = x;
    state[0].ly = y;
  }
}

void releaseAllButtons() { //guncon
  //uint8_t i;
#ifdef ENABLE_PSX_GUNCON_MOUSE
  if (enableMouseMove) {
      //for (i = 0; i < 3; i++)//MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE
      //  AbsMouse->setButton(i, false);
      AbsMouse->setButtons(0);
      AbsMouse->sendState();
  }
#endif

  if (enableJoystick) {
    //for (i = 0; i < 3; i++)
    //  ((Guncon1_*)usbStick[0])->setButton(i, false);
    //((Guncon1_*)usbStick[0])->setButtons(0);
    state[0].buttons = 0;
  }
}

void readGuncon() {
  uint16_t x, y;
  #if GUNCON_FORCE_MODE != 3
    uint16_t convertedX, convertedY;
  #endif
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
#if GUNCON_FORCE_MODE == 3
    {
#else
    if (x >= minPossibleX && x <= maxPossibleX && y >= minPossibleY && y <= maxPossibleY) {
#endif
      //x += offsetX;
      //y += offsetY;
      lastX = x;
      lastY = y;

      //got new min or max values?
#if GUNCON_FORCE_MODE != 3
      if (x < minX)
        minX = x;
      else if (x > maxX)
        maxX = x;

      if (y < minY)
        minY = y;
      else if (y > maxY)
        maxY = y;
#endif


#if GUNCON_FORCE_MODE == 3
      if (enableJoystick) {
        moveToCoords(x + offsetX, y + offsetY);
      }
#else
      if (enableMouseMove || enableJoystick) {
        //convertedX = convertRange(minX, maxX, x);
        //convertedY = convertRange(minY, maxY, y);
        //moveToCoords(convertedX + offsetX, convertedY + offsetY);
        
        convertedX = convertRange(minX + offsetX, maxX + offsetX, x);
        convertedY = convertRange(minY + offsetY, maxY + offsetY, y);
        moveToCoords(convertedX, convertedY);
      }
#endif

    }
  }
  else if (gcStatus == GUNCON_NO_LIGHT) {

    //up to 10 no_light reads will report the last good values
    if (lastX != 0 && lastY != 0) {
      //convertedX = convertRange(minX, maxX, lastX);
      //convertedY = convertRange(minY, maxY, lastY);

#if GUNCON_FORCE_MODE == 3
      moveToCoords(lastX + offsetX, lastY + offsetY);
#else
      convertedX = convertRange(minX + offsetX, maxX + offsetX, lastX);
      convertedY = convertRange(minY + offsetY, maxY + offsetY, lastY);
      moveToCoords(convertedX, convertedY);
#endif
      //moveToCoords(convertedX + offsetX, convertedY + offsetY);

      noLightCount++;

      if (noLightCount > maxNoLightCount) {
        noLightCount = 0;
        lastX = 0;
        lastY = 0;

        //set it offscreen (bottom left). need to test
        //also release all buttons
#ifdef ENABLE_PSX_GUNCON_MOUSE
        if (enableMouseMove) {
            AbsMouse->setXAxis(0);
            AbsMouse->setYAxis(maxMouseValue);
        }
#endif

        //put joystick to the center position
        if (enableJoystick) {
#if GUNCON_FORCE_MODE == 3
          //((Guncon1_*)usbStick[0])->setXAxis(0);
          //((Guncon1_*)usbStick[0])->setYAxis(0);
          state[0].lx = 0;
          state[0].ly = 0;
#else
          if (joyOffScreenEdge) {
            ((Guncon1_*)usbStick[0])->setXAxis(0);
            ((Guncon1_*)usbStick[0])->setYAxis(maxMouseValue);
          } else {
            ((Guncon1_*)usbStick[0])->setXAxis(maxMouseValue/2);//16383
            ((Guncon1_*)usbStick[0])->setYAxis(maxMouseValue/2);
          }
#endif
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
  uint16_t x, y;
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

  #ifdef ENABLE_PSX_GUNCON_MOUSE
    AbsMouse->setButtons(buttonData);
  #endif
  //((Guncon1_*)usbStick[0])->setButtons(buttonData);
  state[0].buttons = buttonData;
}

void runCalibration() {
  bool changed = false;
  if (calibrationStep == 1) { //calibrate x axis
    if (psx.buttonJustPressed(PSB_START)){ //A button press
      offsetX--;
      changed = true;
    } else if (psx.buttonJustPressed(PSB_CROSS)){ //B button press
      offsetX++;
      changed = true;
    }

  } else if (calibrationStep == 2) { //calibrate y axis
    if (psx.buttonJustPressed(PSB_START)) { //A button press
      offsetY--;
      changed = true;
    } else if (psx.buttonJustPressed(PSB_CROSS)) { //B button press
      offsetY++;
      changed = true;
    }
  } else if (calibrationStep == 3) { //exit calibration mode
    calibrationStep = 0;
    return;
  }

  if (psx.buttonJustPressed(PSB_CIRCLE)) { //trigger press
    calibrationStep++;
    changed = true;
  }
  uint16_t x, y;
  if (psx.getGunconCoordinates(x, y) == GUNCON_OK) {
    //moveToCoords(convertRange(minX, maxX, x + offsetX), convertRange(minY, maxY, y + offsetY));
#if GUNCON_FORCE_MODE == 3
    moveToCoords(x + offsetX, y + offsetY);
#else
    moveToCoords(convertRange(minX + offsetX, maxX + offsetX, x), convertRange(minY + offsetY, maxY + offsetY, y));
#endif
  }

#ifdef ENABLE_PSX_GUNCON_MOUSE
  if (enableMouseMove) {
      AbsMouse->sendState();
  } else 
#endif
  if (enableJoystick) {
    //usbStick[0]->sendState();
  }
}

void loopGuncon() {
  if (calibrationStep != 0) {
    runCalibration();
    return;
  }
  
  if (!enableReport) {
    if (!enableMouseMove && !enableJoystick) { //will only configure here on the first time.
      #if defined(GUNCON_FORCE_MODE) && GUNCON_FORCE_MODE >= 0 && GUNCON_FORCE_MODE < 4
        enableReport = true;
        #if GUNCON_FORCE_MODE == 0
          enableMouseMove = true;
        #elif GUNCON_FORCE_MODE == 1
          enableJoystick = true;
        #elif GUNCON_FORCE_MODE == 2
          enableJoystick = true;
          joyOffScreenEdge = true;
        #elif GUNCON_FORCE_MODE == 3
          enableJoystick = true;
          enableReport = true;
        #endif
      #else // GUNCON_FORCE_MODE
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

  }//end if(!enableReport)
  
  if (enableReport) {
    handleButtons();
    PsxControllerProtocol proto = psx.getProtocol();
    switch (proto) {
    case PSPROTO_GUNCON:
      readGuncon();
      break;
    case PSPROTO_DUALSHOCK:
    case PSPROTO_DUALSHOCK2:
      //readDualShock();
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
#ifdef ENABLE_PSX_GUNCON_MOUSE
    if (enableMouseMove) {
        AbsMouse->sendState();
    } else 
#endif
    if (enableJoystick) {
      //usbStick[0]->sendState();
    }
  }
}

void gunconSetup() {

  totalUsb = 1;//MAX_USB_STICKS;
  hasLeftAnalogStick[0] = true;
  
//  #ifdef ENABLE_PSX_GUNCON_MOUSE
//    AbsMouse = new Guncon1_("RZordPSGun", JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_MOUSE, 2);
//    usbStick[0] = new Guncon1_("RZordPSGun", JOYSTICK_DEFAULT_REPORT_ID + 1, JOYSTICK_TYPE_GAMEPAD, 2);
//    AbsMouse->resetState();
//    AbsMouse->sendState();
//  #else
//    usbStick[0] = new Guncon1_("RZordPSGun", JOYSTICK_DEFAULT_REPORT_ID + 1, JOYSTICK_TYPE_GAMEPAD, 1);
//  #endif
//  
//  usbStick[0]->resetState();
//  usbStick[0]->sendState();
}

void gunconSetup2() {
  //change hid mode
  if (canChangeMode()) {
    options.inputMode = INPUT_MODE_HID_GUNCON;
  }  
}

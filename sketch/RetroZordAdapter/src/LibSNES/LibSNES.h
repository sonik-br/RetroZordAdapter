#ifndef SNES_h
#define SNES_h


#include <inttypes.h>

#define SNES_CLOCK  8
#define SNES_STROBE 9
#define SNES_DATA   10


#define SNES_B       0b0000000000000001
#define SNES_Y       0b0000000000000010
#define SNES_SELECT  0b0000000000000100
#define SNES_START   0b0000000000001000
#define SNES_UP      0b0000000000010000
#define SNES_DOWN    0b0000000000100000
#define SNES_LEFT    0b0000000001000000
#define SNES_RIGHT   0b0000000010000000
#define SNES_A       0b0000000100000000
#define SNES_X       0b0000001000000000
#define SNES_L       0b0000010000000000
#define SNES_R       0b0000100000000000


class SNESClass {
    private:
        //uint8_t _clockPin, _strobePin, _dataPin;
        uint16_t _state;
        boolean _haveController;
    public:
        //void begin(uint8_t, uint8_t, uint8_t);
        void begin();
        void update();
        uint16_t getState();
        boolean haveController();
};

extern SNESClass SNES;

#endif




//void SNESClass::begin(uint8_t clockPin, uint8_t strobePin, uint8_t dataPin) {
void SNESClass::begin() {
    //_clockPin = clockPin;
    //_strobePin = strobePin;
    //_dataPin = dataPin;
    _state = 0x00;
    pinMode(SNES_CLOCK, OUTPUT);
    pinMode(SNES_STROBE, OUTPUT);
    pinMode(SNES_DATA, INPUT_PULLUP);
    digitalWrite(SNES_CLOCK, LOW);
    digitalWrite(SNES_STROBE, LOW);
}


void SNESClass::update() {
    /*
       Mapping from http://www.gamesx.com/controldata/snesdat.htm
       Keys: | B Y Select Start Up Down Left Right A X  L  R N/A N/A N/A N/A |
       Bits: | 0 1   2      3    4   5    6    7   8 9 10 11  12  13  14  15 |

       http://problemkaputt.de/fullsnes.htm#snescontrollersioportsautomaticreading
       Bits: 12 to 15 are controller id

       http://problemkaputt.de/fullsnes.htm#snescontrollershardwareidcodes
       Also extended id: bits 16 to 23
     */
    unsigned int fromController = 0x00;
    unsigned int fromControllerExtended = 0x00;
    uint8_t i;

    //Do the strobe to start reading button values
    digitalWriteFast(SNES_STROBE, HIGH);
    delayMicroseconds(12);
    digitalWriteFast(SNES_STROBE, LOW);

    //Wait for controller to start sending the button values 
    delayMicroseconds(6);
    for (i = 0; i < 16; i++) {
        //read the value, shift it and store it as a bit on fromController:
        fromController |= digitalRead(SNES_DATA) << i;

        //More one cycle on the clock pin...
        digitalWriteFast(SNES_CLOCK, HIGH);
        delayMicroseconds(6);
        digitalWriteFast(SNES_CLOCK, LOW);
        delayMicroseconds(6);
    }
    for (i = 0; i < 8; i++) {
        //read the value, shift it and store it as a bit on fromControllerExtended:
        fromControllerExtended |= digitalRead(SNES_DATA) << i;

        //More one cycle on the clock pin...
        digitalWriteFast(SNES_CLOCK, HIGH);
        delayMicroseconds(6);
        digitalWriteFast(SNES_CLOCK, LOW);
        delayMicroseconds(6);
    }

    //return the read bits (16)

    _state = ~fromController;

    // 0000.00000000 No controller connected
    // 0000.11111111 Normal Joypad
    _haveController = !(((fromController >> 12) == 0xF) & (fromControllerExtended == 0xFF));
}


uint16_t SNESClass::getState() {
    return _state;
}

boolean SNESClass::haveController() {
    return _haveController;
}

SNESClass SNES;

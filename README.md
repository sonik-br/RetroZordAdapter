# RetroZordAdapter
Arduino multiple controller adapter

This is a collection of my various adapters but using a single arduino.

Input connectors:

- 2 x Saturn (compatible with megadrive db9)
- 1 x Snes
- 1 x Psx

Three-position switch to select the input mode during boot:
* Left - Saturn
* Middle - Psx
* Right - Snes


## Saturn / Megadrive Mode

Serial ID is RetroZordSat

More info at [SaturnMultiUSB](https://github.com/sonik-br/SaturnMultiUSB).

#### Saturn / Megadrive button mapping

| Saturn | HID | PS3     | Megadrive          |
|--------|-----|---------|--------------------|
| D-PAD  | HAT | D-PAD   |                    |
| A      | 1   | X       |                    |
| B      | 2   | O       |                    |
| C      | 5   | R1      |                    |
| X      | 0   | &#9723; |                    |
| Y      | 3   | &#9651; |                    |
| Z      | 4   | L1      |                    |
| L      | 6   | L2      | "Home" on M30 2.4G |
| R      | 7   | R2      | Mode               |
| Start  | 9   | Start   |                    |


## Snes Mode

Serial ID is RetroZordSnes

#### Snes button mapping

| Snes   | HID | PS3     |
|--------|-----|---------|
| D-PAD  | HAT | D-PAD   |
| B      | 1   | X       |
| A      | 2   | O       |
| Y      | 0   | &#9723; |
| X      | 3   | &#9651; |
| L      | 4   | L1      |
| R      | 5   | R1      |
| Select | 8   | Select  |
| Start  | 9   | Start   |


## PlayStation Mode
More info at [GunconDuino](https://github.com/sonik-br/GunconDuino).

~~DualShock only works in analog mode.
Non-DualShock (digital controller) not supported.~~

#### Mode selection during boot

| Mode                | Serial ID         | During boot                             | Obs          |
|---------------------|-------------------|-----------------------------------------|--------------|
| Digital / DualShock | RetroZordPsDS1    | Digital, DualShock or nothing connected | Default mode |
| Guncon              | RetroZordPsGun    | Guncon connected                        |              |
| NeGcon              | RetroZordPsNeGcon | NeGcon connected                        |              |
| JogCon Mouse        | RetroZordPsJogCon | JogCon connected and holding L2         |              |
| JogCon MiSTer       | MiSTer-A1 JogCon  | JogCon connected and holding L1         |              |


#### PlayStation button mapping

| DualShock | Guncon    | HID     | PS3     |
|-----------|-----------|---------|---------|
| D-PAD     | D-PAD     | HAT     | D-PAD   |
| L-Stick   |           | L-Stick | L-Stick |
| R-Stick   |           | R-Stick | R-Stick |
| X         | B (Right) | 1       | X       |
| O         | Trigger   | 2       | O       |
| R1        |           | 5       | R1      |
| &#9723;   |           | 0       | &#9723; |
| &#9651;   |           | 3       | &#9651; |
| L1        |           | 4       | L1      |
| L2        |           | 6       | L2      |
| R2        |           | 7       | R2      |
| L3        |           | 10      | L3      |
| R3        |           | 11      | R3      |
| Select    |           | 8       | Select  |
| Start     | A (Left)  | 9       | Start   |


| NeGcon    | HID      |
|-----------|----------|
| D-PAD     | HAT      |
| Twist     | Steering |
| I         | Throttle |
| II        | Brake    |
| A         | 0        |
| B         | 1        |
| L1        | ZAxis    |
| R1        | 2        |
| Start     | 3        |


| JogCon    | HID         |
|-----------|-------------|
| D-PAD     | HAT         |
| Jog       | Wheel (Abs) |
| Jog       | Dial (Rel)  |
| Jog       | Mouse       |
| X         | 11          |
| O         | 10          |
| &#9723;   | 12          |
| &#9651;   | 9           |
| L1        | 7           |
| L2        | 5           |
| R1        | 8           |
| R2        | 6           |
| Select    | 3           |
| Start     | 4           |

### Guncon

##### Use a DualShock
Special mode for helping map the axis for the guncon mode on emulators:
* Boot the device with a dualshock connected and holding the select button.
* Enable analog mode and proceed as if it was a guncon.

##### Mode Selection
When connecting it to a PC it will be in a not enabled state.<br/>
It's possible to enabe it as a mouse or as a joystick:

| Guncon        | DualShock | Mode     |
| ------------- |---------- | -------- |
| Trigger       | Circle    | Mouse    |
| A (Left side) | Start     | Joystick |

After the initial selection it will keep it until shutdown.

##### Offset correction: (Needs better documentation)
* Point offscreen.<br/>
Disable the reporting by pressing the trigger while holding A and B.

* Press B to initiate the Offset calibration mode.<br/>
It will adjust the X axis first then the Y axis.

* For the X axis press A to move the cursor left or B to move right.<br/>
Press trigger to confirm.

* *For the Y axis press A to move the cursor up or B to move down.<br/>
Press trigger to confirm and end calibration.


### JogCon special config in real time

**Mode switch** by pressing "mode" button while holding:
- L2 - paddle mode (with FF stoppers)
- R2 - steering mode (FF always enabled / centering)
- L2+R2 - spinner mode (no FF)

**Force Feedback strength** by pressing "mode" button while holding:
- &#9651; - 1
- O - 3
- X - 7
- &#9723; - 15

**Spinner pulses per step adjust** by pressing "mode" button while holding:
- Up - 1
- Right - 2
- Down - 4
- Left - 8

**Paddle / steering range switch** by pressing "mode" button while holding:
- L1 - 270 degree
- R1 - 135 degree


## Arduino Leonardo pin mapping


| Arduino     | Saturn-1 | Saturn-2 | SNES    | PSX                     | Other  |
|-------------|----------|----------|---------|-------------------------|--------|
| GND         | 9        | 9        | 7       | 4                       |        |
| 5V          | 1        | 1        | 1       | 5 - convert to **3.3v** |        |
| A0          | 7        |          |         |                         |        |
| A1          | 6        |          |         |                         |        |
| A2          | 4        |          |         |                         |        |
| A3          | 8        |          |         |                         |        |
| A4          | 3        |          |         |                         |        |
| A5          | 2        |          |         |                         |        |
| 2           |          | 3        |         |                         |        |
| 3           |          | 2        |         |                         |        |
| 4           |          | 8        |         |                         |        |
| 5           |          | 6        |         |                         |        |
| 6           |          | 7        |         |                         |        |
| 7           | 5        |          |         |                         |        |
| 8           |          | 4        | 2 - CLK |                         |        |
| 9           |          | 5        | 3 - LAT |                         |        |
| 10          |          |          | 4 - DAT |                         |        |
| 11          |          |          |         | 6 - ATT                 |        |
| 12          |          |          |         |                         | Switch |
| ICSP 1      |          |          |         | 1 - DAT                 | MISO   |
| ICSP 4      |          |          |         | 2 - CMD                 | MOSI   |
| ICSP 3      |          |          |         | 7 - CLK                 | SCK    |

**PSX Power must use a voltage converter (5V > 3.3v). <br/>
The four PSX data pins must goes through a logic-level shifter (5V <-> 3.3v).**

## Credits

For the saturn adapter [SaturnLib](https://github.com/sonik-br/SaturnLib).<br/>
For the guncon and most of the ps stuff [GunconDuino](https://github.com/sonik-br/GunconDuino).<br/>

MiSTer-A1 JogCon from [JogConUSB](https://github.com/MiSTer-devel/Retro-Controllers-USB-MiSTer/tree/master/JogConUSB) by sorgelig.

Jogcon FFB commands from [pscontroller-rs](https://github.com/RandomInsano/pscontroller-rs) by RandomInsano.

Snes controller reading based on [Snes Library](https://github.com/turicas/SNES) by turicas.

#### This uses modified versions of:

[PsxNewLib](https://github.com/SukkoPera/PsxNewLib) by SukkoPera.<br/>
[absmouse](https://github.com/jonathanedgecombe/absmouse) by jonathanedgecombe.<br/>
[ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) by MHeironimus.<br/>

Special thanks to my father for the amazing support and for doing all the electronics stuff.

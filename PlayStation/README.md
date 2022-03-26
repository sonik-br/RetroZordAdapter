## PlayStation Mode
More info at [GunconDuino](https://github.com/sonik-br/GunconDuino).

#### Mode selection during boot

| Mode                | Serial ID        | During boot                          | Obs          |
|---------------------|------------------|--------------------------------------|--------------|
| Digital / DualShock | RZordPsDS1       | Digital, Analog or nothing connected | Default mode |
| Guncon              | RZordPsGun       | Guncon connected                     |              |
| NeGcon              | RZordPsNeGcon    | NeGcon connected                     |              |
| JogCon Mouse        | RZordPsJogCon    | JogCon connected and holding L2      |              |
| JogCon MiSTer       | MiSTer-A1 JogCon | JogCon connected and holding L1      |              |


#### PlayStation button mapping

| DualShock | HID     | PS3     |
|-----------|---------|---------|
| D-PAD     | HAT     | D-PAD   |
| L-Stick   | L-Stick | L-Stick |
| R-Stick   | R-Stick | R-Stick |
| X         | 1       | X       |
| O         | 2       | O       |
| R1        | 5       | R1      |
| &#9723;   | 0       | &#9723; |
| &#9651;   | 3       | &#9651; |
| L1        | 4       | L1      |
| L2        | 6       | L2      |
| R2        | 7       | R2      |
| L3        | 10      | L3      |
| R3        | 11      | R3      |
| Select    | 8       | Select  |
| Start     | 9       | Start   |


| Guncon    | DualShock | HID | Mouse   |
|-----------|-----------|-----|---------|
| Trigger   | O         | 1   | Left    |
| A (Left)  | Start     | 2   | Right   |
| B (Right) | X         | 3   | Middle  |


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

#### Use a DualShock
Special mode for helping map the axis for the guncon mode on emulators:
* Boot the device with a dualshock connected and holding the select button.
* Enable analog mode and proceed as if it was a guncon.

#### Mode Selection
When connecting it to a PC it will be in a not enabled state.<br/>
It's possible to enabe it as a mouse or as a joystick:

| Guncon         | DualShock | Mode     | Off-screen mode |
|----------------|-----------|----------|-----------------|
| Trigger        | Circle    | Mouse    | Bottom left     |
| A (Left side)  | Start     | Joystick | Center          |
| B (Right side) | Cross     | Joystick | Bottom left     |

After the initial selection it will keep it until shutdown.

It's possible to force a single guncon mode if desired. 
Edit the file `Psx.h` file and uncomment/define `GUNCON_FORCE_MODE`

#### Offset correction: (Needs better documentation)
* Point offscreen.<br/>
Disable the reporting by pressing the trigger while holding A and B.

* Press B to initiate the Offset calibration mode.<br/>
It will adjust the X axis first then the Y axis.

* For the X axis press A to move the cursor left or B to move right.<br/>
Press trigger to confirm.

* For the Y axis press A to move the cursor up or B to move down.<br/>
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

| Arduino     | PSX                     | Other        |
|-------------|-------------------------|--------------|
| GND         | 4                       |              |
| 5V          | 5 - convert to **3.3v** |              |
| 11          | 6 - ATT                 |              |
| ICSP 1      | 1 - DAT                 | MISO         |
| ICSP 4      | 2 - CMD                 | MOSI         |
| ICSP 3      | 7 - CLK                 | SCK          |
| VIN         | 3 - vibration/motors    | **Optional** |

**Important!**

PSX Power must use a voltage converter (5V > 3.3v). <br/>
The four PSX data pins must goes through a logic-level shifter (5V <-> 3.3v).

If using the JogCon with it's motors active, it's highly recommended to supply the correct voltage to the controller.<br/>
This can be done by powering the Leonardo with an external 9V 1A PSU (center positive) and connecting the VIN pin to the controller's pin 3.

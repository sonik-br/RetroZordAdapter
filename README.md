# RetroZordAdapter
Arduino multiple controller adapter

This is a collection of my various arduino based adapters.

The [output](#output-modes-and-features) can be configured to:
- Dinput
- Xinput
- Switch

There's adapters for: <br/>
- [Saturn](Saturn/) (compatible with megadrive db9)
  > Saturn Normal pad, Fighting pad, Twin Stick,
  Saturn Analog pad, Racing wheel,
  Saturn multitap <br/>
  Megadrive pad,
  Megadrive multitap
- [Snes](SNES/) <br/>
  > Snes pad,
  Snes NTT pad,
  Snes multitap,
  Nes pad
  
- [PlayStation](PlayStation/) <br/>
  > Normal pad,
  Analog Stick,
  DualShock,
  DualShock 2,
  Guncon,
  NeGcon,
  Paddle,
  JogCon,
  Jet de GO

And also a more complex adapter including the 3 adapters on a single arduino
- [Multi](Multi/)

Each adapter have at least a unique USB Serial ID.<br>
This is usefull on MiSTer as it can distinguish each Arduino based adapter by it's ID and each adapter will have it's own input mapping.<br/>
The PlayStation adapter have multiple IDs as it implements various output modes.

Arduino's LED_BUILTIN will blink when no controller is connected. It's cool to have the LED visible if building the adapter inside a case.


## Output modes and features

|                 | Dinput | Xinput | Switch | PS3          |
|-----------------|--------|--------|--------|--------------|
| Output devices  | 5      | 1      | 5      | 5            |
| Analog Sticks   | Yes    | Yes    | Yes    | Yes          |
| Analog Triggers | No     | Yes    | No     | Yes          |
| Buttons         | 24     | 13     | 14     | 13           |
| Rumble          | No     | Yes    | No     | No           |

The firmware uses MPG/GP2040 button names:

| MPG | DInput | XInput | Switch  | PS3      |
|-----|--------|--------|---------|----------|
| B1  | 1      | A      | B       | Cross    |
| B2  | 2      | B      | A       | Circle   |
| B3  | 3      | X      | Y       | Square   |
| B4  | 4      | Y      | X       | Triangle |
| L1  | 5      | LB     | L       | L1       |
| R1  | 6      | RB     | R       | R1       |
| L2  | 7      | LT     | ZL      | L2       |
| R2  | 8      | RT     | ZR      | R2       |
| S1  | 9      | Back   | Minus   | Select   |
| S2  | 10     | Start  | Plus    | Start    |
| L3  | 11     | LS     | LS      | L3       |
| R3  | 12     | RS     | RS      | R3       |
| A1  | 13     | Guide  | Home    | PS       |
| A2  | 14     | -      | Capture | -        |

Output mode selection during boot:

| Output Mode | Button Held |
|-------------|-------------|
| Switch      | B1          |
| XInput      | B2          |
| PS3         | B3          |

It defaults to Dinput mode.

Xinput mode is not compatible with xbox consoles.

Switch mode is also compatible with ps3 console but without analog trigger.

For usage on [MiSTer](https://github.com/MiSTer-devel/Main_MiSTer/wiki) it's recommended to use it in Dinput mode.<br/>
Some special modes will only be activated when in Dinput mode. Like the Guncon mode, and Jogcon as spinner.


## Building
One time process: Install the [LUFA](https://github.com/CrazyRedMachine/Arduino-Lufa#installation) board definition first. Then select it before building/flashing.

After flashing the device with a LUFA based firmware, you will have to press the reset button to reflash again.<br/>
If building the adapter inside a casing, I recommend to leave the reset button accessible.

Copy the contents of `_must_copy_to_sketch_folder` to the same folder as the arduino sketch file you want to use.

Use the recommended pins for each adapter.

All required libs are included.

For advanced users:<br/>
All pins can be customized except the PlayStation DAT, CMD and CLK pins.<br/>
The mode selection switch of the Multi adapter must go to an analog pin.

## Notes for MiSTer users
For guncon use it's recommended to enable [fast usb pooling](https://github.com/MiSTer-devel/Main_MiSTer/wiki/Lag-Explained#input-lag) or the cursor will be laggy.

The bellow steps are only required if using MiSTer Main older than [2024-01-10](https://github.com/MiSTer-devel/Main_MiSTer/blob/master/releases/MiSTer_20240110) or if you change the vid:pid of the device.

If using the default vid:pid, and with a recent MiSTer Main build, it should already work without any further configuration.

If using the arduino's vid:pid, it will require some configuration.

The settings are required for multiple inputs to work on MiSTer.

It's a two step process:

- Configure `no_merge_vidpid` on your `MiSTer.ini` file
  
  `no_merge_vidpid=0x23418036`

- Configure `HID_QUIRK_MULTI_INPUT` on your `u-boot.txt` file (it's inside the linux folder)

  Append `usbhid.quirks=0x2341:0x8036:0x040` to the file

  Result file will look like this

  `v=loglevel=4 usbhid.jspoll=1 xpad.cpoll=1 usbhid.quirks=0x2341:0x8036:0x040`

## Usb vid:pid
By default it will use RetroZord's usb vid:pid.<br/>
This is not available for commercial usage!<br/>
Use it on personal projects only. If doing a commercial product, use your own vid:pid.

## Credits

For the saturn adapter [SaturnLib](https://github.com/sonik-br/SaturnLib).<br/>
For the snes adapter [SnesLib](https://github.com/sonik-br/SnesLib).<br/>
For the guncon [GunconDuino](https://github.com/sonik-br/GunconDuino).<br/>

MiSTer-A1 JogCon from [JogConUSB](https://github.com/MiSTer-devel/Retro-Controllers-USB-MiSTer/tree/master/JogConUSB) by sorgelig.

Jogcon FFB commands from [pscontroller-rs](https://github.com/RandomInsano/pscontroller-rs) by RandomInsano.

PS controller pinout by [curiousinventor](https://store.curiousinventor.com/guides/PS2).

[Arduino LUFA](https://github.com/CrazyRedMachine/Arduino-Lufa#installation) by Palatis
 and CrazyRedMachine.

#### This uses modified versions of:

[MPG](https://github.com/OpenStickCommunity/MPG) by FeralAI.<br/>
[PsxNewLib](https://github.com/SukkoPera/PsxNewLib) by SukkoPera.<br/>

Special thanks to my father for the amazing support and for doing all the electronics and soldering stuff.

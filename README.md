# RetroZordAdapter
Arduino multiple controller adapter

This is a collection of my various arduino based adapters.

There's adapters for: <br/>
- [Saturn](Saturn/) (compatible with megadrive db9)
  > Saturn Normal pad, Fighting pad, Twin Stick,
  Saturn Analog pad, Racing wheel,
  Saturn multitap <br/>
  Megadrive pad,
  Megadrive multitap
- [Snes](SNES/) <br/>
  > Snes pad,
  Snes NTT pad (untest),
  Snes multitap,
  Nes pad
  
- [PlayStation](PlayStation/) <br/>
  > Normal pad,
  DualShock,
  DualShock 2,
  Guncon,
  NeGcon,
  JogCon

And also a more complex adapter including the 3 adapters on a single arduino
- [Multi](Multi/)


Arduinos's LED_BUILTIN will blink when no controller is connected.

## Building
Copy the contents of "_must_copy_to_sketch_folder" to the same folder as the arduino sketch file you want to use.

Configure the used pins or use the recommended ones.

All required libs are included.


## Notes for [MiSTer](https://github.com/MiSTer-devel/Main_MiSTer/wiki) users
For guncon use it's recommended to enable [fast usb pooling](https://github.com/MiSTer-devel/Main_MiSTer/wiki/Lag-Explained#input-lag) or the cursor will be laggy.

For multitap and guncon to work you will have to configure MiSTer.

It's a two step process:

- Configure `no_merge` on your `MiSTer.ini` file

  `no_merge_vid=0x2341`<br/>
  `no_merge_pid=0x8036 `
  
  or
  
  `no_merge_vidpid=0x23418036`

- Configure `HID_QUIRK_MULTI_INPUT`

  Edit the fie `u-boot.txt` inside linux folder

  Append `usbhid.quirks=0x2341:0x8036:0x040` to the file

  Result file will look like this

  `v=loglevel=4 usbhid.jspoll=1 xpad.cpoll=1 usbhid.quirks=0x2341:0x8036:0x040`


## Credits

For the saturn adapter [SaturnLib](https://github.com/sonik-br/SaturnLib).<br/>
For the snes adapter [SnesLib](https://github.com/sonik-br/SnesLib).<br/>
For the guncon [GunconDuino](https://github.com/sonik-br/GunconDuino).<br/>

MiSTer-A1 JogCon from [JogConUSB](https://github.com/MiSTer-devel/Retro-Controllers-USB-MiSTer/tree/master/JogConUSB) by sorgelig.

Jogcon FFB commands from [pscontroller-rs](https://github.com/RandomInsano/pscontroller-rs) by RandomInsano.

PS controller pinout by [curiousinventor](https://store.curiousinventor.com/guides/PS2).

#### This uses modified versions of:

[PsxNewLib](https://github.com/SukkoPera/PsxNewLib) by SukkoPera.<br/>
[ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) by MHeironimus.<br/>
[Gamepad](https://github.com/MickGyver/DaemonBite-Retro-Controllers-USB) by MickGyver.<br/>

Special thanks to my father for the amazing support and for doing all the electronics stuff.

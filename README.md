# RetroZordAdapter
Arduino multiple controller adapter

This is a collection of my various arduino based adapters.

There's adapters for: <br/>
- [Saturn](Saturn/) (compatible with megadrive db9)
- [Snes](SNES/)
- [PlayStation](PlayStation/)

And also a more complex adapter including the 3 adapters on a single arduino
- [Multi](Multi/)


Arduinos's LED_BUILTIN will blink when no controller is connected.

## Building
Copy the contents of "_must_copy_to_sketch_folder" to the same folder as the arduino sketch file you want to use.

Configure the used pins or use the recommended ones.

All required libs are included.

## Credits

For the saturn adapter [SaturnLib](https://github.com/sonik-br/SaturnLib).<br/>
For the snes adapter [SnesLib](https://github.com/sonik-br/SnesLib).<br/>
For the guncon [GunconDuino](https://github.com/sonik-br/GunconDuino).<br/>

MiSTer-A1 JogCon from [JogConUSB](https://github.com/MiSTer-devel/Retro-Controllers-USB-MiSTer/tree/master/JogConUSB) by sorgelig.

Jogcon FFB commands from [pscontroller-rs](https://github.com/RandomInsano/pscontroller-rs) by RandomInsano.

#### This uses modified versions of:

[PsxNewLib](https://github.com/SukkoPera/PsxNewLib) by SukkoPera.<br/>
[ArduinoJoystickLibrary](https://github.com/MHeironimus/ArduinoJoystickLibrary) by MHeironimus.<br/>
[Gamepad](https://github.com/MickGyver/DaemonBite-Retro-Controllers-USB) by MickGyver.<br/>

Special thanks to my father for the amazing support and for doing all the electronics stuff.

# RetroZordAdapter
Arduino multiple controller adapter

This is a collection of my various adapters but using a single arduino.

Input connectors:

- 2 x Saturn (compatible with megadrive db9)
- 1 x Snes
- 1 x Psx

Three-position switch to select the input mode during boot:
- Left - [Saturn](../Saturn/)
- Middle - [Psx](../PlayStation/)
- Right - [Snes](../SNES/)


## Combined Arduino Leonardo pin mapping

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

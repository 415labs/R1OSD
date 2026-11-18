# R1OSD: GPS on-screen display for FPV / long-range RC planes

R1OSD is a small board that sits between the onboard camera and the video
transmitter of an RC plane. It overlays flight data on the live video:
altitude, speed, distance and an arrow pointing home, battery voltage, current,
power and consumed capacity, flight time, and warnings. The pilot wearing video
goggles sees it all on top of the picture.

I designed and built it in 2009. This repository holds the firmware (V4.0,
"NG"), the serial bootloader I used to flash it, and a Windows NMEA GPS
simulator I wrote to test the OSD on the bench.

▶ **Flight video:** <https://www.youtube.com/watch?v=ocbuLWeXUxg>
📰 **Featured on Hackaday** (Sept 2010): [Augmented Reality UAV Controller](https://hackaday.com/2010/09/10/augmented-reality-uav-controller/)

---

## Features

**In-flight screen**
- Altitude relative to the launch point (m), ground speed (km/h), climb rate (m/s)
- Distance to home (m, switching to km past 1 km) plus a 16-direction arrow pointing home
- Battery voltage, current, power (W) and consumed capacity (mAh)
- Flight timer, which starts automatically once the plane is moving and away from home
- Blinking warnings for max altitude, max distance, max speed, low voltage, high current and capacity used

**Other screens**
- Boot splash, then a "waiting for GPS fix" screen showing satellites, fix type and HDOP/PDOP
- Home position stored automatically once the fix is good enough (sat count / HDOP thresholds)
- Post-flight review screen after landing: max speed, altitude and distance, total distance flown, flight time, max current, lowest voltage, battery used
- On-screen menus driven by an **RC5 IR remote** or a 3-button nav switch: OSD mode, warning thresholds, logger options, tools, IR code viewer, reset to defaults. Settings are saved to EEPROM.
- PAL and NTSC support
- GPS fixes rejected when HDOP is above a configurable threshold

> The GPS logger / SD-card menus exist but were never finished (`formatSDCard()` is a stub).

## Hardware

| Function | Part | MCU pins |
|---|---|---|
| MCU | ATmega324P @ 20 MHz | |
| Video overlay | Maxim MAX7456 | SPI: PB4 /CS, PB5 MOSI, PB6 MISO, PB7 SCK. Reset: PA5 |
| GPS | Locosys LS20033 (MediaTek), set to 5 Hz, RMC + GGA + GSA | UART1 (PD2/PD3), 38400 baud |
| PC / debug / bootloader | | UART0 (PD0/PD1). App: 38400 baud. Bootloader: 57600 baud |
| IR receiver (RC5) | TSOP-style demodulator | INT2 (PB2), Timer0 |
| Nav buttons | down / enter / up | PD5 / PD6 / PD7 (pin-change interrupt) |
| Battery voltage | divider R1 = 6.34 kΩ, R2 = 1 kΩ | ADC1 (PA1) |
| Battery current | Allegro 50 A Hall-effect sensor | ADC0 (PA0) |
| Status LED | | PC2 |
| SD card (planned) | | /CS on PB0 |

The schematic and PCB files are not in this archive.

## Repository layout

```
firmware/              AVR firmware (avr-gcc)
  src/                 sources
  charset/             MAX7456 character tables (default font + custom arrows/symbols)
  prebuilt/            original 2009 .hex/.eep build of V4.0
bootloader/            MegaLoad serial bootloader configured for ATmega324P / 20 MHz
tools/nmea-simulator/  Delphi (BDS 2006) app that sends fake NMEA to the OSD over a COM port
```

## Building the firmware

You need `avr-gcc`, `avr-libc` and `make`. On Debian/Ubuntu: `sudo apt install gcc-avr avr-libc binutils-avr`.
On macOS: `brew tap osx-cross/avr && brew install avr-gcc`.

```sh
cd firmware
make            # -> build/r1osd.hex
make size
make program PROGRAMMER=avrisp2 PORT=usb   # optional, flash over ISP with avrdude
```

The code was first built with WinAVR / AVR Studio 4 in 2009. A few small
changes let it build with current toolchains (tested with avr-gcc 7.3 /
avr-libc 2.0):

- `rc5-avr.c`: `SIGNAL(SIG_OVERFLOW0)` / `SIGNAL(SIG_INTERRUPT2)` became
  `ISR(TIMER0_OVF_vect)` / `ISR(INT2_vect)`. Modern avr-libc dropped the old
  names, so without this change the IR remote would silently stop working.
- `max7456_software_spi.c`: maps `SPCR/SPDR/SPSR` to the `SPCR0/...` names
  used by newer avr-libc headers.
- `menus.c`: the `PROGMEM` menu tables are now `const char * const`.

The rebuilt image wires up the same interrupt vectors as the original 2009
binary. Build-time options sit at the top of `src/main.c` (`DEBUG`: take NMEA
from UART0 instead of the GPS, `DEBUG_TO_SERIAL`) and in `src/variables.h`
(default thresholds).

Most code comments are in French.

## Flashing

1. Program the bootloader once over ISP and set the fuses. See [bootloader/README.md](bootloader/README.md).
2. After that, flash `firmware/build/r1osd.hex` (or the prebuilt hex) over UART0 with MegaLoad.

You can also skip the bootloader and run `make program` with an ISP programmer.

## Credits

R1OSD builds on these open-source pieces. Thanks to their authors:

- **MAX7456 driver**: Claas Anders "CaScAdE" Rathje, from [C-OSD](http://www.mylifesucks.de/oss/c-osd/) (GPL v2): `firmware/src/max7456_software_spi.[ch]`
- **RC5 IR decoder**: Georg-Johann Lay, from <http://www.gjlay.de/pub/c-code/rc5.html>: `firmware/src/rc5*.{c,h}`
- **MegaLoad bootloader**: Sylvain Bissonnette, [MicroSyl](http://www.microsyl.com/): `bootloader/`
- The MikroKopter C-OSD project, which inspired the screen layout and character set

## License

Because the firmware includes GPL v2 code (the C-OSD MAX7456 driver), it is
released under the **GNU General Public License v2**. See [LICENSE](LICENSE).
Third-party files keep their original headers. The MegaLoad bootloader in
`bootloader/` is a separate program, distributed under its author's original
"free, as is, without warranty" terms.

© 2009 Fabien Blanc-Paques

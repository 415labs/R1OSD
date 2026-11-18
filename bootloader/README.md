# MegaLoad bootloader (ATmega324P, 20 MHz, 57600 baud, UART0)

This is MegaLoad 7.3 by Sylvain Bissonnette ([MicroSyl](http://www.microsyl.com/)),
configured for the R1OSD board. With it you can reflash the firmware over the
serial port without an ISP programmer.

Changes from the stock source in `src/main.c`:

- `MEGATYPE Mega324`, `XTAL 20000000`
- `PageByte` forced to **128** for the ATmega324P (the stock defines picked the wrong value)

## Install the bootloader (one time, with an ISP programmer)

1. Flash `prebuilt/megaload-atmega324p-20mhz-57600-uart0.hex`. It is linked at byte address `0x7C00`.
2. Set the fuses:
   - **BOOTSZ = 512 words (1 KB), boot start at word `0x3E00`**. The ATmega324P datasheet gives
     the boot size in words. The old notes said "512 bytes", which is wrong for this build.
   - **BOOTRST programmed**, so the MCU jumps to the bootloader on reset
   - External 20 MHz crystal clock source
   - Leave `RSTDISBL` alone. Setting it locks out ISP.

Example with avrdude:

```sh
avrdude -p m324p -c avrisp2 -P usb -U flash:w:prebuilt/megaload-atmega324p-20mhz-57600-uart0.hex:i
# then set the fuses for your setup (BOOTSZ1=0, BOOTSZ0=1, BOOTRST=0, plus the crystal clock settings)
```

## Flash the firmware

1. Install MegaLoad (Windows) from <http://www.microsyl.com/>.
2. Open the firmware `.hex` and select the COM port at 57600 baud.
3. Reset the board. MegaLoad catches the bootloader handshake and programs the chip.

## Rebuilding

The source builds with ImageCraft **ICCAVR 7**: open `src/BootLoad.prj`,
select ATmega324P, and set a 1 KB boot area. It does not build with avr-gcc
as is.

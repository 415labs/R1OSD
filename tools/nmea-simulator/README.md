# NMEA simulator

A small Windows app I used to test R1OSD on the bench without flying. It
writes `$GPRMC`, `$GPGGA` and `$GPGSA` sentences to a serial port. From the UI
you can:

- move the simulated aircraft north, south, east or west, and change altitude, speed and heading
- switch the fix between none, 2D and 3D, set it valid or invalid, and change the satellite count
- adjust HDOP, VDOP and PDOP
- inject glitches (bad checksum, latitude jumps) to test how the firmware handles bad data

To feed it into the OSD, build the firmware with `DEBUG 1` in `firmware/src/main.c`.
The NMEA input then comes from UART0 (the PC port) instead of the GPS on UART1.

## Building

- Borland Developer Studio 2006 (Delphi). Recent Delphi / RAD Studio versions should open `NMEA_Simulator.dpr`.
- Requires the **ComPort Library** (`CPort` unit, TComPort component). It is
  available on SourceForge as "ComPort Library" and through GetIt.

# MAX7456 character tables

These are character bitmaps for the MAX7456 font memory: 256 characters,
54 bytes each, 12×18 pixels at 2 bits per pixel. They are not part of the
firmware build. They were used once to load the font into the MAX7456 NVM
with `learn_char()`.

- `default_charset.c`: the full character set, converted from `DEFAULTCM.MCM`
- `characters.c`: the 16 home-direction arrows (chars 176–191), drawn from C-OSD artwork
- `bitmaps.c`: other custom glyphs

# fcinfo

fcinfo is a simple tool for analyzing ROM files for NES, Famicom, and related systems. It outputs various metadata from ROM, such as the iNES/NES 2.0 header, the official Nintendo header (used by the FamicomBox), hardware vectors, and free space in ROM.

Free space is measured differently from [Shiru's NES Space Checker](https://forums.nesdev.org/viewtopic.php?t=8476), fcinfo looks for the largest block of null bytes in PRG ROM banks, and counts redundant tiles in CHR ROM banks.

### Example output
```
$ fcinfo -a game.nes
NES 2.0 header:
 4e 45 53 1a 02 01 01 08 00 00 07 00 00 00 00 01

 PRG-ROM size: 32 KiB
 CHR-ROM size: 8 KiB
 Mapper: 0
 Battery-backed: no
 Mirroring: vertical
 Trainer: no
 System: NES/FC/Dendy

 Submapper: 0
 PRG-RAM size:   8192 B
 PRG-NVRAM size: 0 B
 CHR-RAM size:   0 B
 CHR-NVRAM size: 0 B

 Frame timing: RP2C02 (NTSC)
 Misc ROMs: 0
 Input Device: Standard NES/Famicom controllers

Official header:
 Title: FCINFO TEST
 Title encoding: JIS X 0201
 Title length: 11 B
 Licensee code: 0x00
 PRG-ROM checksum: 0x873a
 CHR-ROM checksum: 0xc40d
 Complementary checksum: 0xfe

 PRG-ROM size: 32 KiB
 CHR size:     8 KiB
 CHR memory type: ROM
 Mirroring: vertical
 Mapper: NROM

Hardware vectors CPU address (ROM address):
 Vblank NMI:   0x8035 (0x000045)
 Entry point:  0x8000 (0x000010)
 External IRQ: 0x805c (0x00006c)

ROM space:
 Free space in PRG-ROM bank 0: 251 bytes
 Free space in PRG-ROM bank 1: 14853 bytes

 Free space in CHR-ROM page 0: 2 tiles
 Free space in CHR-ROM page 1: 26 tiles
```
# fcinfo

fcinfo is a simple tool for analyzing ROM files for NES, Famicom, and related systems. It reads various metadata from the ROM file, such as the iNES/NES 2.0 header, PRG and CHR ROM sizes, hardware vectors, and free space in ROM.

Free space is measured differently from [Shiru's NES Space Checker](https://forums.nesdev.org/viewtopic.php?t=8476), fcinfo looks for the largest block of null bytes in PRG ROM banks, and counts redundant (non-unique) tiles in CHR ROM banks.

### Example output
```
$ fcinfo -a game.nes
NES 2.0 header:
 4e 45 53 1a 02 01 01 08 00 00 07 00 02 00 00 01

 PRG ROM size: 32 KiB
 CHR ROM size: 8 KiB
 Mapper: 0
 Battery-backed: no
 Mirroring: vertical
 Trainer: no
 System: NES/FC/Dendy

 Submapper: 0
 PRG RAM size:   8192 B
 PRG NVRAM size: 0 B
 CHR RAM size:   0 B
 CHR NVRAM size: 0 B

 Frame timing: Dual-region
 Misc ROMs: 0
 Input Device: 0x01

Hardware vectors:
 NMI:          0x8035
 Entry point:  0x8000
 External IRQ: 0x805c

ROM space:
 Free space in PRG ROM bank 0: 267 bytes
 Free space in PRG ROM bank 1: 16378 bytes

 Free space in CHR ROM bank 0: 2 tiles
 Free space in CHR ROM bank 1: 4 tiles
```
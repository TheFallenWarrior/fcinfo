
#include <stdint.h>
#include <stdio.h>

#include "base.h"

// https://www.nesdev.org/wiki/INES
// https://www.nesdev.org/wiki/NES_2.0
uint8_t iNesHeader[16];

// https://www.nesdev.org/wiki/Nintendo_header
uint8_t officialHeader[26];

uint16_t vectors[3]; // HW vector addresses in CPU memory
int absVectors[3];   // HW vector addresses in ROM

uint8_t uniqueTilesBuf[256][16];
int *uniqueTileCounter;
int *emptySpacePrg;

int64_t prgSize;
int64_t chrSize;
int mapper;
int hasTrainer;
int isNes2;

int hasOfficialHeader;
char gameTitle[16];

// Return the file offset of the given CPU address, mapped to the two last PRG banks
// This uses a heuristic that assumes the full PRG is visible for NROM (16 or 32 KiB) ROMs,
//  or that the last bank is fixed at 0xC000-0xFFFF (MMC style).
uint32_t getLastBankOffset(uint16_t addr) {
	uint32_t bank = (
		(prgSize >= 2 && !(addr&0x4000)) ?
		prgSize - 2 :   // 0x8000-0xBFFF: second-to-last bank (NROM-256)
		prgSize - 1     // 0xC000-0xFFFF: last bank (or only bank)
	);
	uint32_t bankStart = 16 + hasTrainer*512 + bank*16384;
	uint32_t offsetInBank = addr&0x3FFF;
	return bankStart + offsetInBank;
}

int16_t readMemory(FILE* fp, uint16_t addr){
	uint32_t offset = getLastBankOffset(addr);

	fseek(fp, offset, SEEK_SET);

	// May return EOF
	return fgetc(fp);
}
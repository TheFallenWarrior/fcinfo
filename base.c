
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

// Return the file offset of the given CPU address, mapped to the last PRG bank
// This uses a heuristic that assumes the last 16 KiB of ROM are fixed at C000-FFFF,
//  which might not be true for all mappers.
uint32_t getLastBankOffset(uint16_t addr){
	return (addr-(32+16*(prgSize==1))*1024) +              // Subtract CPU memory base
	(16+hasTrainer*512+16*1024*(prgSize-(prgSize!=1)-1));  // Add ROM file offset base
}

int16_t readMemory(FILE* fp, uint16_t addr){
	uint32_t offset = getLastBankOffset(addr);

	fseek(fp, offset, SEEK_SET);

	// May return EOF
	return fgetc(fp);
}
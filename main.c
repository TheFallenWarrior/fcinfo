/*
	fcinfo
	Copyright 2025 TheFallenWarrior
	Licensed under MIT/Expat
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "names.h"

#define TILECMP(x, y) (!memcmp((x), (y), 16))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

// https://www.nesdev.org/wiki/INES
// https://www.nesdev.org/wiki/NES_2.0
uint8_t iNesHeader[16];

// https://www.nesdev.org/wiki/Nintendo_header
uint8_t officialHeader[26];

uint16_t vectors[3]; // HW vector addresses in CPU memory
int absVectors[3];   // HW vector addresses in ROM

uint8_t uniqueTilesBank[256][16];
int uniqueTileCounter[512];
int emptySpacePrg[256];

int prgSize;
int chrSize;
int mapper;
int hasTrainer;
int isNes2;

int hasOfficialHeader;
char gameTitle[16];

typedef enum options{
	OPT_VECTORS,
	OPT_SPACE,
	OPT_OFFICIAL,
	OPT_INES,
	OPT_ALL,
} options;

void printUsage(){
	printf(
		"Display information about an FC/NES ROM file\n"
		"Usage: fcinfo [option] ROM\n\n"
		"'option' is one of:\n"
		"\t-a\tShow all available information\n"
		"\t-H\tDisplay iNES/NES 2.0 header information (default)\n"
		"\t-o\tDisplay official header information if present\n"
		"\t-s\tDisplay free ROM space\n"
		"\t-v\tDisplay hardware vectors\n\n"
	);
}

void readINesHeader(FILE *rom){
	fread(iNesHeader, 16, 1, rom);
	if(memcmp(iNesHeader, "NES\x1a", 4)){
		fprintf(stderr, "This file isn't an NES ROM.\n");
		exit(1);
	}
	isNes2 = iNesHeader[7]&0x08 && iNesHeader[7]&~0x04;
	mapper = (iNesHeader[6]>>4) | (iNesHeader[7]&0xf0) | (isNes2 ? (iNesHeader[8]&0xf)<<8 : 0);
	hasTrainer = (iNesHeader[6]&0x04);
	prgSize = iNesHeader[4] | (isNes2 ? iNesHeader[9]&0x0f : 0);
	chrSize = iNesHeader[5] | (isNes2 ? iNesHeader[9]>>4 : 0);
}

void readOfficialHeader(FILE *rom){
	fseek(rom, hasTrainer*512+16*1024*prgSize-16, SEEK_SET);
	fread(officialHeader, 26, 1, rom);
	hasOfficialHeader =
		officialHeader[22] && officialHeader[22] < 3 &&
		officialHeader[23] && officialHeader[23] < 16;

	if(!(hasOfficialHeader)) return;
	memcpy(gameTitle, &officialHeader[15-officialHeader[23]], officialHeader[23]+1);
}

void readHWVectors(FILE *rom){
	fseek(rom, hasTrainer*512+16*1024*prgSize+10, SEEK_SET);
	fread(vectors, 2, 3, rom);

	for(int i=0;i<3;i++){
		absVectors[i] =
			(vectors[i]-(32+16*(prgSize==1))*1024) +
			(hasTrainer*512+16*1024*(prgSize-(prgSize!=1)-1)+16);
	}
}

void countEmptySpace(FILE *rom){
	if(prgSize > 255){
		fprintf(stderr, "Warning: PRG-ROM Size > 256 x 16 KiB is not supported.\n\n");
		prgSize = 255;
	}
	if(chrSize > 255){
		fprintf(stderr, "Warning: CHR-ROM Size > 256 x 8 KiB is not supported.\n\n");
		chrSize = 255;
	}
	// Count empty space in PRG-ROM
	int ch;
	fseek(rom, hasTrainer*512+16, SEEK_SET);
	for(int i=0;i<prgSize;i++){
		int tmp = 0;
		emptySpacePrg[i] = 0;
		for(int j=0; j<(16*1024) && (ch = fgetc(rom)) != EOF; j++){
			if (!ch){
				tmp++;
			} else{
				if(tmp > emptySpacePrg[i]) emptySpacePrg[i] = tmp;
				tmp = 0;
			}
		}
		if(tmp > emptySpacePrg[i]) emptySpacePrg[i] = tmp;
	}

	// Count unique tiles in each 4 KiB page in CHR-ROM
	for(int i =0;i<(chrSize*2);i++){
		memset(uniqueTilesBank, 0, 4096);
		for(int j=0;j<256;j++){
			uint8_t tmp[16];
			int uniqueTileFlag = 1;

			fread(tmp, 16, 1, rom);
			for(int k=0;k<uniqueTileCounter[i];k++){
				if(TILECMP(tmp, uniqueTilesBank[k])){
					uniqueTileFlag = 0;
					break;
				}
			}

			if(uniqueTileFlag){
				memcpy(uniqueTilesBank[uniqueTileCounter[i]], tmp, 16);
				uniqueTileCounter[i]++;
			}
		}
	}
}

void printINesHeaderInfo(){
	printf("%s header:\n", isNes2 ? "NES 2.0" : "iNES");
	for(int i=0;i<8;i++) printf(" %02x", iNesHeader[i]);
	printf(" ");
	if(isNes2){
		for(int i=8;i<16;i++) printf(" %02x", iNesHeader[i]);
	}

	printf("\n\n PRG-ROM size: %d KiB\n", prgSize*16);
	printf(" CHR-ROM size: %d KiB\n", chrSize*8);

	printf(" Mapper: %d\n", mapper);
	printf(" Battery-backed: %s\n", (iNesHeader[6]&0x02) ? "yes" : "no");
	printf(" Mirroring: %s\n", (iNesHeader[6]&0x08) ? "none" : (iNesHeader[6]&0x01) ? "vertical" : "horizontal");
	printf(" Trainer: %s\n", hasTrainer ? "yes" : "no");

	if(isNes2){
		printf(" System: %s\n\n", (iNesHeader[7]&0x3) == 3 ? systemNames[iNesHeader[13]] : systemNames[iNesHeader[7]&0x3]);
		printf(" Submapper: %d\n", iNesHeader[8]>>4);
		printf(" PRG-RAM size:   %d B\n", (iNesHeader[10]&0x0f) ? 64 << (iNesHeader[10]&0x0f) : 0);
		printf(" PRG-NVRAM size: %d B\n", (iNesHeader[10]>>4) ? 64 << (iNesHeader[10]>>4) : 0);
		printf(" CHR-RAM size:   %d B\n", (iNesHeader[11]&0x0f) ? 64 << (iNesHeader[11]&0x0f) : 0);
		printf(" CHR-NVRAM size: %d B\n\n", (iNesHeader[11]>>4) ? 64 << (iNesHeader[11]>>4) : 0);

		if((iNesHeader[7]&0x3) == 1){
			printf(" VS System Type: %s\n", vsSystemTypes[iNesHeader[13]>>4]);
			printf(" VS System PPU: %s\n", vsSystemPpuNames[iNesHeader[13]&0x0f]);
		}

		printf(" Frame timing: %s\n", regionNames[iNesHeader[12]]);
		printf(" Misc ROMs: %d\n", iNesHeader[14]);
		printf(" Input Device: %s\n\n", inputTypes[iNesHeader[15]]);
	} else printf(" System: %s\n\n", (iNesHeader[7]&0x3) < 3 ? systemNames[iNesHeader[7]&0x3] : "Other");
}

void printOfficialHeader(){
	printf("Official header:\n");

	if(!hasOfficialHeader){
		printf(" This ROM does not appear to have an official header.\n\n");
		return;
	}

	printf(" Title: %s\n", gameTitle);
	printf(" Title encoding: %s\n", officialHeader[22] == 1 ? "ASCII" : officialHeader[22] == 2 ? "JIS X 0201" : "none");
	printf(" Title length: %d B\n", officialHeader[23]+1);
	printf(" Licensee code: 0x%02x\n", officialHeader[24]);
	printf(" PRG-ROM checksum: 0x%02x%02x\n", officialHeader[16], officialHeader[17]);
	printf(" CHR-ROM checksum: 0x%02x%02x\n", officialHeader[18], officialHeader[19]);
	printf(" Complementary checksum: 0x%02x\n\n", officialHeader[24]);
	printf(" PRG-ROM size: %s KiB\n", officialPrgSizes[officialHeader[20]>>4]);
	printf(" CHR size:     %s KiB\n", officialChrSizes[officialHeader[20]&0x07]);
	printf(" CHR memory type: %s\n", (officialHeader[20]&0x08) ? "RAM" : "ROM");
	printf(" Mirroring: %s\n", officialHeader[21]&0x80 ? "vertical" : "horizontal");
	printf(" Mapper: %s\n\n", officialMapperNames[officialHeader[21]&0x07]);
}

int main(int argc, char *argv[]){
	if (argc < 2){
		printUsage();
		exit(1);
	}

	options opt = OPT_INES;
	FILE *rom;
	if(argv[1][0] == '-'){
		switch(argv[1][1]){
			case 'v':
			opt = OPT_VECTORS;
			break;

			case 's':
			opt = OPT_SPACE;
			break;

			case 'o':
			opt = OPT_OFFICIAL;
			break;

			case 'H':
			opt = OPT_INES;
			break;

			case 'a':
			opt = OPT_ALL;
			break;

			case 'h':
			printUsage();
			exit(0);

			default:
			printUsage();
			exit(1);
		}
		rom = fopen(argv[2], "rb");
	} else rom = fopen(argv[1], "rb");

	if(rom == NULL){
		perror("Error opening ROM");
		exit(1);
	}

	readINesHeader(rom);
	readOfficialHeader(rom);

	if(opt == OPT_INES || opt == OPT_ALL) printINesHeaderInfo();
	if((opt == OPT_ALL && hasOfficialHeader) || opt == OPT_OFFICIAL) printOfficialHeader();
	if(opt == OPT_VECTORS || opt == OPT_ALL){
		readHWVectors(rom);
		printf("Hardware vectors CPU address (ROM offset):\n");
		printf(" Vblank NMI:   0x%04x (0x%06x)\n", vectors[0], absVectors[0]);
		printf(" Entry point:  0x%04x (0x%06x)\n", vectors[1], absVectors[1]);
		printf(" External IRQ: 0x%04x (0x%06x)\n\n", vectors[2], absVectors[2]);
	}
	if(opt == OPT_SPACE || opt == OPT_ALL){
		countEmptySpace(rom);
		printf("ROM space:\n");
		for(int i=0;i<prgSize;i++)
			printf(" Free space in PRG-ROM bank %d: %d bytes\n", i, emptySpacePrg[i]);
		printf("\n");
		for(int i=0;i<(chrSize*2);i++)
			printf(" Free space in CHR-ROM page %d: %d tiles\n", i, 256-uniqueTileCounter[i]);
		printf("\n");
	}
	fclose(rom);

	exit(0);
}

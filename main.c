/*
	fcinfo
	Copyright 2024 TheFallenWarrior
	Licensed under MIT/Expat
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TILECMP(x, y) (!memcmp((x), (y), 16))
#define MIN(x, y) ((x) > (y) ? (y) : (x))

const char *const systemNames[] = {
	"NES/FC/Dendy",
	"VS System",
	"PlayChoice-10",
	"Famiclone with Decimal Mode",
	"NES/FC with EPSM module",
	"VT01 with STN palette",
	"VT02",
	"VT03",
	"VT09",
	"VT32",
	"VT369",
	"UMC UM6578",
	"Famicom Network System",
	"Reserved (0x0d)",
	"Reserved (0x0e)",
	"Reserved (0x0f)"
};

const char *const regionNames[] = {
	"RP2C02 (NTSC)",
	"RP2C07 (PAL)",
	"Dual-region",
	"UA6538 (Dendy)"
};

const char *const vsSystemPpuNames[] = {
	"RP2C03B",
	"RP2C03G",
	"RP2C04-0001",
	"RP2C04-0002",
	"RP2C04-0003",
	"RP2C04-0004",
	"RC2C03B",
	"RC2C03C",
	"RC2C05-01",
	"RC2C05-02",
	"RC2C05-03",
	"RC2C05-04",
	"RC2C05-05",
	"Reserved (0x0d)",
	"Reserved (0x0e)",
	"Reserved (0x0f)"
};

const char *const vsSystemTypes[] = {
	"VS Unisystem",
	"RBI Baseball",
	"TKO Boxing",
	"Super Xevious",
	"Ice Climber Japan",
	"VS Dualsystem",
	"Raid on the Bungeling Bay"
};

const char *const inputTypes[] = {
	"Unspecified",
	"Standard NES/Famicom controllers",
	"NES Four Score/Satellite",
	"Famicom Four Players",
	"VS System (1P via 0x4016)",
	"VS System (1P via 0x4017)",
	"Reserved",
	"VS Zapper",
	"Zapper (0x4017)",
	"Two Zappers",
	"Bandai Hyper Shot Lightgun",
	"Power Pad Side A",
	"Power Pad Side B",
	"Family Trainer Side A",
	"Family Trainer Side B",
	"Arkanoid Vaus Controller (NES)",
	"Arkanoid Vaus Controller (Famicom)",
	"Two Vaus Controllers plus Famicom Data Recorder",
	"Konami Hyper Shot Controller",
	"Coconuts Pachinko Controller",
	"Exciting Boxing Punching Bag",
	"Jissen Mahjong Controller",
	"Party Tap",
	"Oeka Kids Tablet",
	"Sunsoft Barcode Battler",
	"Miracle Piano Keyboard",
	"Pokkun Moguraa",
	"Top Rider",
	"Double-Fisted",
	"Famicom 3D System",
	"Doremikko Keyboard",
	"R.O.B. Gyro Set",
	"Famicom Data Recorder",
	"ASCII Turbo File",
	"IGS Storage Battle Box",
	"Family BASIC Keyboard plus Famicom Data Recorder",
	"Dongda PEC-586 Keyboard",
	"Bit Corp. Bit-79 Keyboard",
	"Subor Keyboard",
	"Subor Keyboard plus mouse (3x8-bit protocol)",
	"Subor Keyboard plus mouse (24-bit protocol via 0x4016)",
	"SNES Mouse (0x4017.d0)",
	"Multicart",
	"Two SNES controllers replacing the two standard NES controllers",
	"RacerMate Bicycle",
	"U-Force",
	"R.O.B. Stack-Up",
	"City Patrolman Lightgun",
	"Sharp C1 Cassette Interface",
	"Standard Controller with swapped Left-Right/Up-Down/B-A",
	"Excalibur Sudoku Pad",
	"ABL Pinball",
	"Golden Nugget Casino extra buttons",
	"Unknown famiclone keyboard used by the 'Golden Key' educational cartridge",
	"Subor Keyboard plus mouse (24-bit protocol via 0x4017)",
	"Port test controller",
	"Bandai Multi Game Player Gamepad buttons",
	"Venom TV Dance Mat",
	"LG TV Remote Control",
	"Famicom Network Controller",
	"King Fishing Controller",
	"Croaky Karaoke Controller"
};

const char *const officialPrgSizes[] = {
	"64",
	"16",
	"32",
	"128",
	"256",
	"512"
};

const char *const officialChrSizes[] = {
	"8",
	"16",
	"32",
	"64 or 128",
	"256"
};

const char *const officialMapperNames[] = {
	"NROM",
	"CNROM",
	"UNROM",
	"GNROM",
	"MMC"
};

// https://www.nesdev.org/wiki/INES
// https://www.nesdev.org/wiki/NES_2.0
uint8_t iNesHeader[16];

// https://www.nesdev.org/wiki/Nintendo_header
uint8_t officialHeader[26];

uint16_t vectors[3];

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
		"Display information about a FC/NES ROM file\n"
		"Usage: fcinfo [option] rom\n\n"
		"'option' is one of:\n"
		"\t-a\tShow all available information\n"
		"\t-H\tDisplay iNES/NES 2.0 header information [default]\n"
		"\t-o\tDisplay official header information (if present)\n"
		"\t-s\tDisplay free ROM space\n"
		"\t-V\tDisplay hardware vectors\n\n"
	);
}

void readINesHeader(FILE *rom){
	fread(iNesHeader, 16, 1, rom);
	if(memcmp(iNesHeader, "NES\x1a", 4)){
		fprintf(stderr, "File provided isn't an NES rom.\n");
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
}

void countEmptySpace(FILE *rom){
	if(prgSize > 255){
		fprintf(stderr, "Warning: PRG ROM Size > 256 x 16 KiB is not supported.\n");
		prgSize = MIN(prgSize, 255);
	}
	if(chrSize > 255){
		fprintf(stderr, "Warning: CHR ROM Size > 256 x 8 KiB is not supported.\n");
		chrSize = MIN(chrSize, 255);
	}
	// Count empty space in PRG ROM
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
	
	// Count unique tiles in each bank in CHR ROM
	for(int i =0;i<(chrSize*2);i++){
		memset(uniqueTilesBank, 0, 4096);
		//uniqueTileCounter[i] = 1;
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
	if(!isNes2){
		printf("iNES header:\n");
		for(int i=0;i<8;i++) printf(" %02x", iNesHeader[i]);
	} else{
		printf("NES 2.0 header:\n");
		for(int i=0;i<16;i++) printf(" %02x", iNesHeader[i]);
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
			case 'V':
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
        perror("Error opening rom.\n");
        exit(1);
    }
	
	readINesHeader(rom);
	readOfficialHeader(rom);
	
	if(opt == OPT_INES || opt == OPT_ALL) printINesHeaderInfo();
	if((opt == OPT_ALL && hasOfficialHeader) || opt == OPT_OFFICIAL) printOfficialHeader();
	if(opt == OPT_VECTORS || opt == OPT_ALL){
		readHWVectors(rom);
		printf("Hardware vectors (CPU address):\n");
		printf(" Vblank NMI:   0x%04x\n", vectors[0]);
		printf(" Entry point:  0x%04x\n", vectors[1]);
		printf(" External IRQ: 0x%04x\n\n", vectors[2]);
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
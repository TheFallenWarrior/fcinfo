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

// https://www.nesdev.org/wiki/INES
// https://www.nesdev.org/wiki/NES_2.0
uint8_t header[16];

uint16_t vectors[3];

uint8_t uniqueTilesBank[256][16];
int uniqueTileCounter[512];
int emptySpacePrg[256];

int prgSize;
int chrSize;
int mapper;
int hasTrainer;
int isNes2;

typedef enum options{
	OPT_VECTORS,
	OPT_SPACE,
	OPT_INES,
	OPT_ALL,
} options;

void printUsage(){
	printf(
		"Display information about a FC/NES rom file\n"
		"Usage: fcinfo [option] rom\n\n"
		"'option' is one of:\n"
		"\t-a\tShow all information\n"
		"\t-H\tDisplay iNES/NES 2.0 header information [default]\n"
		"\t-s\tDisplay free ROM space\n"
		"\t-V\tDisplay hardware vectors\n"
	);
}

void readINesHader(FILE *rom){
	fread(header, 16, 1, rom);
	if(memcmp(header, "NES\x1a", 4)){
		fprintf(stderr, "File provided isn't an NES rom.\n");
		exit(1);
	}
	isNes2 = header[7]&0x08 && header[7]&~0x04;
	mapper = (header[6]>>4) | (header[7]&0xf0) | (isNes2 ? (header[8]&0xf)<<8 : 0);
	hasTrainer = (header[6]&0x04);
	prgSize = header[4] | (isNes2 ? header[9]&0x0f : 0);
	chrSize = header[5] | (isNes2 ? header[9]>>4 : 0);
}

void readHWVectors(FILE *rom){
	fseek(rom, hasTrainer*512+16*1024*prgSize+10, SEEK_SET);
	fread(vectors, 2, 3, rom);
}

void countEmptySpace(FILE *rom){
	if(prgSize != MIN(prgSize, 255)){
		fprintf(stderr, "Warning: PRG ROM Size > 256 x 16 KiB is not supported.");
		prgSize = MIN(prgSize, 255);
	}
	if(chrSize != MIN(chrSize, 255)){
		fprintf(stderr, "Warning: CHR ROM Size > 256 x 8 KiB is not supported.");
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
		for(int i=0;i<8;i++) printf(" %02x", header[i]);
	} else{
		printf("NES 2.0 header:\n");
		for(int i=0;i<16;i++) printf(" %02x", header[i]);
	}
	
	printf("\n\n PRG ROM size: %d KiB\n", prgSize*16);
	printf(" CHR ROM size: %d KiB\n", chrSize*8);

	printf(" Mapper: %d\n", mapper);
	printf(" Battery-backed: %s\n", (header[6]&0x02) ? "yes" : "no");
	printf(" Mirroring: %s\n", (header[6]&0x08) ? "none" : (header[6]&0x01) ? "vertical" : "horizontal");
	printf(" Trainer: %s\n", hasTrainer ? "yes" : "no");

	if(isNes2){
		printf(" System: %s\n\n", (header[7]&0x3) == 3 ? systemNames[header[13]] : systemNames[header[7]&0x3]);
		printf(" Submapper: %d\n", header[8]>>4);
		printf(" PRG RAM size:   %d B\n", (header[10]&0x0f) ? 64 << (header[10]&0x0f) : 0);
		printf(" PRG NVRAM size: %d B\n", (header[10]>>4) ? 64 << (header[10]>>4) : 0);
		printf(" CHR RAM size:   %d B\n", (header[11]&0x0f) ? 64 << (header[11]&0x0f) : 0);
		printf(" CHR NVRAM size: %d B\n\n", (header[11]>>4) ? 64 << (header[11]>>4) : 0);

		if((header[7]&0x3) == 1){
			printf(" VS System Type: %s\n", vsSystemTypes[header[13]>>4]);
			printf(" VS System PPU: %s\n", vsSystemPpuNames[header[13]&0x0f]);
		}

		printf(" Frame timing: %s\n", regionNames[header[12]]);
		printf(" Misc ROMs: %d\n", header[14]);
		printf(" Input Device: 0x%02x\n\n", header[15]);
	} else printf(" System: %s\n\n", (header[7]&0x3) < 3 ? systemNames[header[7]&0x3] : "Other");
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
	
	readINesHader(rom);
	
	if(opt == OPT_INES || opt == OPT_ALL) printINesHeaderInfo();
	if(opt == OPT_VECTORS || opt == OPT_ALL){
		readHWVectors(rom);
		printf("Hardware vectors:\n");
		printf(" NMI:          0x%04x\n", vectors[0]);
		printf(" Entry point:  0x%04x\n", vectors[1]);
		printf(" External IRQ: 0x%04x\n\n", vectors[2]);
	}
	if(opt == OPT_SPACE || opt == OPT_ALL){
		countEmptySpace(rom);
		printf("ROM space:\n");
		for(int i=0;i<prgSize;i++)
			printf(" Free space in PRG ROM bank %d: %d bytes\n", i, emptySpacePrg[i]);
		printf("\n");
		for(int i=0;i<(chrSize*2);i++)
			printf(" Free space in CHR ROM bank %d: %d tiles\n", i, 256-uniqueTileCounter[i]);
		printf("\n");
	}
    fclose(rom);

    exit(0);
}
#ifndef FC_MAPPER_H
#define FC_MAPPER_H

#include <stdint.h>

extern uint8_t iNesHeader[16];
extern uint8_t officialHeader[26];
extern uint16_t vectors[3];
extern int absVectors[3];
extern uint8_t uniqueTilesBuf[256][16];
extern int *uniqueTileCounter;
extern int *emptySpacePrg;
extern int64_t prgSize;
extern int64_t chrSize;
extern int mapper;
extern int hasTrainer;
extern int isNes2;
extern int hasOfficialHeader;
extern char gameTitle[16];

uint32_t getLastBankOffset(uint16_t addr);

#endif
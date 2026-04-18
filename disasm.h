/*
	fcinfo
	Copyright 2026 TheFallenWarrior
	Licensed under MIT/Expat
*/

#ifndef FC_DISASM_H
#define FC_DISASM_H

#include <stdint.h>
#include <string.h>

#include "instructions.h"

uint16_t disassemble(FILE *fp, uint16_t addr, char *out, uint16_t n);

#endif
/*
	fcinfo
	Copyright 2026 TheFallenWarrior
	Licensed under MIT/Expat
*/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "base.h"
#include "disasm.h"
#include "instructions.h"

// Get disassembly of instruction at addr;
// Return address of the next instruction
uint16_t disassemble(FILE *fp, uint16_t addr, char *out, uint16_t n){
	Opcode   op;
	uint8_t  ins, param8  = 0;
	uint16_t param16 = 0;
	uint16_t nextAddr;
	char pbuf[10]; // Instruction parameters
	char bbuf[11]; // Instruction hex dump

	ins = readMemory(fp, addr);
	op = opcodes[ins];
	nextAddr = addr + instruction_length[op.addr_mode];

	switch(instruction_length[op.addr_mode]){
		case 1:
		snprintf(bbuf, 11, "; %02X", ins);
		break;

		case 2:
		param8  = readMemory(fp, 1 + addr);
		snprintf(bbuf, 11, "; %02X %02X", ins, param8);
		break;
		
		case 3:
		param16 = readMemory(fp, 1 + addr) | readMemory(fp, 2 + addr) << 8;
		snprintf(bbuf, 11, "; %02X %02X %02X", ins, param16&0xff, param16>>8);
		break;

		default:;
	}

	switch(op.addr_mode){
		case AM_IMPLIED:
		snprintf(pbuf, 10, " ");
		break;

		case AM_ACCUMULATOR:
		snprintf(pbuf, 10, "a");
		break;
		
		case AM_IMMEDIATE:
		snprintf(pbuf, 10, "#$%02X", param8);
		break;
		
		case AM_ABSOLUTE:
		if(op.instr == INS_JMP || op.instr == INS_JSR) // Treat the address as a label
			snprintf(pbuf, 10, "L%04X", param16);
		else
			snprintf(pbuf, 10, "$%04X", param16);
		break;
		
		case AM_ZEROPAGE:
		snprintf(pbuf, 10, "$%02X", param8);
		break;
		
		case AM_INDEXED_ZEROPAGE_X:
		snprintf(pbuf, 10, "$%02X,x", param8);
		break;
		
		case AM_INDEXED_ZEROPAGE_Y:
		snprintf(pbuf, 10, "$%02X,y", param8);
		break;
		
		case AM_INDEXED_ABSOLUTE_X:
		snprintf(pbuf, 10, "$%04X,x", param16);
		break;
		
		case AM_INDEXED_ABSOLUTE_Y:
		snprintf(pbuf, 10, "$%04X,y", param16);
		break;
		
		case AM_RELATIVE:
		snprintf(pbuf, 10, "L%04X", relative_addr(nextAddr, (int8_t)param8));
		break;
		
		case AM_INDEXED_INDIRECT_X:
		snprintf(pbuf, 10, "($%02X,x)", param8);
		break;
		
		case AM_INDIRECT_INDEXED_Y:
		snprintf(pbuf, 10, "($%02X),y", param8);
		break;
		
		case AM_ABSOLUTE_INDIRECT:
		snprintf(pbuf, 10, "($%04X)", param16);
		break;
	}

	snprintf(out, n, "L%04X:\t%s\t%s\t%s", addr, mnemonics[op.instr], pbuf, bbuf);

	return nextAddr;
}
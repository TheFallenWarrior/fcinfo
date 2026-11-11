/*
	fcinfo
	Copyright 2026 TheFallenWarrior
	Licensed under MIT/Expat
*/

#ifndef FC_INSTRUCTIONS_H
#define FC_INSTRUCTIONS_H

#include <stdint.h>

enum _addressing_modes{
	AM_ACCUMULATOR,
	AM_IMMEDIATE,
	AM_ABSOLUTE,
	AM_ZEROPAGE,
	AM_INDEXED_ZEROPAGE_X,
	AM_INDEXED_ZEROPAGE_Y,
	AM_INDEXED_ABSOLUTE_X,
	AM_INDEXED_ABSOLUTE_Y,
	AM_IMPLIED,
	AM_RELATIVE,
	AM_INDEXED_INDIRECT_X,
	AM_INDIRECT_INDEXED_Y,
	AM_ABSOLUTE_INDIRECT,
	AM_BREAK
};

enum _instructions{
	INS_JAM, // Crash CPU

	// Documented instuctions
	INS_ADC,
	INS_AND,
	INS_ASL,
	INS_BCC,
	INS_BCS,
	INS_BEQ,
	INS_BIT,
	INS_BMI,
	INS_BNE,
	INS_BPL,
	INS_BRK,
	INS_BVC,
	INS_BVS,
	INS_CLC,
	INS_CLD,
	INS_CLI,
	INS_CLV,
	INS_CMP,
	INS_CPX,
	INS_CPY,
	INS_DEC,
	INS_DEX,
	INS_DEY,
	INS_EOR,
	INS_INC,
	INS_INX,
	INS_INY,
	INS_JMP,
	INS_JSR,
	INS_LDA,
	INS_LDX,
	INS_LDY,
	INS_LSR,
	INS_NOP,
	INS_ORA,
	INS_PHA,
	INS_PHP,
	INS_PLA,
	INS_PLP,
	INS_ROL,
	INS_ROR,
	INS_RTI,
	INS_RTS,
	INS_SBC,
	INS_SEC,
	INS_SED,
	INS_SEI,
	INS_STA,
	INS_STX,
	INS_STY,
	INS_TAX,
	INS_TAY,
	INS_TSX,
	INS_TXA,
	INS_TXS,
	INS_TYA,

	// Illegal instructions
	INS_ANE, // OR A with unstable, AND X with immediate
	INS_ANC, // AND A with immediate, copy N into C
	INS_ARR, // AND A with immediate, rotate result right
	INS_ASR, // AND A with immediate, shift result right
	INS_DCP, // Decrement and compare result
	INS_ISC, // Increment memory, SBC from A
	INS_LAS, // AND S with memory, store result into A, X, and S
	INS_LAX, // Load A and X with memory
	INS_LXA, // OR A with unstable, AND A with immediate, store result into A and X
	INS_RLA, // Rotate left, AND result with A
	INS_RRA, // Rotate right and add with carry
	INS_SAX, // AND A with X, store result into memory
	INS_SBX, // AND A with X, SBC immediate
	INS_SHA, // AND A with X with MSB(ADDR)+1, store result into memory
	INS_SHX, // AND X with MSB(ADDR)+1, store result into memory
	INS_SHY, // AND Y with MSB(ADDR)+1, store result into memory
	INS_SLO, // Shift left, OR result with A
	INS_SRE, // Shift right, XOR result with A
	INS_TAS, // AND A with X, store result into S, AND S with MSB(ADDR)+1, store result into memory 
};

typedef struct{
	uint8_t instr;
	uint8_t addr_mode;
} Opcode;

extern const uint8_t instruction_length[];
extern const Opcode opcodes[];
extern const char *const mnemonics[];


uint16_t relative_addr(uint16_t addr, int8_t offset);

#endif
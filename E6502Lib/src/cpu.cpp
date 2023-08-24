// 6502Emulator.cpp : Defines the entry point for the application.
// 
// Emulator built following tutorial here: https://www.youtube.com/watch?v=qJgsuQoy9bc
// 6502 CPU Reference: https://www.obelisk.me.uk/6502/
//
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"

/* Resets the CPU state - Until this is called, CPU state is undefined */
void CPU::reset(Memory* memory) {
	D = 0;					// Clear decimal flag
	I = 0;					// Clear interrupt flag
	PC = 0xFFFC;			// Set Program Counter
	SP = 0x100;				// Set Stack Pointer
	A = X = Y = 0;			// Reset registers
	memory->initialise();	// Reset Memory
}

/* Helper method, allows setting all flags at once */
void CPU::setFlags(u8 flags) {
	C = (flags & 0b00000001) >> 0;
	Z = (flags & 0b00000010) >> 1;
	I = (flags & 0b00000100) >> 2;
	D = (flags & 0b00001000) >> 3;
	B = (flags & 0b00010000) >> 4;
	O = (flags & 0b01000000) >> 6;
	N = (flags & 0b10000000) >> 7;
}

/* Allows getting all flags in a single byte */
u8 CPU::getFlags() {
	return ((C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (O << 6) | (N << 7));
}

/*
* Execute <numInstructions> instructions. Return the number of cycles used.
*/
u8 CPU::execute(u8 numInstructions, Memory* memory) {
	u8 cyclesUsed = 0;
	while (numInstructions > 0) {
		//Get the next instruction and increment PC
		Byte instruction = memory->readByte(cyclesUsed, PC);
		PC++;
		cyclesUsed++;

		//Get the handler for this instruction
		const InstructionHandler* handler = insManager[instruction];
		if (!handler->isLegal) {
			printf("Executing illegal opcode 0x%02X\n", instruction);
		}
		cyclesUsed += handler->execute(memory);
		numInstructions--;

	}
	return cyclesUsed;
}


// 6502Emulator.cpp : Defines the entry point for the application.
// 
// Emulator built following tutorial here: https://www.youtube.com/watch?v=qJgsuQoy9bc
// 6502 CPU Reference: https://www.obelisk.me.uk/6502/
//
#pragma once
#include <stdio.h>
#include "cpu.h"

/* Initialises CPU objects */
CPU::CPU(CPUState* initState) {
	insManager = new InstructionManager(&InstructionUtils::loader);
	currentState = initState;
}

/* Resets the CPU state - Until this is called, CPU state is undefined */
void CPU::reset(Memory* memory) {
	currentState->D = 0;											// Clear decimal flag
	currentState->I = 0;											// Clear interrupt flag
	currentState->PC = 0xFFFC;										// Set Program Counter
	currentState->SP = 0x100;										// Set Stack Pointer
	currentState->A = currentState->X = currentState->Y = 0;		// Reset registers
	memory->initialise();	// Reset Memory
}

/* Helper method, allows setting all flags at once */
void CPU::setFlags(u8 flags) {
	currentState->C = (flags & 0b00000001) >> 0;
	currentState->Z = (flags & 0b00000010) >> 1;
	currentState->I = (flags & 0b00000100) >> 2;
	currentState->D = (flags & 0b00001000) >> 3;
	currentState->B = (flags & 0b00010000) >> 4;
	currentState->O = (flags & 0b01000000) >> 6;
	currentState->N = (flags & 0b10000000) >> 7;
}

/* Allows getting all flags in a single byte */
u8 CPU::getFlags() {
	return (	(currentState->C << 0) | (currentState->Z << 1) | (currentState->I << 2) | 
				(currentState->D << 3) | (currentState->B << 4) | (currentState->O << 6) | (currentState->N << 7));
}

/*
* Execute <numInstructions> instructions. Return the number of cycles used.
*/
u8 CPU::execute(u8 numInstructions, Memory* memory) {
	u8 cyclesUsed = 0;
	while (numInstructions > 0) {
		//Get the next instruction and increment PC
		Byte code = memory->data[currentState->PC];
		currentState->PC++;
		InstructionCode instruction(code);


		//Get the handler for this instruction
		const InstructionHandler* handler = (*insManager)[instruction];
		if (!handler->isLegal) {
			fprintf(stderr, "Executing illegal opcode 0x%02X\n", instruction.code);
		}
		cyclesUsed += handler->execute(memory, currentState, &instruction);
		numInstructions--;
	}
	return cyclesUsed;
}

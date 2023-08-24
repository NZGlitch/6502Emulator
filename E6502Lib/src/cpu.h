// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "types.h"
#include "memory.h"
#include "instruction_manager.h"

class CPU {
private:
	InstructionManager insManager;

public:
	// Internal Registers
	Word PC;			// Program Counter
	Word SP;			// Stack Pointer

	// Registers
	Byte A, X, Y;

	// Status Flags
	Byte C : 1;			// Carry Flag				(0)
	Byte Z : 1;			// Zero Flag				(1)
	Byte I : 1;			// Interrupt Disable Flag	(2)
	Byte D : 1;			// Decimal Mode Flag		(3)
	Byte B : 1;			// Break Flag				(4)
	Byte O : 1;			// Overflow Flag			(6)
	Byte N : 1;			// Negative Flag			(7)

	/* Resets the CPU state - Until this is called, CPU state is undefined */
	void reset(Memory* memory);

	
	/* Helper method, allows setting all flags at once */
	void setFlags(u8 flags);

	/* Allows getting all flags in a single byte */
	u8 getFlags();

	/*
	 * Execute <numInstructions> instructions. Return the number of cycles used.
	 *
	 */
	u8 execute(u8 numInstructions, Memory* memory);

};

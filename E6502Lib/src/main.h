// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <stdio.h>
#include <stdlib.h>

// Data types used by the CPU
using Byte = unsigned char;
using Word = unsigned short;

using u8 = unsigned char;
using s8 = signed char;

using u16 = unsigned short;
using s16 = signed short;

// Op Codes
static constexpr Byte INS_NOP = 0xEA;


// System Memory
struct Mem {
	static constexpr Word MAX_MEM = 0xFFFF;	// Maximum addressable meory

	Byte data[MAX_MEM] = {};	//Actual data

	/* Reset memory to all 0's */
	virtual void initialise() {
		for (Word i = 0; i < MAX_MEM; i++)
			data[i] = 0x00;
	}

	Byte readByte(u8& cycles, Word address) {
		cycles++;
		return data[address];
	}

	void writeByte(u8& cycles, Word address, Byte value) {
		data[address] = value;
		cycles++;
	}
};

// CPU State
struct CPU {

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
	void reset(Mem& memory) {
		D = 0;					// Clear decimal flag
		I = 0;					// Clear interrupt flag
		PC = 0xFFFC;			// Set Program Counter
		SP = 0x100;				// Set Stack Pointer
		A = X = Y = 0;			// Reset registers
		memory.initialise();	// Reset Memory
	}

	/* Helper method, allows setting all flags at once */
	void setFlags(u8 flags) {
		C = (flags & 0b00000001) >> 0;
		Z = (flags & 0b00000010) >> 1;
		I = (flags & 0b00000100) >> 2;
		D = (flags & 0b00001000) >> 3;
		B = (flags & 0b00010000) >> 4;
		O = (flags & 0b01000000) >> 6;
		N = (flags & 0b10000000) >> 7;
	}

	/* Allows getting all flags in a single byte */
	u8 getFlags() {
		return ((C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (O << 6) | (N << 7));
	}

	/* 
	 * Execute <numInstructions> instructions. Return the number of cycles used.
	 * 
	 */
	u8 execute(u8 numInstructions, Mem& memory) {
		u8 cyclesUsed = 0;
		while (numInstructions > 0) {
			//Get the next instruction and increment PC
			Byte instruction = memory.readByte(cyclesUsed, PC);
			PC++;
			cyclesUsed++;

			switch (instruction) {
				case INS_NOP: {
					// NOP Instruction does nothing
					numInstructions--;
				}
			}
		}
		return cyclesUsed;
	}

};

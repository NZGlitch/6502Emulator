/** Defines all the main types used by the emulator */
#pragma once

// Data types used by the CPU
using Byte = unsigned char;
using Word = unsigned short;

using u8 = unsigned char;
using s8 = signed char;

using u16 = unsigned short;
using s16 = signed short;

struct CPUState {
	// Internal Registers
	Word PC;			// Program Counter
	Word SP;			// Stack Pointer

	// Registers
	Byte A,X,Y;

	// Status Flags
	Byte C : 1;			// Carry Flag				(0)
	Byte Z : 1;			// Zero Flag				(1)
	Byte I : 1;			// Interrupt Disable Flag	(2)
	Byte D : 1;			// Decimal Mode Flag		(3)
	Byte B : 1;			// Break Flag				(4)
	Byte O : 1;			// Overflow Flag			(6)
	Byte N : 1;			// Negative Flag			(7)	

	bool operator==(CPUState other) const {
		return (
			PC == other.PC &&
			SP == other.SP &&
			A == other.A &&
			X == other.X &&
			Y == other.Y &&
			C == other.C &&
			Z == other.Z &&
			I == other.I &&
			D == other.D &&
			B == other.B &&
			O == other.O &&
			N == other.N
			);
	}
};

// System Memory
struct Memory {
	static constexpr int MAX_MEM = 0x10000;	// Maximum addressable meory

	Byte data[MAX_MEM] = {};	//Actual data

	/* Reset memory to all 0's */
	virtual void initialise() {
		for (int i = 0; i < MAX_MEM; i++)
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

typedef int (*insHandlerFn)(Memory*, CPUState*);

struct InstructionHandler {
	Byte opcode;
	bool isLegal;
	char* name;
	insHandlerFn execute;
};

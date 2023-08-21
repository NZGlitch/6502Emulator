// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include <stdio.h>
#include <stdlib.h>

struct Mem {

};

struct CPU {
	using Byte = unsigned char;
	using Word = unsigned short;

	// Internal Registers
	Word PC;			// Program Counter
	Word SP;			// Stack Pointer

	// Registers
	Byte A, X, Y;

	// Status Flags
	Byte C : 1;			// Carry Flag
	Byte Z : 1;			// Zero Flag
	Byte I : 1;			// Interrupt Disable Flag
	Byte D : 1;			// Decimal Mode Flag
	Byte B : 1;			// Break Flag
	Byte O : 1;			// Overflow Flag
	Byte N : 1;			// Negative Flag

	void reset() {
		D = 0;			// Clear decimal flag
		I = 0;			// Clear interrupt flag
		PC = 0xFFFC;	// Set Program Counter
		SP = 0x100;		// Set Stack Pointer
		A = X = Y = 0;	// Reset registers
	}


};

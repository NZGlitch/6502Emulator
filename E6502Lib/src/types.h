/** Defines all the main types used by the emulator */
#pragma once

// Data types used by the CPU
using Byte = unsigned char;
using Word = unsigned short;

using u8 = unsigned char;
using s8 = signed char;

using u16 = unsigned short;
using s16 = signed short;

struct InstructionCode {

protected:
	/* Sets IC values from given Byte */
	void set(Byte b) {
		this->code = b;
		this->A = (b >> 5) & 0x7;
		this->B = (b >> 2) & 0x7;
		this->C = b & 0x3;
	}

public:
	Byte code=0;
	Byte A : 3;
	Byte B : 3;
	Byte C : 2;

	InstructionCode(Byte code) {
		this->set(code);
	}

	operator Byte() {
		return (this->A << 5 | this->B << 2 | this->C);
	}

	InstructionCode& operator=(Byte code) {
		this->set(code);
		return *this;
	}
};

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

	/* Returns the current value of the PC and increments it by 1 */
	Word incPC(u8& cycles) {
		cycles++;
		return PC++;
	}

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

	void setFlags(Byte flags) {
		C = (flags & (1 << 0)) > 0;
		Z = (flags & (1 << 1)) > 0;
		I = (flags & (1 << 2)) > 0;
		D = (flags & (1 << 3)) > 0;
		B = (flags & (1 << 4)) > 0;
		O = (flags & (1 << 6)) > 0;
		N = (flags & (1 << 7)) > 0;
	}

	Byte getFlags() {
		return (C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (O << 6) | (N << 7);
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

	/** Emulate reading a byte from memory. @cycles will be incremented by the number of cycles taken */
	Byte readByte(u8& cycles, Word address) {
		cycles++;
		return data[address];
	}

	/** Emulate writing a byte to memory. @cycles will be incremented by the number of cycles taken */
	void writeByte(u8& cycles, Word address, Byte value) {
		data[address] = value;
		cycles++;
	}

	/**
	* Load a program into memory at the given address.
	* Notes: memory will wrap around after 0xFFFF and no size check will be done on prgram array
	*/
	void loadProgram(Word address, Byte program[], u16 programSize) {
		for (u16 i = 0; i < programSize; i++) {
			Word nextAddr = address + i;
			data[nextAddr] = program[i];
		}
	}

};

typedef u8 (*insHandlerFn)(Memory*, CPUState*, InstructionCode*);

struct InstructionHandler {
	Byte opcode;
	bool isLegal;
	const char* name;
	insHandlerFn execute;
};

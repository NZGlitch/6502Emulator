#pragma once
#include <stdio.h>

namespace E6502 {
	// Data types used by the CPU
	using Byte = unsigned char;
	using Word = unsigned short;

	using u8 = unsigned char;
	using s8 = signed char;

	using u16 = unsigned short;
	using s16 = signed short;

	struct InstructionCode {

	protected:
		/* Sets InstructionCode values from given Byte */
		void set(Byte b) {
			this->code = b;
			this->A = (b >> 5) & 0x7;
			this->B = (b >> 2) & 0x7;
			this->C = b & 0x3;
		}

	public:
		Byte code = 0;
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
	private:
		Word SP = 0x01FF;					// Stack Pointer - need to ensure it remains on page 01

	public:
		CPUState() { setFlags(0x00); }		// Initialise flags to 0b00000000

		/** Identifiers for the registers are sometimes useful */
		const static u8 REGISTER_A = 0;
		const static u8 REGISTER_X = 1;
		const static u8 REGISTER_Y = 2;

		// Internal Registers
		Word PC = 0xFFFC;					// Program Counter

		// Registers
		Byte A = 0;
		Byte X = 0;
		Byte Y = 0;

		// Status Flags
		Byte C : 1;			// Carry Flag				(0)
		Byte Z : 1;			// Zero Flag				(1)
		Byte I : 1;			// Interrupt Disable Flag	(2)
		Byte D : 1;			// Decimal Mode Flag		(3)
		Byte B : 1;			// Break Flag				(4)
		Byte O : 1;			// Overflow Flag			(6)
		Byte N : 1;			// Negative Flag			(7)	

		/* Returns the current value of the PC and increments it by 1 */
		Word incPC() {
			return PC++;
		}

		/* Returns the value of the current SP then decrements */
		Word pushSP() {
			Word res = SP--;
			if (SP < 0x0100) SP = 0x01FF;
			return res;
		}

		/* increments the stack, then returns its new value */
		Word popSP() {
			if (++SP > 0x01FF) SP = 0x0100;
			Word res = SP;
			return res;
		}

		/* Returns current SP value without decrementing */
		Word getSP() {
			return SP;
		}

		/* Sets low order bits of SP */
		void setSP(Byte value) {
			SP = 0x0100 | value;
		}

		bool operator==(CPUState other) const {
			return (
				PC == other.PC && SP == other.SP &&
				A == other.A && X == other.X && Y == other.Y &&
				C == other.C && Z == other.Z && I == other.I &&
				D == other.D && B == other.B && O == other.O &&
				N == other.N);
		}

		/* Set the status from the provided Byte - bit 5 is ignored */
		void setFlags(Byte flags) {
			C = (flags & (1 << 0)) > 0;
			Z = (flags & (1 << 1)) > 0;
			I = (flags & (1 << 2)) > 0;
			D = (flags & (1 << 3)) > 0;
			B = (flags & (1 << 4)) > 0;
			O = (flags & (1 << 6)) > 0;
			N = (flags & (1 << 7)) > 0;
		}

		/* Get the status flags as a Byte - bit 5 is always 0 */
		Byte getFlags() {
			return (C << 0) | (Z << 1) | (I << 2) | (D << 3) | (B << 4) | (O << 6) | (N << 7);
		}

		void saveToReg(u8 REGISTER, Byte value) {
			switch (REGISTER) {
				case REGISTER_A: A = value; break;
				case REGISTER_X: X = value; break;
				case REGISTER_Y: Y = value; break;
				default: {
					fprintf(stderr, "Attempt to call CPUState->saveToReg with invalid register");
					break;
				}
			}
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

		/**
		 * Emulate reading a word from memory.
		 * @param cycles will be incremented by the number of cycles taken (2)
		 * NOTE: reading is little endian, i.e. if address = 0x1234, result[lsb] = 0x1234 and result[msb] = 0x1235
		 */
		Word readWord(u8& cycles, Word address) {
			Byte lsb = data[address];
			cycles++;
			Word result = (data[(address + 1) & 0xFFFF] << 8) | lsb;
			cycles++;
			return result;
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

	/* A function that can handle execution of a single instruction */
	typedef u8(*insHandlerFn)(Memory*, CPUState*, InstructionCode*);

	struct InstructionHandler {
		Byte opcode;
		bool isLegal;
		const char* name;
		insHandlerFn execute;
	};
}

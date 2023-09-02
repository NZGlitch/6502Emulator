#pragma once
#include <stdio.h>

// TODO -> Move InstructionHandler, inHandlrFn, Memory and CPUState into new files
namespace E6502 {
	// Data types used by the CPU
	using Byte = unsigned char;
	using Word = unsigned short;

	using u8 = unsigned char;
	using s8 = signed char;

	using u16 = unsigned short;
	using s16 = signed short;

	struct CPUState {
	private:
		Word SP = 0x01FF;					// Stack Pointer - need to ensure it remains on page 01

	public:
		CPUState() { setFlags(0x00); }		// Initialise flags to 0b00000000

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

		/** Saves the given value to the target register address and sets THIS states Z and N flags based on the value */
		virtual void saveToRegAndFlag(Byte* reg, Byte value) {
			(*reg) = value;
			Z = (*reg) == 0x00;
			N = ((*reg) & 0x80) > 0;
		}

		/** Resets all fields in this state back to 0, except SP which is initialised to 0x01FF */
		void reset() {
			A = X = Y = 0;
			PC = 0;
			SP = 0x1FF;
			setFlags(0x00);
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
}

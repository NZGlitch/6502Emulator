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

	public:
		static constexpr Word DEFAULT_RESET_VECTOR = 0xFFFC;
		static constexpr Byte DEFAULT_SP = 0xFF;

		CPUState() { setFlags(0x00); }		// Initialise flags to 0b00000000

		// Internal Registers
		Word PC = 0xFFFC;	// Program Counter
		Byte SP;			// Stack Pointer

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

		/** Resets all fields in this state back to 0, SP init to 0xFF, PC init to DEFAULT_RESET_VECTOR */
		virtual void reset() {
			A = X = Y = 0;
			PC = DEFAULT_RESET_VECTOR;
			SP = DEFAULT_SP;
			setFlags(0x00);
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

		bool operator==(CPUState other) const {
			return (
				PC == other.PC && SP == other.SP &&
				A == other.A && X == other.X && Y == other.Y &&
				C == other.C && Z == other.Z && I == other.I &&
				D == other.D && B == other.B && O == other.O &&
				N == other.N);
		}
	};
}

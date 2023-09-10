#pragma once
#include <stdio.h>
#include <vector>

// TODO -> Move InstructionHandler, inHandlrFn, Memory and CPUState into new files
namespace E6502 {
	// Data types used by the CPU
	using Byte = unsigned char;
	using Word = unsigned short;

	using u8 = unsigned char;
	using s8 = signed char;

	using u16 = unsigned short;
	using s16 = signed short;

	/* Bitwise flag register */
	struct StatusFlags {
		// Status Flags
		Byte C : 1;			// Carry Flag				(0)
		Byte Z : 1;			// Zero Flag				(1)
		Byte I : 1;			// Interrupt Disable Flag	(2)
		Byte D : 1;			// Decimal Mode Flag		(3)
		Byte B : 1;			// Break Flag				(4)
		Byte Unused : 1;	// Unused					(5)
		Byte V : 1;			// Overflow Flag			(6)
		Byte N : 1;			// Negative Flag			(7)	
	};

	/* Useful structure to record flags as both a bitwise SatusFlag object and/or a byte */
	struct FlagUnion {
		union {
			Byte byte;
			StatusFlags bit;
		};
	};

	/* Represents the internal state of a CPU (Not including memory) */
	struct CPUState {

	public:
		static constexpr Word DEFAULT_RESET_VECTOR = 0xFFFC;
		static constexpr Byte DEFAULT_SP = 0xFF;

		CPUState() { FLAGS.byte = 0x32; }		// Initialise flags to 0b00100000

		// Flags
		FlagUnion FLAGS;

		// Internal Registers
		Word PC = 0xFFFC;	// Program Counter
		Byte SP = 0xFF;		// Stack Pointer

		// Registers
		Byte A = 0;
		Byte X = 0;
		Byte Y = 0;

		/** Resets all fields in this state back to 0, SP init to 0xFF, PC init to DEFAULT_RESET_VECTOR */
		virtual void reset() {
			A = X = Y = 0;
			PC = DEFAULT_RESET_VECTOR;
			SP = DEFAULT_SP;
			FLAGS.byte = 0x32;
		}

		bool operator==(CPUState other) const {
			return (
				PC == other.PC && SP == other.SP &&
				A == other.A && X == other.X && Y == other.Y &&
				FLAGS.byte == other.FLAGS.byte);
		}
	};

	struct Program {
		Word loadAddress = 0x8000;
		Word size = 0;
		std::vector<Byte> bytes;
	};

	/* Useful for passing interchangble memory or register references (Only one can be valid at a given time) */
	struct Reference {
		u8 referenceType;
		union {
			Word memoryAddress;
			u8 reg;
		};
	};
}

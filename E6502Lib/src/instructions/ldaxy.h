#pragma once
#include "../types.h"
#include "base.h"

namespace E6502 {

	/** Imediate Instructions */
	const static Byte INS_LDA_IMM = 0xA9;	//10 101 001
	const static Byte INS_LDX_IMM = 0xA2;	//10 100 010
	const static Byte INS_LDY_IMM = 0xA0;	//10 100 000
	
	/** Zero Page instructions */
	const static Byte INS_LDA_ZP = 0xA5;	//10 100 101
	const static Byte INS_LDX_ZP = 0xA6;	//10 100 110
	const static Byte INS_LDY_ZP = 0xA4;	//10 100 100

	/** Zero Page Indexed (X/Y) Instructions */
	const static Byte INS_LDA_ZPX = 0xB5;	//10 110 101
	const static Byte INS_LDX_ZPY = 0xB6;	//10 110 110
	const static Byte INS_LDY_ZPX = 0xB4;	//10 110 100

	/* Absolute Instructions */
	const static Byte INS_LDA_ABS = 0xAD;	//10 101 101
	const static Byte INS_LDX_ABS = 0xAE;	//10 101 110
	const static Byte INS_LDY_ABS = 0xAC;	//10 101 100

	/* Absolute Indexed X/Y Instructions */
	const static Byte INS_LDA_ABSY = 0xB9;	//10 111 001
	const static Byte INS_LDA_ABSX = 0xBD;	//10 111 101
	const static Byte INS_LDX_ABSY = 0xBE;	//10 111 110
	const static Byte INS_LDY_ABSX = 0xBC;	//10 111 100


	/** Indirect Instructions */
	const static Byte INS_LDA_INDX = 0xA1;
	const static Byte INS_LDA_INDY = 0xB1;

	class LDAXY : public BaseInstruction {
	public:
		static constexpr Byte instructions[] = { 
			INS_LDA_IMM,	INS_LDX_IMM,	INS_LDY_IMM,
			INS_LDA_ZP,		INS_LDX_ZP,		INS_LDY_ZP,
			INS_LDA_ZPX,	INS_LDX_ZPY,	INS_LDY_ZPX,
			INS_LDA_ABS,	INS_LDX_ABS,	INS_LDY_ABS,
			INS_LDA_ABSX,	INS_LDA_ABSY,	INS_LDX_ABSY,	INS_LDY_ABSX,
			INS_LDA_INDX,
			INS_LDA_INDY
		};

		/** One function will handle the 'execute' method for all variants */
		static const insHandlerFn JSRInstructionHandler;

		/* Helper method to set the CPU flags. Only N(7) and Z(2) flags are affeted by LDA */
		static void setFlags(CPUState* state, u8 currentRegister);

		/** Actually handles execution of instructions */
		static u8 executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Defines properties common to all LDA instructions */
	struct LDAXYHandler : public INSTRUCTION_BASE {
		LDAXYHandler(Byte opcode_) {
			opcode = opcode_;
			execute = LDAXY::executeHandler;
			switch (opcode_) {
				case INS_LDA_IMM: name = "LDA - Load Accumulator [Immediate]"; break;
				case INS_LDX_IMM: name = "LDX - Load Index Register X [Immediate]"; break;
				case INS_LDY_IMM: name = "LDY - Load Index Register Y [Immediate]"; break;

				case INS_LDA_ZP: name = "LDA - Load Accumulator [Zero Page]"; break;
				case INS_LDX_ZP: name = "LDX - Load Index Register X [Zero Page]"; break;
				case INS_LDY_ZP: name = "LDY - Load Index Register Y [Zero Page]"; break;

				case INS_LDA_ZPX: name = "LDA - Load Accumulator [X-Indexed Zero Page]"; break;
				case INS_LDX_ZPY: name = "LDX - Load Index Register X [Y-Indexed Zero Page]"; break;
				case INS_LDY_ZPX: name = "LDY - Load Index Register Y [X-Indexed Zero Page]"; break;

				case INS_LDA_ABS: name = "LDA - Load Accumulator [Absolute]"; break;
				case INS_LDX_ABS: name = "LDX - Load Index Register X [Absolute]"; break;
				case INS_LDY_ABS: name = "LDY - Load Index Register Y [Absolute]"; break;

				case INS_LDA_ABSX: name = "LDA - Load Accumulator [X-Indexed Absolute]"; break;
				case INS_LDA_ABSY: name = "LDA - Load Accumulator [Y-Indexed Absolute]"; break;
				case INS_LDX_ABSY: name = "LDX - Load Accumulator [Y-Indexed Absolute]"; break;
				case INS_LDY_ABSX: name = "LDY - Load Accumulator [X-Indexed Absolute]"; break;

				case INS_LDA_INDX: name = "LDA - Load Accumulator [X-Indexed Zero Page Indirect]"; break;
				case INS_LDA_INDY: name = "LDA - Load Accumulator [Zero Page Indirect Y-Indexed]"; break;
				default: name = "Unknown opcode used to instantiate LDAXYHandler"; break;
			}
		}
	};
}

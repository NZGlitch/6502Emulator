#pragma once
#include "../types.h"
#include "base.h"

namespace E6502 {

	/** Imediate Instructions */
	const static Byte INS_LDY_IMM = 0xA0;	//10 100 000
	const static Byte INS_LDA_IMM = 0xA9;	//10 101 001
	const static Byte INS_LDX_IMM = 0xA2;	//10 100 010
	

	/** LDA Instruction Codes */
	const static Byte INS_LDA_INDX = 0xA1;
	const static Byte INS_LDA_ZP = 0xA5;
	
	const static Byte INS_LDA_ABS = 0xAD;
	const static Byte INS_LDA_INDY = 0xB1;
	const static Byte INS_LDA_ZPX = 0xB5;
	const static Byte INS_LDA_ABSY = 0xB9;
	const static Byte INS_LDA_ABSX = 0xBD;

	/** LDX Instruction Codes */
	//const static Byte INS_LDX_ZP = 0xA6;

	/** LDY Instruction Codes */
	//const static Byte INS_LDY_ZP = 0xA4;

	class LDAXY : public BaseInstruction {
	public:
		static constexpr Byte instructions[] = { 
			INS_LDA_IMM, INS_LDX_IMM, INS_LDY_IMM,
			INS_LDA_ZP, 
			INS_LDA_ZPX,
			INS_LDA_ABS,
			INS_LDA_ABSX,
			INS_LDA_ABSY,
			INS_LDA_INDX,
			INS_LDA_INDY
		};

		/** One function will handle the 'execute' method for all variants */
		static const insHandlerFn JSRInstructionHandler;

		/* Helper method to set the CPU flags. Only N(7) and Z(2) flags are affeted by LDA */
		static void setFlags(CPUState* state);

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
				case INS_LDX_IMM: name = "LDX - Load Accumulator [Immediate]"; break;
				case INS_LDY_IMM: name = "LDY - Load Accumulator [Immediate]"; break;

				case INS_LDA_ZP: name = "LDA - Load Accumulator [Zero Page]"; break;
				case INS_LDA_ZPX: name = "LDA - Load Accumulator [X-Indexed Zero Page]"; break;
				case INS_LDA_ABS: name = "LDA - Load Accumulator [Absolute]"; break;
				case INS_LDA_ABSX: name = "LDA - Load Accumulator [X-Indexed Absolute]"; break;
				case INS_LDA_ABSY: name = "LDA - Load Accumulator [Y-Indexed Absolute]"; break;
				case INS_LDA_INDX: name = "LDA - Load Accumulator [X-Indexed Zero Page Indirect]"; break;
				case INS_LDA_INDY: name = "LDA - Load Accumulator [Zero Page Indirect Y-Indexed]"; break;
				default: name = "Unknown opcode used to instantiate LDAXYHandler"; break;
			}
		}
	};
}

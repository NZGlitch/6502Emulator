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

	/* Absolute Indexed X */
	const static Byte INS_LDA_ABSX = 0xBD;	//10 111 101
	const static Byte INS_LDY_ABSX = 0xBC;	//10 111 100

	/* Absolute Indexed Y */
	const static Byte INS_LDA_ABSY = 0xB9;	//10 111 001
	const static Byte INS_LDX_ABSY = 0xBE;	//10 111 110


	/** Indirect Instructions */
	const static Byte INS_LDA_INDX = 0xA1;	//10 100 001
	const static Byte INS_LDA_INDY = 0xB1;	//10 110 001

	class LDAXY : public BaseInstruction {
	public:

		/** Actually handles execution of instructions */
		static u8 executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Handles Immediate Addressing Mode Instructions */
		static u8 immediateHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Handles ZeroPage Addressing Mode Instructions */
		static u8 zeroPageHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Handles ZeroPage Addressing Mode Instructions */
		static u8 zeroPageIndexedHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Handles Absolute and Absolute Indexed Addressing Mode Instructions */
		static u8 absoluteHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Returns the address of the register indicated by the instruction in the cpu state */
		static Byte* LDAXY::getRegFromInstruction(InstructionCode* instruction, CPUState* state);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);

		/** Helper method to get a value from memory and store in a register */
		static void fetchAndSaveToRegister(u8* cycles, Memory* memory, CPUState* state, Word address, Byte* reg);

		static constexpr InstructionHandler instructions[] = {
			// Immediate Instructions
			{INS_LDA_IMM, true, "LDA - Load Accumulator [Immediate]", LDAXY::immediateHandler},
			{INS_LDX_IMM, true, "LDX - Load Index Register X[Immediate]", LDAXY::immediateHandler},
			{INS_LDY_IMM, true, "LDY - Load Index Register Y[Immediate]", LDAXY::immediateHandler},

			// Zero Page Instructions
			{INS_LDA_ZP, true, "LDA - Load Accumulator[Zero Page]", LDAXY::zeroPageHandler},
			{INS_LDX_ZP, true, "LDX - Load Index Register X [Zero Page]", LDAXY::zeroPageHandler},
			{INS_LDY_ZP, true, "LDY - Load Index Register Y [Zero Page]", LDAXY::zeroPageHandler},

			// Zero Page Indexed Instructions
			{INS_LDA_ZPX, true, "LDA - Load Accumulator [X-Indexed Zero Page]", LDAXY::zeroPageIndexedHandler},
			{INS_LDX_ZPY, true, "LDX - Load Accumulator [Y-Indexed Zero Page]", LDAXY::zeroPageIndexedHandler},
			{INS_LDY_ZPX,  true, "LDY - Load Accumulator [X-Indexed Zero Page]", LDAXY::zeroPageIndexedHandler},

			// Absolute Instructions
			{INS_LDA_ABS,true,"LDA - Load Accumulator [Absolute]", LDAXY::absoluteHandler}, 
			{INS_LDX_ABS,true,"LDX - Load Index Register X [Absolute]", LDAXY::absoluteHandler},
			{INS_LDY_ABS,true,"LDY - Load Index Register Y [Absolute]", LDAXY::absoluteHandler},

			// Absolute X-Indexed Instructions
			{INS_LDA_ABSX,true,"LDA - Load Accumulator [X-Indexed Absolute]", LDAXY::absoluteHandler}, 
			{INS_LDY_ABSX,true,"LDY - Load Accumulator [X-Indexed Absolute]", LDAXY::absoluteHandler},

			// Absolute Y-Indexed Instructions
			{INS_LDA_ABSY,true,"LDA - Load Accumulator [Y-Indexed Absolute]", LDAXY::absoluteHandler},
			{INS_LDX_ABSY,true,"LDX - Load Accumulator [Y-Indexed Absolute]", LDAXY::absoluteHandler}
			 
		//	{INS_LDA_INDX,true,"", nullptr},
		//	{INS_LDA_INDY,true,"", nullptr}
		};
	};

	/** Defines properties common to all LDA instructions */
	struct LDAXYHandler : public INSTRUCTION_BASE {
		LDAXYHandler(Byte opcode_) {
			opcode = opcode_;
			execute = LDAXY::executeHandler;
			switch (opcode_) {
				case INS_LDA_INDX: name = "LDA - Load Accumulator [X-Indexed Zero Page Indirect]"; break;
				case INS_LDA_INDY: name = "LDA - Load Accumulator [Zero Page Indirect Y-Indexed]"; break;
				default: name = "Unknown opcode used to instantiate LDAXYHandler"; break;
			}
		}
	};
}

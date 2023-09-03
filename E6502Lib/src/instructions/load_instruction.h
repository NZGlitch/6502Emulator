#pragma once
#include "base.h"

namespace E6502 {

	
	class LoadInstruction : public BaseInstruction {
	public:

		/** Handles Immediate Addressing Mode Instructions */
		static u8 immediateHandler(CPU* cpu, Byte opCode);

		/** Handles ZeroPage Addressing Mode Instructions */
		static u8 zeroPageHandler(CPU* cpu, Byte opCode);

		/** Handles ZeroPage Addressing Mode Instructions */
		static u8 zeroPageIndexedHandler(CPU* cpu, Byte opCode);

		/** Handles Absolute and Absolute Indexed Addressing Mode Instructions */
		static u8 absoluteHandler(CPU* cpu, Byte opCode);

		/** Handles execution of Indirect Mode Instructions */
		static u8 indirectHandler(CPU* cpu, Byte opCode);

		/** Called to add Load Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);

		/** Helper method to get a value from memory and store in a register */
		static void fetchAndSaveToRegister(u8* cycles, CPU* cpu, Word address, u8 reg);

	};

	/* Global Instruction Definitions */

	/** Imediate Instructions */
	constexpr static InstructionHandler INS_LDA_IMM = { 0xA9, true, "LDA - Load Accumulator [Immediate]", LoadInstruction::immediateHandler };
	constexpr static InstructionHandler INS_LDX_IMM = { 0xA2, true, "LDX - Load Index Register X [Immediate]", LoadInstruction::immediateHandler };
	constexpr static InstructionHandler INS_LDY_IMM = { 0Xa0, true, "LDY - Load Index Register Y [Immediate]", LoadInstruction::immediateHandler };

	/** Zero Page instructions */
	constexpr static InstructionHandler INS_LDA_ZP = { 0Xa5, true, "LDA - Load Accumulator[Zero Page]", LoadInstruction::zeroPageHandler };
	constexpr static InstructionHandler INS_LDX_ZP = { 0XA6, true, "LDX - Load Index Register X [Zero Page]", LoadInstruction::zeroPageHandler };
	constexpr static InstructionHandler INS_LDY_ZP = { 0xA4, true, "LDY - Load Index Register Y [Zero Page]", LoadInstruction::zeroPageHandler };

	/** Zero Page Indexed (X/Y) Instructions */
	constexpr static InstructionHandler INS_LDA_ZPX = { 0xB5, true, "LDA - Load Accumulator [X-Indexed Zero Page]", LoadInstruction::zeroPageIndexedHandler };
	constexpr static InstructionHandler INS_LDX_ZPY = { 0xB6, true, "LDX - Load Accumulator [Y-Indexed Zero Page]", LoadInstruction::zeroPageIndexedHandler };
	constexpr static InstructionHandler INS_LDY_ZPX = { 0xB4, true, "LDY - Load Accumulator [X-Indexed Zero Page]", LoadInstruction::zeroPageIndexedHandler };

	/* Absolute Instructions */
	constexpr static InstructionHandler INS_LDA_ABS = { 0xAD, true, "LDA - Load Accumulator [Absolute]", LoadInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_LDX_ABS = { 0xAE, true, "LDX - Load Index Register X [Absolute]", LoadInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_LDY_ABS = { 0xAC, true, "LDY - Load Index Register Y [Absolute]", LoadInstruction::absoluteHandler };

	/* Absolute Indexed X */
	constexpr static InstructionHandler INS_LDA_ABSX = { 0xBD, true, "LDA - Load Accumulator [X-Indexed Absolute]", LoadInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_LDY_ABSX = { 0xBC, true, "LDY - Load Accumulator [X-Indexed Absolute]", LoadInstruction::absoluteHandler };

	/* Absolute Indexed Y */
	constexpr static InstructionHandler INS_LDA_ABSY = { 0xB9, true, "LDA - Load Accumulator [Y-Indexed Absolute]", LoadInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_LDX_ABSY = { 0xBE, true, "LDX - Load Accumulator [Y-Indexed Absolute]", LoadInstruction::absoluteHandler };

	// X-Indexed Zero Page Indirect
	constexpr static InstructionHandler INS_LDA_INDX = { 0xA1, true, "LDA - Load Accumulator [X-Indexed Zero Page Indirect]", LoadInstruction::indirectHandler };

	// ZeroPage Indirect Y-Indexed
	constexpr static InstructionHandler INS_LDA_INDY = { 0xB1, true, "LDA - Load Accumulator [Zero Page Indirect Y-Indexed]", LoadInstruction::indirectHandler };

	// Handy array of all load instructions
	static constexpr InstructionHandler LOAD_INSTRUCTIONS[] = {
		// Immediate Instructions
		INS_LDA_IMM, INS_LDX_IMM, INS_LDY_IMM,

		// Zero Page Instructions
		INS_LDA_ZP, INS_LDX_ZP, INS_LDY_ZP,

		// Zero Page Indexed Instructions
		INS_LDA_ZPX, INS_LDX_ZPY, INS_LDY_ZPX,

		// Absolute Instructions
		INS_LDA_ABS, INS_LDX_ABS, INS_LDY_ABS,

		// Absolute X-Indexed Instructions
		INS_LDA_ABSX, INS_LDY_ABSX,

		// Absolute Y-Indexed Instructions
		INS_LDA_ABSY, INS_LDX_ABSY,

		// X-Indexed Zero Page Indirect
		INS_LDA_INDX,

		// ZeroPage Indirect Y-Indexed
		INS_LDA_INDY
	};
}

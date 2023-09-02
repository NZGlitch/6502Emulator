#pragma once
#include "../types.h"
#include "base.h"

namespace E6502 {

	class StoreInstruction : public BaseInstruction {
	public:

		/** Absolute and Absolute-Indexed mode instructions */
		static u8 absoluteHandler(Memory* mem, CPUState* state, Byte opCode);

		/** Zero Page mode instructions */
		static u8 zeroPageHandler(Memory* mem, CPUState* state, Byte opCode);

		/** Zero Page mode instructions */
		static u8 zeroPageIndexedHandler(Memory* mem, CPUState* state, Byte opCode);

		/** Called to add Store Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Abslute Mode Instructions */
	constexpr static InstructionHandler INS_STA_ABS = { 0x8D, true, "STA - Store Accumulator [Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STX_ABS = { 0x8E, true, "STX - Store Index Register X [Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STY_ABS = { 0x8C, true, "STY - Store Index Register Y [Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STA_ABSX = { 0x9D, true, "STA - Store Accumulator [X-Indexed Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STA_ABSY = { 0x99, true, "STA - Store Accumulator [Y-Indexed Absolute]", StoreInstruction::absoluteHandler };

	/** Zero Page Instructions */
	constexpr static InstructionHandler INS_STA_ZP = { 0x85, true, "STA - Store Accumulator [Zero Page]", StoreInstruction::zeroPageHandler };
	constexpr static InstructionHandler INS_STX_ZP = { 0x86, true, "STX - Store Index Register X [Zero Page]", StoreInstruction::zeroPageHandler };
	constexpr static InstructionHandler INS_STY_ZP = { 0x84, true, "STY - Store Index Register Y [Zero PAge]", StoreInstruction::zeroPageHandler };

	/** Zero Page Indexed Instructions */
	constexpr static InstructionHandler INS_STA_ZPX = { 0x95, true, "STA - Store Accumulator [X-Indexed Zero Page]", StoreInstruction::zeroPageIndexedHandler };
	constexpr static InstructionHandler INS_STX_ZPY = { 0x96, true, "STX - Store Index Register X [Y-Indexed Zero Page]", StoreInstruction::zeroPageIndexedHandler };
	constexpr static InstructionHandler INS_STY_ZPX = { 0x94, true, "STY - Store Index Register Y [X-Indexed Zero PAge]", StoreInstruction::zeroPageIndexedHandler };

	// Handy array of all store instructions
	static constexpr InstructionHandler STORE_INSTRUCTIONS[] = {
		
		// Zero Page Instructions
		INS_STA_ZP, INS_STX_ZP, INS_STY_ZP,
		
		// Zero Page Indexed Instructions
		INS_STA_ZPX, INS_STX_ZPY, INS_STY_ZPX,

		// Absolute Instructions
		INS_STA_ABS, INS_STX_ABS, INS_STY_ABS,
		
		// Absolute X-Indexed Instructions
		INS_STA_ABSX, INS_STA_ABSY,
		
		// Absolute Y-Indexed Instructions
		
		// X-Indexed Zero Page Indirect
		
		// ZeroPage Indirect Y-Indexed
		
	};

}

#pragma once
#include "../types.h"
#include "base.h"

namespace E6502 {

	class StoreInstruction : public BaseInstruction {
	public:

		/** Actually handles execution of JSR instruction */
		static u8 absoluteHandler(Memory* mem, CPUState* state, Byte opCode);

		/** Called to add Store Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Abslute Mode Instructions */
	constexpr static InstructionHandler INS_STA_ABS = { 0x8D, true, "STA - Store Accumulator [Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STX_ABS = { 0x8E, true, "STX - Store Index Register X [Absolute]", StoreInstruction::absoluteHandler };
	constexpr static InstructionHandler INS_STY_ABS = { 0x8C, true, "STY - Store Index Register Y [Absolute]", StoreInstruction::absoluteHandler };

	// Handy array of all store instructions
	static constexpr InstructionHandler STORE_INSTRUCTIONS[] = {
		
		// Zero Page Instructions
		
		// Zero Page Indexed Instructions
		
		// Absolute Instructions
		INS_STA_ABS, INS_STX_ABS, INS_STY_ABS,
		
		// Absolute X-Indexed Instructions
		
		// Absolute Y-Indexed Instructions
		
		// X-Indexed Zero Page Indirect
		
		// ZeroPage Indirect Y-Indexed
		
	};

}

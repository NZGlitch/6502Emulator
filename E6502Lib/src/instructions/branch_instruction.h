#pragma once
#include "base.h"

namespace E6502 {

	class BranchInstruction : public BaseInstruction {
	public:
		
		/** Handles execution of all brnach instructions */
		static void branchHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Branch Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** DEC Mem By One */
	constexpr static InstructionHandler INS_BCC_REL = { 0x90, true, "BCC - Branch on Carry Clear [Relative]", BranchInstruction::branchHandler };

	// Array of all Increment/Decrement instructions
	static constexpr InstructionHandler BRANCH_INSTRUCTIONS[] = {
		INS_BCC_REL
	};
}

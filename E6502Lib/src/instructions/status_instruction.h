#pragma once
#include "base.h"

namespace E6502 {

	class StatusInstruction : public BaseInstruction {
	public:

		/** Handles execution of all status flag instructions */
		static void statusHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Status Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// Status instruction defs
	constexpr static InstructionHandler INS_STATUS = { 0x00, true, "XXX - Status instruction [Implied]", StatusInstruction::statusHandler };

	// Array of all Status Fla instructions
	static constexpr InstructionHandler STATUS_INSTRUCTIONS[] = {
		INS_STATUS
	};
}

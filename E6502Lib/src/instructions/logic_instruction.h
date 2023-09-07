#pragma once
#include "base.h"

namespace E6502 {

	class LogicInstruction : public BaseInstruction {
	public:

		/** Actually handles execution of JSR instruction */
		static void logicHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** ASL Instruction Definitions */
	constexpr static InstructionHandler INS_ASL_ACC = { 0x0A, true, "ASL - Arithmetic Shift Left [Accumulator]",	LogicInstruction::logicHandler };
	

	// Array of all logic instructions
	static constexpr InstructionHandler LOGIC_INSTRUCTIONS[] = {
		INS_ASL_ACC
	};
}

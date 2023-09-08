#pragma once
#include "base.h"

namespace E6502 {

	class StackInstruction : public BaseInstruction {
	public:

		/** Handle stack push ops */
		static void pushHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Handle stack pull ops */
		static void pullHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add TransferInstruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Push ops */
	constexpr static InstructionHandler INS_PHA = { 0x48, true, "PHA - Push Accumulator On Stack",			StackInstruction::pushHandler };
	constexpr static InstructionHandler INS_PHP = { 0x08, true, "PHP - Push Processor Status On Stack",		StackInstruction::pushHandler };

	/** Pull ops */
	constexpr static InstructionHandler INS_PLA = { 0x68, true, "PLA - Pull Accumulator From Stack",		StackInstruction::pullHandler };
	constexpr static InstructionHandler INS_PLP = { 0x28, true, "PLP - Pull Processor Status From Stack",	StackInstruction::pullHandler };

	// Array of all transfer instructions
	static constexpr InstructionHandler STACK_INSTRUCTIONS[] = {
		/* Push ops */
		INS_PHA, INS_PHP,

		/* Pull ops */
		INS_PLA, INS_PLP
	};
}

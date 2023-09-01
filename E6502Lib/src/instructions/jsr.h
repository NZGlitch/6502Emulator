#pragma once
#include "../types.h"
#include "base.h"

namespace E6502 {

	/** JSR Instruction Code */
	const static Byte INS_JSR = 0x20;

	class JSR : public BaseInstruction {
	public:
		/** Actually handles execution of JSR instruction */
		static u8 jsrHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);

		/** Defines all JSR instruction handlers */
		static constexpr InstructionHandler instructions[] = {
			{INS_JSR, true, "JSR - Jump to Subroutine [Absolute]", JSR::jsrHandler},
		};
	};
}

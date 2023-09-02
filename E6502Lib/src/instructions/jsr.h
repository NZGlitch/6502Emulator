#pragma once
#include "base.h"

namespace E6502 {

	class JSR : public BaseInstruction {
	public:
		
		/** Actually handles execution of JSR instruction */
		static u8 jsrHandler(CPU* cpu, Byte opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** JSR Instruction Code */
	static InstructionHandler INS_JSR = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", JSR::jsrHandler };
	
}

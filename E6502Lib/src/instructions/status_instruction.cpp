#pragma once
#include "status_instruction.h"

namespace E6502 {

	/** Handles execution of all branch instructions */
	void StatusInstruction::statusHandler(CPU* cpu, u8& cycles, Byte opCode) {
		
	}

	/** Called to add Increment/Decrement instruction handlers to the emulator */
	void StatusInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STATUS_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

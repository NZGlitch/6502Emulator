#pragma once
#include "incdec_instruction.h"

namespace E6502 {

	/** Handles execution of all logical instructions */
	void IncDecInstruction::incdecHandler(CPU* cpu, u8& cycles, Byte opCode) {
		
	}

	/** Called to add logic instruction handlers to the emulator */
	void IncDecInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : INCDEC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

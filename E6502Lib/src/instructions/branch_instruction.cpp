#pragma once
#include "branch_instruction.h"

namespace E6502 {

	/** Handles execution of all branch instructions */
	void BranchInstruction::branchHandler(CPU* cpu, u8& cycles, Byte opCode) {
		s8 offset = cpu->readPCByte(cycles);
		if (!cpu->getCarryFlag(cycles)) {
			cpu->branch(cycles, offset);
		}
	}

	/** Called to add Increment/Decrement instruction handlers to the emulator */
	void BranchInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : BRANCH_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

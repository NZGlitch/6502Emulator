#pragma once
#include "logic_instruction.h"

namespace E6502 {

	/** Actually handles execution of JSR instruction */
	void LogicInstruction::logicHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// ASL ACC
		Byte acc = cpu->regValue(cycles, CPU::REGISTER_A);
		acc << 1; cycles++;
		cpu->saveToRegAndFlagNZC(cycles, CPU::REGISTER_A, acc);
	}

	/** Called to add LDA Instruction handlers to the emulator */
	void LogicInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : LOGIC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

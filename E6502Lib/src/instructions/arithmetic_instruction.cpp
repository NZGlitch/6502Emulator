#pragma once
#include "arithmetic_instruction.h"

namespace E6502 {

	/** Handles execution of all arithmetic instructions */
	void ArithmeticInstruction::arithmeticHandler(CPU* cpu, u8& cycles, Byte opCode) {
		Byte operandB = cpu->readPCByte(cycles);
		cpu->addAccumulator(cycles, operandB);
	}

	/** Called to add arithmetic instruction handlers to the emulator */
	void ArithmeticInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : ARITHMETIC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

#pragma once
#include "arithmetic_instruction.h"

namespace E6502 {

	/** Handles execution of ADC instructions */
	void ArithmeticInstruction::adcHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Memory mode
		Byte md = (opCode >> 2) & 0x7;					// Memory Mode (bits 4,3,2)
		Byte operandB = 0x00;

		if (md == ADDRESS_MODE_IMMEDIATE) {				// Base class can't handle immediate instructions
			operandB = cpu->readPCByte(cycles);
		} else {
			Reference ref = BaseInstruction::getReferenceForMode(cpu, cycles, md);
			operandB = cpu->readReferenceByte(cycles, ref);
		}
		cpu->addAccumulator(cycles, operandB);
	}

	/** Handles execution of SBC instructions */
	void ArithmeticInstruction::sbcHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// TODO
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

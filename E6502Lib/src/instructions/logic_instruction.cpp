#pragma once
#include "logic_instruction.h"

namespace E6502 {

	/** Handles execution of all logical instructions */
	void LogicInstruction::logicHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Split opcode into more useful fields
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		Byte md = (opCode >> 2) & 0x7;							// Memory Mode (bits 4,3,2)

		// Declare vars
		Byte operandA = 0x00;
		Byte result = 0x00;

		// Get the operands
		if (md == ADDRESS_MODE_IMMEDIATE)
			operandA = cpu->readPCByte(cycles);
		else {
			Reference ref = getReferenceForMode(cpu, cycles, md);
			operandA = cpu->readReferenceByte(cycles, ref);
		}
		Byte operandB = cpu->regValue(cycles, CPU::REGISTER_A);

		// Perform the operation (method based on the Op Mode), set the carry argument as required
		switch (op) {
			case OP_BIT:
			case OP_AND: result = AND(operandA, operandB); break; 
			case OP_EOR: result = EOR(operandA, operandB); break;
			case OP_ORA: result = ORA(operandA, operandB); break;
			default: {
				fprintf(stderr, "Unknown operation %d for logical instruction\n", op);
				break;
			}
		}

		// Set the N, Z flags based on the result
		cpu->setNegativeFlag(cycles, result >> 7);
		cpu->setZeroFlag(cycles, result == 0);

		// Save the data to accumulator (Unless BIT)
		if (op != OP_BIT)
			cpu->saveToReg(cycles, CPU::REGISTER_A, result);
	}

	/** Called to add logic instruction handlers to the emulator */
	void LogicInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : LOGIC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

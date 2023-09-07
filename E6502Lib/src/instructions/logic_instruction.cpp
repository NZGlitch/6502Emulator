#pragma once
#include "logic_instruction.h"

namespace E6502 {

	/** Handles execution of all logical instructions */
	void LogicInstruction::logicHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Split opcode into more useful fields
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		Byte md = (opCode >> 2) & 0x7;							// Memory Mode (bits 4,3,2)

		// Carry needs to be set by the op
		Byte carry = 0;

		// Get the data (method based on the Memory Mode)
		Byte data = getByteForMode(cpu, cycles, md);

		// Perform the operation (method based on the Op Mode), set the carry byte as required
		performOp(cpu, cycles, op, data, carry);

		// Set the N and Z flags based on the result
		cpu->setNegativeFlag(cycles, data >> 7);
		cpu->setZeroFlag(cycles, data == 0);

		// Save the data based on the memory mode.
		saveByteForMode(cpu, cycles, md, data);
	}

	/* Helper method actually performs the required operation */
	void LogicInstruction::performOp(CPU* cpu, u8& cycles, Byte op, Byte& value, Byte& carry) {
		switch (op) {
			case OP_ASL:		/* Arithmetic Shift Left */
				carry = value >> 7;
				value = value << 1; cycles++;
				cpu->setCarryFlag(cycles, carry);
				break;

			case OP_LSR:		/* Logical shift Right */
				carry = value & 0x01;
				value = value >> 1; cycles++;
				cpu->setCarryFlag(cycles, carry);
				break;

			default:
				fprintf(stderr, "Unknown operation %d for logical instruction\n", op);
				break;
		}
	}

	/** Called to add logic Instruction handlers to the emulator */
	void LogicInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : LOGIC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

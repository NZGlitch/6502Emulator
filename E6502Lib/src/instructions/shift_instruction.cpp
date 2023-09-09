#pragma once
#include "shift_instruction.h"

namespace E6502 {

	/** Handles execution of all logical instructions */
	void ShiftInstruction::shiftHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Split opcode into more useful fields
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		Byte md = (opCode >> 2) & 0x7;							// Memory Mode (bits 4,3,2)

		// Carry needs to be set by the op
		Byte carry = 0;

		// Get a refrence to the data location based on the memory mode
		Reference ref = getReferenceForMode(cpu, cycles, md);
		Byte data = cpu->readReferenceByte(cycles, ref);

		// Perform the operation (method based on the Op Mode), set the carry argument as required
		performOp(cpu, cycles, op, data, carry);

		// Set the N, Z, C flags based on the result
		cpu->setNegativeFlag(cycles, data >> 7);
		cpu->setZeroFlag(cycles, data == 0);
		cpu->setCarryFlag(cycles, (carry != 0));

		// Save the data to accumulator
		cpu->saveToReg(cycles, CPU::REGISTER_A, data);

		// Cycle correction - Shift instructions seem to have unusual fixed costs? TODO confirm
		if (md == ADDRESS_MODE_ACCUMULATOR) cycles = 2;
		if (md == ADDRESS_MODE_ABSOLUTE_X) cycles = 7;	
	}

	/* Helper method actually performs the required operation */
	void ShiftInstruction::performOp(CPU* cpu, u8& cycles, Byte op, Byte& value, Byte& carry) {
		switch (op) {
			/* Arithmetic Shift Left */
			case OP_ASL:
				carry = value >> 7; cycles++;
				value = value << 1; cycles++;
				break;
			/* Rotate Left */
			case OP_ROL:
				carry = value >> 7; cycles++;
				value = value << 1; cycles++;
				if (cpu->getCarryFlag(cycles)) value |= 0x01;
				break;
			/* Logical shift Right */
			case OP_LSR:
				carry = value & 0x01; cycles++;
				value = value >> 1; cycles++;
				break;
			/* Rotate Right */
			case OP_ROR:
				carry = value & 0x01; cycles++;
				value = value >> 1; cycles++;
				if (cpu->getCarryFlag(cycles)) value |= 0x80;
				break;
			/* Unknown operation */
			default:
				fprintf(stderr, "Unknown operation %d for logical instruction\n", op);
				break;
		}
	}

	/** Called to add logic instruction handlers to the emulator */
	void ShiftInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : SHIFT_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

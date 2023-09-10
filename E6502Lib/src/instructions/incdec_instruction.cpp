#pragma once
#include "incdec_instruction.h"

namespace E6502 {

	/** Handles execution of all Increment and Decrement instructions */
	void IncDecInstruction::incdecHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Split opcode into more useful fields
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		Byte md = (opCode >> 2) & 0x7;							// Memory Mode (bits 4,3,2)
		
		// Reference for reading and writing
		Reference ref{ CPU::REFERENCE_REG, CPU::REGISTER_A };

		if (md == ADDRESS_MODE_IMPLIED) {	// Implied mode opcodes can not be handled by parent class
			switch (opCode) {
				case INS_DEX_IMP.opcode: ref.reg = CPU::REGISTER_X; op = OP_DEC; break;
				case INS_DEY_IMP.opcode: ref.reg = CPU::REGISTER_Y; op = OP_DEC; break;
				case INS_INX_IMP.opcode: ref.reg = CPU::REGISTER_X; op = OP_INC; break;
				case INS_INY_IMP.opcode: ref.reg = CPU::REGISTER_Y; op = OP_INC; break;
			}
		}
		else {
			ref = getReferenceForMode(cpu, cycles, md);
		}

		Byte operand = cpu->readReferenceByte(cycles, ref);
		Byte result = 0;

		// Perform the operation (method based on the Op Mode), set the carry argument as required
		switch (op) {
			case OP_INC: result = IncDecInstruction::INC(operand); cycles++; break;
			case OP_DEC: result = IncDecInstruction::DEC(operand); cycles++; break;
			default: {
				fprintf(stderr, "Unknown operation %d for IncDec instruction\n", op);
				break;
			}
		}
		
		// Set the N, Z flags based on the result
		cpu->setNegativeFlag(cycles, result >> 7);
		cpu->setZeroFlag(cycles, result == 0);

		// Save
		cpu->writeReferenceByte(cycles, ref, result);		

		// Some instructions have fixed cycle values that need to be corrected
		if (md == ADDRESS_MODE_ABSOLUTE_X) cycles = 7;
		if (md == ADDRESS_MODE_IMPLIED) cycles = 2;
	}

	/** Called to add Increment/Decrement instruction handlers to the emulator */
	void IncDecInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : INCDEC_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

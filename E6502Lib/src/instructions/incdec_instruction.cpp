#pragma once
#include "incdec_instruction.h"

namespace E6502 {

	/** Handles execution of all logical instructions */
	void IncDecInstruction::incdecHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Split opcode into more useful fields
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		Byte md = (opCode >> 2) & 0x7;							// Memory Mode (bits 4,3,2)

		Reference ref = getReferenceForMode(cpu, cycles, md);
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

		// Indexed instructions have fixed cycle count (regardless of page crossig)
		if (md == ADDRESS_MODE_ABSOLUTE_X) cycles = 7;
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

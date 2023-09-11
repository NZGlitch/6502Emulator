#pragma once
#include "branch_instruction.h"

namespace E6502 {

	/** Handles execution of all branch instructions */
	void BranchInstruction::branchHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Get opcode, init branch
		Byte op = ((opCode >> 3) & 0x1C) | (opCode & 0x03);		// Op Mode (bits 7,6,5,1,0)
		bool branch = false;

		// Test flags based on opcode
		switch (op) {
		case OP_BCC: branch = !cpu->getFlag(cycles, CPU::FLAG_CARRY); break;
		case OP_BNE: branch = !cpu->getFlag(cycles, CPU::FLAG_ZERO); break;
		case OP_BPL: branch = !cpu->getFlag(cycles, CPU::FLAG_NEGATIVE); break;
		case OP_BVC: branch = !cpu->getFlag(cycles, CPU::FLAG_OVERFLOW); break;
		case OP_BCS: branch = cpu->getFlag(cycles, CPU::FLAG_CARRY); break;
		case OP_BEQ: branch = cpu->getFlag(cycles, CPU::FLAG_ZERO); break;
		case OP_BMI: branch = cpu->getFlag(cycles, CPU::FLAG_NEGATIVE); break;
		case OP_BVS: branch = cpu->getFlag(cycles, CPU::FLAG_OVERFLOW); break;
			default: {
				fprintf(stderr, "Unknown operation %d for Branch instruction\n", op);
				break;
			}
		}

		s8 offset = cpu->readPCByte(cycles);
		if (branch) cpu->branch(cycles, offset);
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

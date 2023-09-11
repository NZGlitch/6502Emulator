#pragma once
#include "status_instruction.h"

namespace E6502 {

	/** Handles execution of all status instructions */
	void StatusInstruction::statusHandler(CPU* cpu, u8& cycles, Byte opCode) {

		switch (opCode) {
			case INS_CLC_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_CARRY, false); cycles++; break;
			case INS_CLD_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_DECIMAL, false); cycles++; break;
			case INS_CLI_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_INTERRUPT_DISABLE, false); cycles++; break;
			case INS_CLV_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_OVERFLOW, false); cycles++; break;
			case INS_SEC_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_CARRY, true); cycles++; break;
			case INS_SED_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_DECIMAL, true); cycles++; break;
			case INS_SEI_IMP.opcode: cpu->setFlag(cycles, CPU::FLAG_INTERRUPT_DISABLE, true); cycles++; break;
			default: {
				fprintf(stderr, "Invalid opcode for status instruction %X", opCode);
			}
		}
	}

	/** Called to add status instruction handlers to the emulator */
	void StatusInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STATUS_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

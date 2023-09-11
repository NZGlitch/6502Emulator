#include "transfer_instruction.h"
#include <vector>

namespace E6502 {

	void TransferInstruction::transferRegHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// First ascertain the source and target registers
		Byte source = CPU::REGISTER_A;
		Byte target = CPU::REGISTER_A;

		switch (opCode) {
			case INS_TAX.opcode: source = CPU::REGISTER_A; target = CPU::REGISTER_X; break;
			case INS_TAY.opcode: source = CPU::REGISTER_A; target = CPU::REGISTER_Y; break;
			case INS_TXA.opcode: source = CPU::REGISTER_X; target = CPU::REGISTER_A; break;
			case INS_TYA.opcode: source = CPU::REGISTER_Y; target = CPU::REGISTER_A; break;
		}

		// Get the value
		Byte value = cpu->regValue(cycles, source);

		// Save reg and set flags
		cpu->saveToReg(cycles, target, value);
		cpu->setFlag(cycles, CPU::FLAG_NEGATIVE, value >> 7);
		cpu->setFlag(cycles, CPU::FLAG_ZERO, value == 0);
	}

	void TransferInstruction::transferStackHandler(CPU* cpu, u8& cycles, Byte opCode) {
		Byte value = 0;
		switch (opCode) {
			case INS_TSX.opcode:
				value = cpu->getSP(cycles);
				cpu->saveToReg(cycles, CPU::REGISTER_X, value);
				cpu->setFlag(cycles, CPU::FLAG_NEGATIVE, value >> 7);
				cpu->setFlag(cycles, CPU::FLAG_ZERO, value == 0);
				break;
			case INS_TXS.opcode: cpu->setSP(cycles, cpu->regValue(cycles, CPU::REGISTER_X)); break;
		}
	}

	/** Add TransferInstruction Handlers */
	void TransferInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : TRANS_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

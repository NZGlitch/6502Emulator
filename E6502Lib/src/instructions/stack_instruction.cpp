#include "stack_instruction.h"

namespace E6502 {

	void StackInstruction::pushHandler(CPU* cpu, u8& cycles, Byte opCode) {
		Byte value = 0x00;
		if (opCode == INS_PHA.opcode)
			value = cpu->regValue(cycles, CPU::REGISTER_A);
		else if (opCode == INS_PHP.opcode)
			value = cpu->getFlags(cycles).byte;
		cpu->pushStackByte(cycles, value);
	}

	void StackInstruction::pullHandler(CPU* cpu, u8& cycles, Byte opCode) {
		Byte value = cpu->pullStackByte(cycles);
		if (opCode == INS_PLA) {
			cpu->saveToReg(cycles, CPU::REGISTER_A, value); 
			cpu->setNegativeFlag(cycles, value >> 7);
			cpu->setZeroFlag(cycles, value == 0);
			
		} else if (opCode == INS_PLP.opcode) {
			FlagUnion flags = cpu->getFlags(cycles);
			flags.byte = (flags.byte & 0x30 | (value & 0xCF));		// Important - don't change bits 4 and 5
			cpu->setFlags(cycles, flags);
		}
		cycles += 2;
	}

	/** Add TransferInstruction Handlers */
	void StackInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STACK_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

#include "base.h"

namespace E6502 {
	
	BaseInstruction::BaseInstruction() {}


	/* Uses Field B (Bits 4,3,2) to determine the addressing mode and returns a reference to the correct location */
	Reference BaseInstruction::getReferenceForMode(CPU * cpu, u8 & cycles, Byte mode) {
		Word addr = 0x0;
		switch (mode) {
			//Accumulator mode
			case ADDRESS_MODE_ACCUMULATOR:
				return Reference{ CPU::REFERENCE_REG, CPU::REGISTER_A };
			case ADDRESS_MODE_ABSOLUTE:
				addr = cpu->readPCByte(cycles);
				addr |= (cpu->readPCByte(cycles) << 8); cycles++;
				return Reference{ CPU::REFERENCE_MEM, addr };
			default: {
				fprintf(stderr, "Unknown memory mode %d in BaseInstruction::getByteForMode\n", mode);
				return Reference{};
			}
		}
	}
}

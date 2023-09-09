#include "base.h"

namespace E6502 {
	
	BaseInstruction::BaseInstruction() {}


	/* Uses Field B (Bits 4,3,2) to determine the addressing mode and returns a reference to the correct location */
	Reference BaseInstruction::getReferenceForMode(CPU * cpu, u8 & cycles, Byte mode) {
		Byte index = 0x0;
		Word preAddr = 0x0;
		Word addr = 0x0;
		switch (mode) {
			//Accumulator mode
			case ADDRESS_MODE_INDIRECT_X:
				preAddr = cpu->readPCByte(cycles);
				preAddr += cpu->regValue(cycles, CPU::REGISTER_X);
				preAddr &= 0x00FF; cycles++;
				addr = cpu->readWord(cycles, preAddr);
				return Reference{ CPU::REFERENCE_MEM, addr };
			case ADDRESS_MODE_ZERO_PAGE:
				addr = cpu->readPCByte(cycles);
				return Reference{ CPU::REFERENCE_MEM, (Word)(0x00FF & addr) };
			case ADDRESS_MODE_ACCUMULATOR:
				return Reference{ CPU::REFERENCE_REG, CPU::REGISTER_A };
			case ADDRESS_MODE_ABSOLUTE:
				addr = cpu->readPCByte(cycles);
				addr |= (cpu->readPCByte(cycles) << 8);
				return Reference{ CPU::REFERENCE_MEM, addr };
			case ADDRESS_MODE_INDIRECT_Y:
				preAddr = cpu->readPCByte(cycles);
				preAddr = cpu->readWord(cycles, preAddr);
				addr = preAddr + cpu->regValue(cycles, CPU::REGISTER_Y);
				// Add cycle if page crossed
				if ((addr & 0xFF) < (preAddr & 0xFF)) cycles++;
				return Reference{ CPU::REFERENCE_MEM, addr };
			case ADDRESS_MODE_ZERO_PAGE_X:
				addr = cpu->readPCByte(cycles); cycles++;
				addr += cpu->regValue(cycles, CPU::REGISTER_X);
				return Reference{ CPU::REFERENCE_MEM, (Word)(0x00FF & addr) };
			case ADDRESS_MODE_ABSOLUTE_Y:
				preAddr = cpu->readPCByte(cycles);
				preAddr |= (cpu->readPCByte(cycles) << 8);
				addr = preAddr + cpu->regValue(cycles, CPU::REGISTER_Y);
				// Increment cycles if page crossed
				if ((preAddr & 0xFF) > (addr & 0xFF)) cycles++;
				return Reference{ CPU::REFERENCE_MEM, addr };
			case ADDRESS_MODE_ABSOLUTE_X:
				preAddr = cpu->readPCByte(cycles);
				preAddr |= (cpu->readPCByte(cycles) << 8);
				addr = preAddr + cpu->regValue(cycles, CPU::REGISTER_X);
				// Increment cycles if page crossed
				if ((preAddr & 0xFF) > (addr & 0xFF)) cycles++;
				return Reference{ CPU::REFERENCE_MEM, addr };
			default: {
				fprintf(stderr, "Unknown memory mode %d in BaseInstruction::getByteForMode\n", mode);
				return Reference{};
			}
		}
	}
}

#include "base.h"

namespace E6502 {
	
	BaseInstruction::BaseInstruction() {}

	Byte BaseInstruction::getByteForMode(CPU * cpu, u8 & cycles, Byte mode) {
		switch (mode) {
			case 0x2: return cpu->regValue(cycles, CPU::REGISTER_A);		//Accumulator mode
			default: {
				fprintf(stderr, "Unknown memory mode %d for logical instruction\n", mode);
				return 0;
			}
		}
	}

	void BaseInstruction::saveByteForMode(CPU * cpu, u8 & cycles, Byte mode, Byte valueToSave) {
		switch (mode) {
			case 0x2: cpu->saveToReg(cycles, CPU::REGISTER_A, valueToSave);		//Accumulator mode
			default: {
				fprintf(stderr, "Unknown memory mode %d for logical instruction\n", mode);
			}
		}
	}
}
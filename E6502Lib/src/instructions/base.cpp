#include "base.h"

namespace E6502 {
	
	BaseInstruction::BaseInstruction() {}

	/* Uses Field B (Bits 4,3,2) to determine the addressing mode and reads a Byte from the relvant location
	* NOTE: modes that require bytes from the instruction will cause the PC to change
	* Will not affect any processor falgs
	*/
	Byte BaseInstruction::getByteForMode(CPU * cpu, u8 & cycles, Byte mode) {
		switch (mode) {
			//Accumulator mode
			case ADDRESS_MODE_ACCUMULATOR: 
				return cpu->regValue(cycles, CPU::REGISTER_A);				
			default: {
				fprintf(stderr, "Unknown memory mode %d in BaseInstruction::getByteForMode\n", mode);
				return 0;
			}
		}
	}

	/* Uses Field B (Bits 4,3,2) to determine the addressing mode and writes a Byte from the relvant location
	 * NOTE: modes that require bytes from the instruction will cause the PC to change
	 * Will not affect any processor falgs
	 */
	void BaseInstruction::saveByteForMode(CPU * cpu, u8 & cycles, Byte mode, Byte valueToSave) {
		switch (mode) {
			//Accumulator mode
			case ADDRESS_MODE_ACCUMULATOR: 
				cpu->saveToReg(cycles, CPU::REGISTER_A, valueToSave);
				break;
			default: {
				fprintf(stderr, "Unknown memory mode %d in BaseInstruction::saveByteForMode\n", mode);
			}
		}
	}
}

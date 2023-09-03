#include "jsr.h"
#include <vector>

namespace E6502 {
	u8 JSR::jsrHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte lsb = cpu->incPCandReadByte(cycles);															// Get the lsb of the target address
		
		//TODO replace deprecated funcs
																										//cpu->writeByte(cycles, cpu->currentState->pushSP(), ((cpu->currentState->PC >> 8) & 0xFF));		// Copy the high order bits of PC to stack
		//cpu->writeByte(cycles, cpu->currentState->pushSP(), (cpu->currentState->PC & 0xFF));			// Copy the low order bits of PC to stack
		//cpu->currentState->PC = (cpu->readByte(cycles, cpu->currentState->incPC()) << 8) | lsb;			// Update the program counter to jump
		cycles++;
		return cycles;
	};

	/** Implementation of addhandlers needs to be after the struct defs */
	void JSR::addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_JSR.opcode] = &INS_JSR;
	}
}

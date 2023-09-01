#include "jsr.h"

namespace E6502 {
	u8 JSR::jsrHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte lsb = mem->readByte(cycles, state->incPC());						// Get the lsb of the target address
		mem->writeByte(cycles, state->pushSP(), ((state->PC >> 8) & 0xFF));		// Copy the high order bits of PC to stack
		mem->writeByte(cycles, state->pushSP(), (state->PC & 0xFF));			// Copy the low order bits of PC to stack
		state->PC = (mem->readByte(cycles, state->incPC()) << 8) | lsb;			// Update the program counter to jump
		cycles++;
		return cycles;
	};

	/** Implementation of addhandlers needs to be after the struct defs */
	void JSR::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : JSR::instructions) {
			handlers[handler.opcode] = new InstructionHandler{ handler.opcode, handler.isLegal, handler.name, handler.execute };
		}
	}
}

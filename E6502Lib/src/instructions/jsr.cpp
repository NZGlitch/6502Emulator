#include "jsr.h"

u8 JSR::executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
	u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
	switch (opCode->code) {
		case INS_JSR: {
			Byte lsb = mem->readByte(cycles, state->incPC());						// Get the lsb of the target address
			mem->writeByte(cycles, state->pushSP(), ((state->PC >> 8) & 0xFF));		// Copy the high order bits of PC to stack
			mem->writeByte(cycles, state->pushSP(), (state->PC & 0xFF));			// Copy the low order bits of PC to stack
			state->PC = (mem->readByte(cycles, state->incPC()) << 8) | lsb;			// Update the program counter to jump
			cycles++;
			break;
		}
		default: {
			//Shouldn't be here!
			fprintf(stderr, "Attempting to use JSA instruction executor for non JSR instruction $%X\n", opCode->code);
			// We won't change the state or use cycles
			return (u8)0;
		}
	}
	return cycles;
};

/** Implementation of addhandlers needs to be after the struct defs */
void JSR::addHandlers(InstructionHandler* handlers[]) {
	handlers[INS_JSR] = ((InstructionHandler*) new JSR_ABS);
}

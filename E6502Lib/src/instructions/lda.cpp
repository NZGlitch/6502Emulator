#include "lda.h"

/* Helper method to set the CPU flags. Only N(7) and Z(2) flags are affeted by LDA */
void LDA::setFlags(CPUState* state) {
	state->Z = (state->A == 0);
	state->N = (state->A >> 7);
}

/** One function will handle the 'execute' method for all variants */
u8 LDA::executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
	u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
	switch (opCode->B) {
		case INDIRECT_X:
		case INDIRECT_Y: {
			// Read the next byte as the base for a zero page address.
			Byte baseAddress = mem->readByte(cycles, state->incPC());

			//Get the index
			Byte index = (opCode->B == INDIRECT_X ? state->X : state->Y);

			// Base + Index (ignore carry for now)
			Word indirectAddressLSB = baseAddress + index;
			Word indirectAddress = (0x00FF & indirectAddressLSB);

			// Read inderect address from zero page and construct target address
			Byte lsb = mem->readByte(cycles, indirectAddress);
			Byte msb = mem->readByte(cycles, indirectAddress + 1);

			if (opCode->B == INDIRECT_Y) {
				// For Indirect Y only, if the earlier baseAddress+Y caused a carry then we need to increment msb
				if (indirectAddressLSB > 0xFF)
					msb++; cycles++;
			}
			else {
				cycles++;	//indirect x is always 6 cycels for some reason?
			}

			// Set A register with value in target address
			Word targetAddress = (msb << 8) | lsb;
			state->A = mem->readByte(cycles, targetAddress);
			break;
		}
		case ZERO_PAGE: {
			/* Read the next byte as the lsb for a zero page address */
			Byte address = mem->readByte(cycles, state->incPC());
			state->A = mem->readByte(cycles, address);
			break;
		}
		case IMMEDIATE: {
			/* Read the next byte from PC and put into the accumulator */
			state->A = mem->readByte(cycles, state->incPC());
			break;
		}
		case ZERO_PAGE_X: {
			// Read the next byte as the lsb for a zero page base address
			Byte address = mem->readByte(cycles, state->incPC());

			// Add X
			address += state->X;
			cycles++;

			// Read the value at address into A
			state->A = mem->readByte(cycles, address);
			break;
		}
		case ABSOLUTE:
		case ABSOLUTE_X:
		case ABSOLUTE_Y: {
			// Read address from next two bytes (lsb first)
			Byte lsb = mem->readByte(cycles, state->incPC());
			Byte msb = mem->readByte(cycles, state->incPC());
			Byte index = 0;

			// Get index
			if (opCode->B == ABSOLUTE_X) index = state->X;
			else if (opCode->B == ABSOLUTE_Y) index = state->Y;

			lsb += index;		//Doesn't seem to take a cycle?

			//Check for page bouundry
			if (lsb < index) {
				msb++; cycles++;
			}

			// Calculate address and read memory into A
			Word address = (msb << 8) | lsb;
			state->A = mem->readByte(cycles, address);
			break;
		}
		default: {
			//Shouldn't be here!
			fprintf(stderr, "Attempting to use LDA instruction executor for non LDA instruction $%X\n", opCode->code);
			// We won't change the state or use cycles
			return (u8)0;
		}
	}
	LDA::setFlags(state);
	return cycles;
};

/** Called to add LDA Instruction handlers to the emulator */
void LDA::addHandlers(InstructionHandler* handlers[]) {
	handlers[INS_LDA_IMM] = ((InstructionHandler*) new LDA_IMM);
	handlers[INS_LDA_ZP] = ((InstructionHandler*) new LDA_ZP);
	handlers[INS_LDA_ZPX] = ((InstructionHandler*) new LDA_ZPX);
	handlers[INS_LDA_ABS] = ((InstructionHandler*) new LDA_ABS);
	handlers[INS_LDA_ABSX] = ((InstructionHandler*) new LDA_ABSX);
	handlers[INS_LDA_ABSY] = ((InstructionHandler*) new LDA_ABSY);
	handlers[INS_LDA_INDX] = ((InstructionHandler*) new LDA_INDX);
	handlers[INS_LDA_INDY] = ((InstructionHandler*) new LDA_INDY);
};

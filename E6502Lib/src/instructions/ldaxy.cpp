#include "ldaxy.h"

namespace E6502 {
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

				/**
				 Indirect modes are a bit confusing, heres a high level explanation.
			 
				 INDIRECT_X:
					X register is added to argument to determine ZP address
					Word is read from ZP and is used as the actual data address
					Value of mem[Word] is stored in register
					e.g.
						ASSERT state->Y = 0x15
						ASSERT ZP[0x05] = 0x34
						ASSERT ZP[0x06] = 0x12
						LDA_INDX $F0

						$F0 + 0x15 = 0x05 (wrap)
						Word = 0x1234
						save mem[0x1234]

				INDIRECT Y
					Word is read from ZP[arg], Y is then added to Word and this is the
					data address that is used

					eg
						ASSERT state->Y = 0x15
						ASSERT ZP[0xF0] = 0xF0
						ASSERT ZP[0xF1] = 0x65
						LDA_INDY $F0

						Word = ZP[$F0] = 0x65F0
						Word += Y = 0x65F0 + 0x15 = 0x6605
						save mem[0x660f]
				*/

				// Read the next byte as the base for a zero page address.
				Byte baseAddress = mem->readByte(cycles, state->incPC());

				//Add Register if IndirectX
				if (opCode->B == INDIRECT_X) {
					baseAddress += state->X;
					cycles++;
				}
			
				//Read the word from zero page
				Word targetAddress = mem->readWord(cycles, 0x00FF & baseAddress);
			
				//Add Register if IndirectY
				if (opCode->B == INDIRECT_Y) {
					targetAddress += (opCode->B == INDIRECT_X ? 0 : state->Y);
					//Add a cycle iff we crossed a page boundry
					if ((targetAddress & 0x00FF) < state->Y) cycles++;
				}

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
}

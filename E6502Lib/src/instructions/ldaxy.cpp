#include "ldaxy.h"

namespace E6502 {

	/** One function will handle the 'execute' method for all variants */
	u8 LDAXY::executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		switch (opCode->code) {
			case INS_LDA_INDX:
			case INS_LDA_INDY: {

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
				Byte value = mem->readByte(cycles, targetAddress);
				state->saveToRegAndFlag(&state->A, value);
				break;
			}
			
			/* Zero Page Instructions */
			case INS_LDA_ZP:
			case INS_LDX_ZP: 
			case INS_LDY_ZP: {
				/* Read the next byte as the lsb for a zero page address */
				Byte address = mem->readByte(cycles, state->incPC());
				Byte value = mem->readByte(cycles, address);
				switch (opCode->code) {
					case INS_LDA_ZP: state->saveToRegAndFlag(&state->A, value); break;
					case INS_LDX_ZP: state->saveToRegAndFlag(&state->X, value); break;
					case INS_LDY_ZP: state->saveToRegAndFlag(&state->Y, value); break;
					default: {
						fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
						// We won't change the state or use cycles
						return 0;
					}
				}
				break;
			}
			
			/* Immeidate Instructions */
			case INS_LDA_IMM:
			case INS_LDX_IMM:
			case INS_LDY_IMM: {
				/* Read the next byte from PC and put into the appropriate register */
				Byte value = mem->readByte(cycles, state->incPC());
				switch (opCode->code) {
					case INS_LDA_IMM: state->saveToRegAndFlag(&state->A, value); break;
					case INS_LDX_IMM: state->saveToRegAndFlag(&state->X, value); break;
					case INS_LDY_IMM: state->saveToRegAndFlag(&state->Y, value); break;
					default: {
						fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
						// We won't change the state or use cycles
						return 0;
					}
				}
				break;
			}

			case INS_LDA_ZPX:
			case INS_LDX_ZPY:
			case INS_LDY_ZPX: {
				// Read the next byte as the lsb for a zero page base address
				Byte address = mem->readByte(cycles, state->incPC());

				// Add X or Y
				if (opCode->code == INS_LDX_ZPY) address += state->Y;
				else address += state->X;
				cycles++;

				//Read value
				Byte value = mem->readByte(cycles, address);

				// Read the value at address into register
				switch (opCode->code) {
					case INS_LDA_ZPX: state->saveToRegAndFlag(&state->A, value); break;
					case INS_LDX_ZPY: state->saveToRegAndFlag(&state->X, value); break;
					case INS_LDY_ZPX: state->saveToRegAndFlag(&state->Y, value); break;
					default: {
						fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
						// We won't change the state or use cycles
						return 0;
					}
				}
				break;
			}
			
			case INS_LDX_ABS:
			case INS_LDX_ABSY:
			case INS_LDY_ABS:
			case INS_LDY_ABSX:
			case INS_LDA_ABS:
			case INS_LDA_ABSX:
			case INS_LDA_ABSY: {
				// Read address from next two bytes (lsb first)
				Byte lsb = mem->readByte(cycles, state->incPC());
				Byte msb = mem->readByte(cycles, state->incPC());
				Byte index = 0;

				// Get index
				switch (opCode->code) {
					case INS_LDA_ABSX:
					case INS_LDY_ABSX:
						index = state->X;
						break;
					case INS_LDA_ABSY:
					case INS_LDX_ABSY:
						index = state->Y;
						break;
				}
				lsb += index;		//Doesn't seem to take a cycle?

				//Check for page bouundry
				if (lsb < index) {
					msb++; cycles++;
				}

				// Calculate address and read memory into A
				Word address = (msb << 8) | lsb;

				// Read the value at address into register
				Byte value = mem->readByte(cycles, address);
				switch (opCode->code) {
					case INS_LDA_ABS: 
					case INS_LDA_ABSX: 
					case INS_LDA_ABSY: 
						state->saveToRegAndFlag(&state->A, value); 
						break;

					case INS_LDX_ABS: 
					case INS_LDX_ABSY:
						state->saveToRegAndFlag(&state->X, value);
						break;

					case INS_LDY_ABS: 
					case INS_LDY_ABSX:
						state->saveToRegAndFlag(&state->Y, value);
						break;

					default: {
						fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
						// We won't change the state or use cycles
						return 0;
					}
				}
				break;
			}

			default: {
				//Shouldn't be here!
				fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
				// We won't change the state or use cycles
				return (u8)0;
			}
		}
		return cycles;
	};

	/** Called to add LDA Instruction handlers to the emulator */
	void LDAXY::addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_LDA_IMM]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_IMM);
		handlers[INS_LDX_IMM]	= (InstructionHandler*) new LDAXYHandler(INS_LDX_IMM);
		handlers[INS_LDY_IMM]	= (InstructionHandler*) new LDAXYHandler(INS_LDY_IMM);

		handlers[INS_LDA_ZP]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_ZP);
		handlers[INS_LDX_ZP]	= (InstructionHandler*) new LDAXYHandler(INS_LDX_ZP);
		handlers[INS_LDY_ZP]	= (InstructionHandler*) new LDAXYHandler(INS_LDY_ZP);

		handlers[INS_LDA_ZPX]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_ZPX);
		handlers[INS_LDX_ZPY]	= (InstructionHandler*) new LDAXYHandler(INS_LDX_ZPY);
		handlers[INS_LDY_ZPX]	= (InstructionHandler*) new LDAXYHandler(INS_LDY_ZPX);

		handlers[INS_LDA_ABS]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_ABS);
		handlers[INS_LDX_ABS]	= (InstructionHandler*) new LDAXYHandler(INS_LDX_ABS);
		handlers[INS_LDY_ABS]	= (InstructionHandler*) new LDAXYHandler(INS_LDY_ABS);

		handlers[INS_LDA_ABSX]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_ABSX);
		handlers[INS_LDA_ABSY]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_ABSY);
		handlers[INS_LDX_ABSY]	= (InstructionHandler*) new LDAXYHandler(INS_LDX_ABSY);
		handlers[INS_LDY_ABSX]	= (InstructionHandler*) new LDAXYHandler(INS_LDY_ABSX);


		handlers[INS_LDA_INDX]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_INDX);
		handlers[INS_LDA_INDY]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_INDY);
	};
}

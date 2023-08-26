#pragma once
#include "../types.h"
#include <stdio.h>

/** LDA Instruction Codes */
const static Byte INS_LDA_INDX = 0xA1;
const static Byte INS_LDA_ZP = 0xA5;
const static Byte INS_LDA_IMM = 0xA9;
const static Byte INS_LDA_ABS = 0xAD;
const static Byte INS_LDA_INDY = 0xB1;
const static Byte INS_LDA_ZPX = 0xB5;
const static Byte INS_LDA_ABSY = 0xB9;
const static Byte INS_LDA_ABSX = 0xBD;

#ifndef LDA
#define LDA
namespace LDA {
	/** LDA Adressing Modes */
	const static Byte INDIRECT_X	= 0b000; // 101 000 01
	const static Byte ZERO_PAGE		= 0b001; // 101 001 01
	const static Byte IMMEDIATE		= 0b010; // 101 010 01
	const static Byte ABSOLUTE		= 0b011; // 101 011 01
	const static Byte INDIRECT_Y	= 0b100; // 101 100 01
	const static Byte ZERO_PAGE_X	= 0b101; // 101 101 01
	const static Byte ABSOLUTE_Y	= 0b110; // 101 110 01
	const static Byte ABSOLUTE_X	= 0b111; // 101 111 01


	Byte instructions[] = { INS_LDA_IMM, INS_LDA_ZP, INS_LDA_ZPX,INS_LDA_ABS,
							INS_LDA_ABSX,INS_LDA_ABSY,INS_LDA_INDX,INS_LDA_INDY };


	/* Helper method to set the CPU flags. Only N(7) and Z(2) flags are affeted by LDA */
	void setFlags(CPUState* state) {
		Byte flags = state->getFlags() & 0b01011101;					// Get all the flags except N & Z
		flags |= (state->A & 0x80) | (state->A == 0 ? 0x02 : 0x00);		// Set N & Z Flags
		state->setFlags(flags);											// Update state
	}

	/** One function will handle the 'execute' method for all variants */
	insHandlerFn LDAInstructionHandler = [](Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		switch (opCode->B) {
			case INDIRECT_X: {

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
			case ABSOLUTE: {
				// Read address from next two bytes (lsb first)
				Byte lsb = mem->readByte(cycles, state->incPC());
				Byte msb = mem->readByte(cycles, state->incPC());

				// Calculate address and read memory into A
				Word address = (msb << 8) | lsb;
				state->A = mem->readByte(cycles, address);
				break;
			}
			case INDIRECT_Y: {

			}
			case ZERO_PAGE_X: {
				/* Read the next byte as the lsb for a zero page base address */
				Byte address = mem->readByte(cycles, state->incPC());

				/* Add X */
				address += state->X;
				cycles++;

				/* Read the value at address into A */
				state->A = mem->readByte(cycles, address);
				break;
			}
			case ABSOLUTE_Y: {

			}
			case ABSOLUTE_X: {

			}
			default: {
				//Shouldn't be here!
				fprintf(stderr, "Attempting to use LDA instruction executor for non LDA instruction $%X\n", opCode->code);
				// We won't change the state or use cycles
				return (u8)0;
		    }
		}
		setFlags(state);
		return cycles;
	};

	/** Defines proprties common to all LDA instructions */
	struct BASE : public InstructionHandler {
		BASE() {
			isLegal = true;
			execute = LDAInstructionHandler;
		}
	};

	/** Defines properties for LDA Immediate instruction */
	struct LDA_IMM : public BASE {
		LDA_IMM() {
			opcode = INS_LDA_IMM;
			name = "LDA - Load Accumulator [Immediate]";
		}
	};

	/** Defines properties for LDA ZeroPage instruction */
	struct LDA_ZP : public BASE {
		LDA_ZP() {
			opcode = INS_LDA_ZP;
			name = "LDA - Load Accumulator [ZeroPage]";
		}
	};

	/** Defines properties for LDA ZeroPage(X) instruction */
	struct LDA_ZPX : public BASE {
		LDA_ZPX() {
			opcode = INS_LDA_ZPX;
			name = "LDA - Load Accumulator [ZeroPage-X]";
		}
	};

	/** Defines properties for LDA Absolute instruction */
	struct LDA_ABS : public BASE {
		LDA_ABS() {
			opcode = INS_LDA_ABS;
			name = "LDA - Load Accumulator [Absolute]";
		}
	};

	/** Defines properties for LDA Absolute(X) instruction */
	struct LDA_ABSX : public BASE {
		LDA_ABSX() {
			opcode = INS_LDA_ABSX;
			name = "LDA - Load Accumulator [Absolute-X]";
		}
	};

	/** Defines properties for LDA Absolute(Y) instruction */
	struct LDA_ABSY : public BASE {
		LDA_ABSY() {
			opcode = INS_LDA_ABSY;
			name = "LDA - Load Accumulator [Absolute-Y]";
		}
	};

	/** Defines properties for LDA Indirect(X) instruction */
	struct LDA_INDX : public BASE {
		LDA_INDX() {
			opcode = INS_LDA_INDX;
			name = "LDA - Load Accumulator [Indirect-X]";
		}
	};

	/** Defines properties for LDA Indirect(Y) instruction */
	struct LDA_INDY : public BASE {
		LDA_INDY() {
			opcode = INS_LDA_INDY;
			name = "LDA - Load Accumulator [Indirect-Y]";
		}
	};

	/** Called to add LDA Instruction handlers to the emulator */
	static void addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_LDA_IMM] = ((InstructionHandler*) new LDA_IMM);
		handlers[INS_LDA_ZP] = ((InstructionHandler*) new LDA_ZP);
		handlers[INS_LDA_ZPX] = ((InstructionHandler*) new LDA_ZPX);
		handlers[INS_LDA_ABS] = ((InstructionHandler*) new LDA_ABS);
		handlers[INS_LDA_ABSX] = ((InstructionHandler*) new LDA_ABSX);
		handlers[INS_LDA_ABSY] = ((InstructionHandler*) new LDA_ABSY);
		handlers[INS_LDA_INDX] = ((InstructionHandler*) new LDA_INDX);
		handlers[INS_LDA_INDY] = ((InstructionHandler*) new LDA_INDY);
		
	}
}
#endif 

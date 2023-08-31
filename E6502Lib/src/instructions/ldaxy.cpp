#include "ldaxy.h"

namespace E6502 {

	/** Handles Immediate Addressing Mode Instructions */
	u8 LDAXY::immediateHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = getRegFromInstruction(opCode, state);
		
		/* Read the next byte from PC and put into the appropriate register */
		Byte value = mem->readByte(cycles, state->incPC());
		state->saveToRegAndFlag(saveRegister, value);
		return cycles;
	}

	/** Handles ZeroPage Addressing Mode Instructions */
	u8 LDAXY::zeroPageHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = getRegFromInstruction(opCode, state);

		/* Read the next byte as the lsb for a zero page address */
		Byte address = mem->readByte(cycles, state->incPC());

		/* Get and store the value */
		Byte value = mem->readByte(cycles, address);
		state->saveToRegAndFlag(saveRegister, value);

		return cycles;
	}/** Handles ZeroPage Addressing Mode Instructions */

	/** Handles ZeroPageIndexed Addressing Mode Instructions */
	u8 LDAXY::zeroPageIndexedHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = getRegFromInstruction(opCode, state);

		// Read the next byte as the lsb for a zero page base address
		Byte address = mem->readByte(cycles, state->incPC());

		// Add X or Y
		if (opCode->code == INS_LDX_ZPY) address += state->Y;
		else address += state->X;
		cycles++;

		//Read value
		Byte value = mem->readByte(cycles, address);

		// Read the value at address into register
		state->saveToRegAndFlag(saveRegister, value);
		return cycles;
	}

	/** Handles Absolute and Absolute Indexed Addressing Mode Instructions */
	u8 LDAXY::absoluteHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = getRegFromInstruction(opCode, state);
		
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
		Byte value = mem->readByte(cycles, address);
		state->saveToRegAndFlag(saveRegister, value);

		return cycles;
	}

	/** One function will handle the 'execute' method for all variants */
	u8 LDAXY::executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = nullptr;

		//Last 2 bits of opcode indicates target register
		switch (opCode->C & 0x03) {
			case 0x00: saveRegister = &state->Y; break;
			case 0x01: saveRegister = &state->A; break;
			case 0x02: saveRegister = &state->X; break;
		}
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
			default: {
				//Shouldn't be here!
				fprintf(stderr, "Attempting to use LD(AXY) instruction executor for non LD(AXY) instruction $%X\n", opCode->code);
				// We won't change the state or use cycles
				return (u8)0;
			}
		}
		return cycles;
	};

	/** Function to get a pointer to a register in the state based on opcode */
	Byte* LDAXY::getRegFromInstruction(InstructionCode* instruction, CPUState* state) {
		//Last 2 bits of opcode indicates target register
		switch (instruction->C & 0x03) {
			case 0x00: return &state->Y;
			case 0x01: return &state->A;;
			case 0x02: return &state->X;
		}
		//TODO error handling
		return nullptr;
	}

	/** Helper method to get a value from memory and store in a register */
	void LDAXY::fetchAndSaveToRegister(u8* cycles, Memory* memory, CPUState* state, Word address, Byte* reg) {
		Byte value = memory->readByte(*cycles, address);
		state->saveToRegAndFlag(reg, value);
	}

	/** Called to add LDA Instruction handlers to the emulator */
	void LDAXY::addHandlers(InstructionHandler* handlers[]) {

		for (InstructionHandler handler : LDAXY::instructions) {
			handlers[handler.opcode] = new InstructionHandler{handler.opcode, handler.isLegal, handler.name, handler.execute};
		}

		handlers[INS_LDA_INDX]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_INDX);
		handlers[INS_LDA_INDY]	= (InstructionHandler*) new LDAXYHandler(INS_LDA_INDY);
	};
}

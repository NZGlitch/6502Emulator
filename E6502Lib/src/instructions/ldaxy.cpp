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

	/** Handles Indirect Addressing Modes */
	u8 LDAXY::indirectHandler(Memory* mem, CPUState* state, InstructionCode* opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		Byte* saveRegister = getRegFromInstruction(opCode, state);

		// Read the next byte as the base for a zero page address.
		Byte baseAddress = mem->readByte(cycles, state->incPC());

		//Add Register if IndirectX
		if (opCode->code == INS_LDA_INDX) {
			baseAddress += state->X;
			cycles++;
		}
			
		//Read the word from zero page
		Word targetAddress = mem->readWord(cycles, 0x00FF & baseAddress);
			
		//Add Register if IndirectY
		if (opCode->code == INS_LDA_INDY) {
			targetAddress += state->Y;
			//Add a cycle iff we crossed a page boundry
			if ((targetAddress & 0x00FF) < state->Y) cycles++;
		}

		Byte value = mem->readByte(cycles, targetAddress);
		state->saveToRegAndFlag(&state->A, value);
		
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
	};
}

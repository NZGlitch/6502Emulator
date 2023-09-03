#include "instruction_utils.h"
#include "load_instruction.h"

namespace E6502 {

	/** Handles Immediate Addressing Mode Instructions */
	u8 LoadInstruction::immediateHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		u8 saveRegister = InstructionUtils::getRegFromInstruction(opCode, cpu);
		
		// Read the next byte from PC and put into the appropriate register
		Byte value = cpu->readPCByte(cycles);
		cpu->saveToRegAndFlag(cycles, saveRegister, value);
		return cycles;
	}

	/** Handles ZeroPage Addressing Mode Instructions */
	u8 LoadInstruction::zeroPageHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		u8 saveRegister = InstructionUtils::getRegFromInstruction(opCode, cpu);

		// Read the next byte as the lsb for a zero page address
		Byte address = cpu->readPCByte(cycles);

		// Get and store the value
		Byte value = cpu->readByte(cycles, address);
		cpu->saveToRegAndFlag(cycles, saveRegister, value);
		return cycles;
	}

	/** Handles ZeroPageIndexed Addressing Mode Instructions */
	u8 LoadInstruction::zeroPageIndexedHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		u8 saveRegister = InstructionUtils::getRegFromInstruction(opCode, cpu);

		// Read the next byte as the lsb for a zero page base address
		Byte address = 0x00FF & cpu->readPCByte(cycles);

		// Add X or Y
		if (opCode == INS_LDX_ZPY.opcode) address += cpu->regValue(cycles, CPU::REGISTER_Y);
		else address += cpu->regValue(cycles, CPU::REGISTER_X);
		cycles++;

		// Read the value at address into register
		Byte value = cpu->readByte(cycles, address);
		cpu->saveToRegAndFlag(cycles, saveRegister, value);
		return cycles;
	}

	/** Handles Absolute and Absolute Indexed Addressing Mode Instructions */
	u8 LoadInstruction::absoluteHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		u8 saveRegister = InstructionUtils::getRegFromInstruction(opCode, cpu);
		
		// Read address from next two bytes (lsb first)
		Byte lsb = cpu->readPCByte(cycles);
		Byte msb = cpu->readPCByte(cycles);
		Byte index = 0;

		// Get index
		switch (opCode) {
			case INS_LDA_ABSX.opcode:
			case INS_LDY_ABSX.opcode:
				index = cpu->regValue(cycles, CPU::REGISTER_X);
				break;
			case INS_LDA_ABSY.opcode:
			case INS_LDX_ABSY.opcode:
				index = cpu->regValue(cycles, CPU::REGISTER_Y);
				break;
		}

		lsb += index;		//Doesn't seem to take a cycle?

		//Check for page bouundry
		if (lsb < index) {
			msb++; cycles++;
		}

		// Calculate address and read memory into A
		Word address = (msb << 8) | lsb;
		Byte value = cpu->readByte(cycles, address);
		cpu->saveToRegAndFlag(cycles, saveRegister, value);
		return cycles;
	}

	/** Handles Indirect Addressing Modes */
	u8 LoadInstruction::indirectHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle
		u8 saveRegister = InstructionUtils::getRegFromInstruction(opCode, cpu);

		// Read the next byte as the base for a zero page address.
		Byte baseAddress = cpu->readPCByte(cycles);

		// Add Register if IndirectX
		if (opCode == INS_LDA_INDX.opcode) {
			baseAddress += cpu->regValue(cycles, CPU::REGISTER_X);
			cycles++;
		}
			
		// Read the word from zero page
		Word targetAddress = cpu->readWord(cycles, 0x00FF & baseAddress);
			
		// Add Register if IndirectY
		if (opCode == INS_LDA_INDY.opcode) {
			targetAddress += cpu->regValue(cycles, CPU::REGISTER_Y);
			if ((targetAddress & 0x00FF) < cpu->regValue(cycles, CPU::REGISTER_Y)) cycles++; // Add a cycle iff we crossed a page boundry
		}

		// Save value
		Byte value = cpu->readByte(cycles, targetAddress);
		cpu->saveToRegAndFlag(cycles, CPU::REGISTER_A, value);
		return cycles;
	};

	/** Helper method to get a value from memory and store in a register */
	void LoadInstruction::fetchAndSaveToRegister(u8* cycles, CPU* cpu, Word address, u8 reg) {
		Byte value = cpu->readByte(*cycles, address);
		cpu->saveToRegAndFlag(*cycles, reg, value);
	}

	/** Called to add Load Instruction handlers to the emulator */
	void LoadInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : LOAD_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{handler.opcode, handler.isLegal, handler.name, handler.execute};
		}
	};
}

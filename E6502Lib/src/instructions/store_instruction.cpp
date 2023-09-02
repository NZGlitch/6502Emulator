#include "instruction_utils.h"
#include "store_instruction.h"

namespace E6502 {
	/** Absolute and Absolute-Indexed instructions */
	u8 StoreInstruction::absoluteHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;	// 1 cycle to load instruction
		
		// Read address from next two bytes (lsb first)
		Word address = mem->readWord(cycles, state->incPC());
		state->incPC();

		// If using an indexed mode, apply the index to the address
		if (opCode == INS_STA_ABSX.opcode || opCode == INS_STA_ABSY.opcode) {
			address += (opCode == INS_STA_ABSX ? state->X : state->Y);
			cycles++;	//Index mode always uses 5 cycles
		}
	
		// Get the value from the source register
		Byte value = *InstructionUtils::getRegFromInstruction(opCode, state);

		// Write it to memory
		mem->writeByte(cycles, address, value);

		return cycles;
	};

	/** Zero Page instructions */
	u8 StoreInstruction::zeroPageHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;	// 1 cycle to load instruction

		// Read zero page address from next byte
		Word address = 0x00FF & mem->readByte(cycles, state->incPC());

		// Get the value from the source register
		Byte value = *InstructionUtils::getRegFromInstruction(opCode, state);

		// Store in memory
		mem->writeByte(cycles, address, value);

		return cycles;
	}

	/** Zero Page mode instructions */
	u8 StoreInstruction::zeroPageIndexedHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;

		// Base address
		Word address = mem->readByte(cycles, state->incPC());

		// Add Index
		switch (opCode) {
		case INS_STA_ZPX.opcode:
		case INS_STY_ZPX.opcode:
			address += state->X;
			cycles++;
			break;
		case INS_STX_ZPY.opcode:
			address += state->Y;
			cycles++;
			break;
		}

		// Align to zero page and get value
		address = 0x00FF & address;
		Byte value = *InstructionUtils::getRegFromInstruction(opCode, state);

		// Store value and return
		mem->writeByte(cycles, address, value);
		return cycles;
	}

	/** Add store instructions to handlers array */
	void StoreInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STORE_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{ handler.opcode, handler.isLegal, handler.name, handler.execute };
		}
	}
}

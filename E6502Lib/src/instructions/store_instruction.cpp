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

	/** Zero Page Indexed instructions */
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

	/** X-Indexed Zerp Page Indirect instructions */
	u8 StoreInstruction::indirectXHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;

		// Calculate ZP Address
		Word zpAddress = mem->readByte(cycles, state->incPC());
		zpAddress = (zpAddress + state->X) & 0x00FF; cycles++;

		// Calculate Target Address
		Word targetAddress = mem->readWord(cycles, zpAddress);
		Byte value = *InstructionUtils::getRegFromInstruction(opCode, state);

		// Write and save
		mem->writeByte(cycles, targetAddress, value);
		return cycles;
	}

	/** Zero PAge Y-Indexed Indirect instructions */
	u8 StoreInstruction::indirectYHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;

		// Calculate ZP Address
		Word zpAddr = mem->readByte(cycles, state->incPC());

		// Caclulcate target Address
		Word targetAddr = mem->readWord(cycles, zpAddr);
		targetAddr = (targetAddr & 0xFF00) | ((targetAddr + state->Y) & 0x00FF); cycles++;	// Do not allow carry to affect high 8 bits
		Byte value = state->A;

		// Write and Save
		mem->writeByte(cycles, targetAddr, value);
		return cycles;
	}

	/** Add store instructions to handlers array */
	void StoreInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STORE_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{ 
				handler.opcode, handler.isLegal, handler.name, handler.execute 
			};
		}
	}
}

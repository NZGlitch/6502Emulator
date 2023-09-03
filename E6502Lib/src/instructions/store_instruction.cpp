#include "instruction_utils.h"
#include "store_instruction.h"

namespace E6502 {
	/** Absolute and Absolute-Indexed instructions */
	void StoreInstruction::absoluteHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Read address from next two bytes (lsb first)
		Word address = cpu->readPCWord(cycles);
		

		// If using an indexed mode, apply the index to the address
		if (opCode == INS_STA_ABSX.opcode || opCode == INS_STA_ABSY.opcode) {
			address += (opCode == INS_STA_ABSX ? cpu->regValue(cycles, CPU::REGISTER_X) : cpu->regValue(cycles, CPU::REGISTER_Y));
			cycles++;	//Index mode always uses 5 cycles
		}
	
		// Get the value from the source register
		Byte value = cpu->regValue(cycles, InstructionUtils::getRegFromInstruction(opCode, cpu));

		// Write it to memory
		cpu->writeByte(cycles, address, value);
	};

	/** Zero Page instructions */
	void StoreInstruction::zeroPageHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Read zero page address from next byte
		Word address = 0x00FF & cpu->readPCByte(cycles);

		// Get the value from the source register
		Byte value = cpu->regValue(cycles, InstructionUtils::getRegFromInstruction(opCode, cpu));

		// Store in memory
		cpu->writeByte(cycles, address, value);
	}

	/** Zero Page Indexed instructions */
	void StoreInstruction::zeroPageIndexedHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Base address
		Word address = 0x00FF & cpu->readPCByte(cycles);

		// Add Index
		switch (opCode) {
		case INS_STA_ZPX.opcode:
		case INS_STY_ZPX.opcode:
			address += cpu->regValue(cycles, CPU::REGISTER_X);
			cycles++;
			break;
		case INS_STX_ZPY.opcode:
			address += cpu->regValue(cycles, CPU::REGISTER_Y);
			cycles++;
			break;
		}

		// Align to zero page and get value
		address = 0x00FF & address;
		Byte value = cpu->regValue(cycles, InstructionUtils::getRegFromInstruction(opCode, cpu));

		// Store value and return
		cpu->writeByte(cycles, address, value);
	}

	/** X-Indexed Zero Page Indirect instructions */
	void StoreInstruction::indirectXHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Calculate ZP Address
		Word zpAddress = 0x00FF & cpu->readPCByte(cycles);
		zpAddress = (zpAddress + cpu->regValue(cycles, CPU::REGISTER_X)) & 0x00FF; cycles++;

		// Calculate Target Address
		Word targetAddress = cpu->readWord(cycles, zpAddress);
		Byte value = cpu->regValue(cycles, InstructionUtils::getRegFromInstruction(opCode, cpu));

		// Write and save
		cpu->writeByte(cycles, targetAddress, value);
	}

	/** Zero Page Y-Indexed Indirect instructions */
	void StoreInstruction::indirectYHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Calculate ZP Address
		Word zpAddr = 0x00FF & cpu->readPCByte(cycles);

		// Caclulcate target Address
		Word targetAddr = cpu->readWord(cycles, zpAddr);
		targetAddr = (targetAddr & 0xFF00) | ((targetAddr + cpu->regValue(cycles, CPU::REGISTER_Y)) & 0x00FF); cycles++;	// Do not allow carry to affect high 8 bits
		Byte value = cpu->regValue(cycles, CPU::REGISTER_A);

		// Write and Save
		cpu->writeByte(cycles, targetAddr, value);
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

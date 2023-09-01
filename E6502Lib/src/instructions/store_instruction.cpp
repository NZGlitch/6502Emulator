#include "instruction_utils.h"
#include "store_instruction.h"

namespace E6502 {
	u8 StoreInstruction::absoluteHandler(Memory* mem, CPUState* state, Byte opCode) {
		u8 cycles = 1;	// 1 cycle to load instruction
		
		// Read address from next two bytes (lsb first)
		Word address = mem->readWord(cycles, state->incPC());
		state->incPC();

		// If using an indexed mode, apply the index to the address
		if (opCode == INS_STA_ABSX || opCode == INS_STA_ABSY) {
			address += (opCode == INS_STA_ABSX ? state->X : state->Y);
			cycles++;	//Index mode always uses 5 cycles
		}
	
		// Get the value from the source register
		Byte value = *InstructionUtils::getRegFromInstruction(opCode, state);

		// Write it to memory
		mem->writeByte(cycles, address, value);

		return cycles;
	};

	/** Add store instructions to handlers array */
	void StoreInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : STORE_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{ handler.opcode, handler.isLegal, handler.name, handler.execute };
		}
	}
}

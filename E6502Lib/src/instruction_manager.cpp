#pragma once
#include "instruction_manager.h"

namespace E6502 {
	InstructionManager::InstructionManager(InstructionLoader* loader) {
		//Initialise all handlers to be NOPs
		for (u16 i = 0; i <= 0xFF; i++) {
			this->handlers[i] = &this->defaultHandler;
		}

		// TODO replace with properly defined NOP
		this->handlers[INS_NOP.opcode] = &INS_NOP;

		loader->load(handlers);

	}

	InstructionHandler* InstructionManager::operator[](Byte instruction) {
		return this->handlers[instruction];
	}
}

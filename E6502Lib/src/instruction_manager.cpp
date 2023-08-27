#pragma once
#include "instruction_manager.h"

InstructionManager::InstructionManager(InstructionUtils::InstructionLoader* loader) {
	//Initialise all handlers to be NOPs
	for (u16 i = 0; i <= 0xFF; i++) {
		this->handlers[i] = &this->defaultHandler;
	}

	//Add known Instructions
	loader->load(handlers);
}

InstructionHandler* InstructionManager::operator[](Byte instruction) {
	return this->handlers[instruction];
}

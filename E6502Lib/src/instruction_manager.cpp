#pragma once
#include "instruction_manager.h"

InstructionManager::InstructionManager() {
	initInstructionHandlers();
}

//Needs to be called on initialisation of program
void InstructionManager::initInstructionHandlers() {
	//Initialise all handlers to be NOPs

	for (u16 i = 0; i <= 0xFF; i++) {
		this->handlers[i] = &this->defaultHandler;
	}
}
const InstructionHandler* InstructionManager::operator[](Byte instruction) {
	return this->handlers[instruction];
}

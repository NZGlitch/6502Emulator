#pragma once
#include "instruction_handler.h"

/*
Instruction Manager
-------------------
6502 has 256 potential opcodes - see this chart for deets: 
https://www.masswerk.at/nowgobang/2021/6502-illegal-opcodes
*/

// Op Codes 
static constexpr Byte INS_NOP = 0xEA;

class InstructionManager {
private:
	//Handler Matrix
	const InstructionHandler* handlers[0x100] = {};

	//Needs to be called on initialisation of program
	void initInstructionHandlers();

public:
	//Default handler for undefined instructions
	const InstructionHandler defaultHandler{ 0xEA, false, "Unsupported OP", [](Memory* mem) { return 0;} };

	//Constructor
	InstructionManager();

	//Array read access
	const InstructionHandler* operator[](Byte instruction);
};

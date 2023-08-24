#pragma once
#include "instructions/instruction_utils.h"
#include "instruction_handler.h"

/*
Instruction Manager
-------------------
6502 has 256 potential opcodes - see this chart for deets: 
https://www.masswerk.at/nowgobang/2021/6502-illegal-opcodes
*/

// Op Codes 
static constexpr Byte INS_NOP = 0xEA;		//NOP

class InstructionManager {
private:
	//Handler Matrix
	InstructionHandler* handlers[0x100] = {};

public:
	//Default handler for undefined instructions
	InstructionHandler defaultHandler{ 0xEA, false, "Unsupported OP", [](Memory* mem) { return 0;} };

	//Constructor
	InstructionManager(InstructionUtils::InstructionLoader* loader);

	//Array read access
	InstructionHandler* operator[](Byte instruction);
};

/**
* InstructionManager deals with handing execution to handlers defined in the instructions subdir
*/ 
#pragma once
#include "types.h"
#include "instructions/instruction_utils.h"

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
	InstructionHandler defaultHandler{ 0xEA, false, "Unsupported OP", [](Memory* mem, CPUState* state, InstructionCode*) { return (u8)0;} };

	//Constructor
	InstructionManager(InstructionUtils::InstructionLoader* loader);

	//Array read access
	InstructionHandler* operator[](Byte instruction);
};

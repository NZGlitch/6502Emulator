/**
* InstructionManager deals with handing execution to handlers defined in the instructions subdir
*/ 
#pragma once
#include "types.h"
#include "instruction_handler.h"


namespace E6502 {

	// Default executor for unimplemented instructions
	static InstructionHandler INS_NOP = { 0xEA, true, "NOP - Unimplemented", [](CPU* cpu, u8& cycles, Byte instruction) { cycles++; } };

	/* Manages instruction definitions and executors	*/
	class InstructionManager {
	private:
		// Handler Matrix
		InstructionHandler* handlers[0x100] = {};

	public:
		// Default handler for undefined instructions
		InstructionHandler defaultHandler{ 0xEA, false, "Unsupported OP", [](CPU* cpu, u8& cycles, Byte instruction) { cycles++; } };

		// Constructor
		InstructionManager(InstructionLoader* loader);

		// Array read access
		InstructionHandler* operator[](Byte instruction);
	};
}

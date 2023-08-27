// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "types.h"
#include "instruction_manager.h"

namespace E6502 {
	class CPU {

	private:
		InstructionManager* insManager;

	public:
		/* The current state of the CPU */
		CPUState* currentState;

		/** Constructor */
		CPU(CPUState* initSate);

		/* Resets the CPU state - Until this is called, CPU state is undefined */
		void reset(Memory* memory);

		/* Helper method, allows setting all flags at once */
		void setFlags(u8 flags);

		/* Allows getting all flags in a single byte */
		u8 getFlags();

		/*
		 * Execute <numInstructions> instructions. Return the number of cycles used.
		 */
		u8 execute(u8 numInstructions, Memory* memory);
	};
}
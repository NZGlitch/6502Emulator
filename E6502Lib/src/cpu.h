// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "types.h"
#include "instruction_manager.h"

namespace E6502 {
	class CPU {

	private:
		InstructionManager* insManager;
		Memory* mainMemory;

	public:
		/* The current state of the CPU */
		CPUState* currentState;

		/** Constructor */
		CPU(CPUState* initSate, Memory* initMemory, InstructionLoader* loader);

		/* Resets the CPU state - Until this is called, CPU state is undefined */
		void reset();

		/* Helper method, allows setting all flags at once */
		void setFlags(u8 flags);

		/* Allows getting all flags in a single byte */
		u8 getFlags();

		/** Allows an instruction to read a Byte from memory, uses 1 cycle */
		Byte readByte(u8& cycles, Word address);

		/** Allows an instruction to read a word from memory (Little endiean), uses 2 cycles*/
		Word readWord(u8& cycles, Word address);

		/** Allows an instruction to wite a word to memory (Little endiean), uses 2 cycles */
		void writeByte(u8& cycles, Word address, Byte value);

		/* Execute <numInstructions> instructions. Return the number of cycles used. */
		u8 execute(u8 numInstructions);
	};
}

// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "types.h"
#include "memory.h"
#include "instruction_manager.h"

namespace E6502 {
	class CPU {

	private:
		InstructionManager* insManager;
		Memory* mainMemory;
		CPUState* currentState;


	public:

		constexpr static u8 REGISTER_A = 0;
		constexpr static u8 REGISTER_X = 1;
		constexpr static u8 REGISTER_Y = 2;

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

		/** returns mem[PC++]  */
		Byte readPCByte(u8& cycles);

		/** calls readPCByte twice, constructs word (assumes little endian) */
		Word readPCWord(u8 &cycles);

		/* Tells the CPU to save a value to a register and set flags according */
		virtual void saveToRegAndFlag(u8& cycles, u8 reg, Byte value);

		/* Returns the value currently held in the specified register */
		virtual Byte regValue(u8& cycles, u8 reg);

		/* Push the current value of the program counter to the stack */
		void pushPCToStack(u8& cycles);

		/* Pops a word from the stack */
		Word popStackWord(u8& cycles);
		
		/* Set the program counter to the specified value */
		void setPC(u8& cycles, Word address);

		/* Execute <numInstructions> instructions. Return the number of cycles used. */
		u8 execute(u8 numInstructions);

	};
}

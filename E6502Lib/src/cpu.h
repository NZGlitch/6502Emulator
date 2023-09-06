// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once
#include "types.h"
#include "memory.h"
#include "instruction_manager.h"

namespace E6502 {

	/** 
	 * Virtual class represents CPU ops that may be accessed by instructions 
	 * All methods must take a u8&cycles parameter and increment this to reflect
	 * the cycles used in the operation requested
	 */
	class CPU {
	
	public:

		constexpr static u8 REGISTER_A = 0;
		constexpr static u8 REGISTER_X = 1;
		constexpr static u8 REGISTER_Y = 2;

		/** Allows an instruction to read a Byte from memory */
		virtual Byte readByte(u8& cycles, Word address) = 0;

		/** Allows an instruction to read a word from memory */
		virtual Word readWord(u8& cycles, Word address) = 0;

		/** Allows an instruction to wite a word to memory */
		virtual void writeByte(u8& cycles, Word address, Byte value) = 0;

		/** returns mem[PC++]  */
		virtual Byte readPCByte(u8& cycles) = 0;

		/** calls readPCByte twice, constructs word */
		virtual Word readPCWord(u8& cycles) = 0;

		/* Tells the CPU to save a value to a register and set flags according */
		virtual void saveToRegAndFlag(u8& cycles, u8 reg, Byte value) = 0;

		/* Copy the stack pointer to register X */
		virtual void copyStackToXandFlag(u8& cycles) = 0;

		/* Copy X register to stack pointer */
		virtual void copyXtoStack(u8& cycles) = 0;

		/* Returns the value currently held in the specified register */
		virtual Byte regValue(u8& cycles, u8 reg) = 0;

		/* Push the current value of the program counter to the stack */
		virtual void pushPCToStack(u8& cycles) = 0;

		/* Pops a word from the stack */
		virtual Word popStackWord(u8& cycles) = 0;

		/* Set the program counter to the specified value */
		virtual void setPC(u8& cycles, Word address) = 0;
	};

	
	/* This represents CPU with additional methods for emulation management - should not be used by instructions */
	class CPUInternal : public CPU {

	private:
		InstructionManager* insManager;
		Memory* mainMemory;
		CPUState* currentState;

	public:
		/** Constructor */
		CPUInternal(CPUState* initSate, Memory* initMemory, InstructionLoader* loader);

		/* Execute <numInstructions> instructions. Return the number of cycles used. */
		u8 execute(u8 numInstructions);

		/* Allows getting all flags in a single byte */
		u8 getFlags();

		/* Pops the most recently added item from the stack */
		Byte popByteFromStack();

		/* Resets the CPU state - Until this is called, CPU state is undefined */
		void reset();

		/* Helper method, allows setting all flags at once */
		void setFlags(u8 flags);

		/** CPU Overrides */
		virtual Byte readByte(u8& cycles, Word address);
		virtual Word readWord(u8& cycles, Word address);
		virtual void writeByte(u8& cycles, Word address, Byte value);
		virtual Byte readPCByte(u8& cycles);
		virtual Word readPCWord(u8& cycles);
		virtual void saveToRegAndFlag(u8& cycles, u8 reg, Byte value);
		virtual Byte regValue(u8& cycles, u8 reg);
		virtual void pushPCToStack(u8& cycles);
		virtual Word popStackWord(u8& cycles);
		virtual void setPC(u8& cycles, Word address);
		virtual void copyStackToXandFlag(u8& cycles);
		virtual void copyXtoStack(u8& cycles);
	};
}

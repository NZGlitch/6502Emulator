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

		/** Allows an instruction to wite a word to memory */
		virtual void writeByte(u8& cycles, Word address, Byte value) = 0;


		/** Allows an instruction to read a word from memory */
		virtual Word readWord(u8& cycles, Word address) = 0;


		/** returns mem[PC++]  */
		virtual Byte readPCByte(u8& cycles) = 0;
		
		/** calls readPCByte twice, constructs word */
		virtual Word readPCWord(u8& cycles) = 0;


		/* Tells the CPU to save a value to a register and set flags according */
		virtual void saveToRegAndFlag(u8& cycles, u8 reg, Byte value) = 0;
		
		/* Returns the value currently held in the specified register */
		virtual Byte regValue(u8& cycles, u8 reg) = 0;


		/* Push a bute onto the stack */
		virtual void pushStackByte(u8& cycles, Byte value) = 0;
		
		/* Push a word onto the stack */
		virtual void pushStackWord(u8& cycles, Word value) = 0;

		/* Pull a byte of the stack */
		virtual Byte pullStackByte(u8& cycles) = 0;

		/* Pops a word from the stack */
		virtual Word pullStackWord(u8& cycles) = 0;


		/* Get a the current status flags for the CPU */
		virtual FlagUnion getFlags(u8& cycles) = 0;

		/* Set the cpu status flags */
		virtual void setFlags(u8& cycles, FlagUnion flags) = 0;


		/* Get the current PC */
		virtual Word getPC(u8& cycles) = 0;

		/* Set the program counter to the specified value */
		virtual void setPC(u8& cycles, Word address) = 0;


		/* Get thecurrent value of the Stack Pointer */
		virtual Byte getSP(u8& cycles) = 0;

		/* Set the SP register to the specified value */
		virtual void setSP(u8& cycles, Byte value) = 0;
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

		/* Resets the CPU state - Until this is called, CPU state is undefined */
		void reset();

		/** CPU Overrides */
		virtual Byte readByte(u8& cycles, Word address);
		virtual void writeByte(u8& cycles, Word address, Byte value);

		virtual Word readWord(u8& cycles, Word address);
		
		virtual Byte readPCByte(u8& cycles);
		virtual Word readPCWord(u8& cycles);

		virtual void saveToRegAndFlag(u8& cycles, u8 reg, Byte value);
		virtual Byte regValue(u8& cycles, u8 reg);

		
		virtual Word getPC(u8& cycles);
		virtual void setPC(u8& cycles, Word address);

		virtual void pushStackByte(u8& cycles, Byte value);
		virtual void pushStackWord(u8& cycles, Word value);
		virtual Byte pullStackByte(u8& cycles);
		virtual Word pullStackWord(u8& cycles);

		virtual FlagUnion getFlags(u8& cycles);
		virtual void setFlags(u8& cycles, FlagUnion);

		virtual Byte getSP(u8& cycles);
		virtual void setSP(u8& cycles, Byte value);
	};
}

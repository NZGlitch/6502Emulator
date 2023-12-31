﻿// main.h : Include file for standard system include files,
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

		const static u8 REFERENCE_REG = 0;
		const static u8 REFERENCE_MEM = 1;

		constexpr static u8 FLAG_CARRY = 0;
		constexpr static u8 FLAG_ZERO = 1;
		constexpr static u8 FLAG_INTERRUPT_DISABLE = 2;
		constexpr static u8 FLAG_DECIMAL = 3;
		constexpr static u8 FLAG_BREAK = 4;
		constexpr static u8 FLAG_UNUSED = 5;
		constexpr static u8 FLAG_OVERFLOW = 6;
		constexpr static u8 FLAG_NEGATIVE = 7;

		/** Read a Byte from memory */
		virtual Byte readByte(u8& cycles, Word address) = 0;

		/** Write a Byte to memory */
		virtual void writeByte(u8& cycles, Word address, Byte value) = 0;


		/** Read a word from memory */
		virtual Word readWord(u8& cycles, Word address) = 0;


		/** Reads the byte pointed to by the PC and increments the PC  */
		virtual Byte readPCByte(u8& cycles) = 0;		//TODO change method name to indicate this changes the PC
		
		/** Reads the word pointed to by the PC and increments the PC twice */
		virtual Word readPCWord(u8& cycles) = 0;		//TODO change method name to indicate this changes the PC


		/* Reads the Byte currently held in the specified register */
		virtual Byte regValue(u8& cycles, u8 reg) = 0;

		/* Writes the given byte to the specified register - this method does not change any flags */
		virtual void saveToReg(u8& cycles, u8 reg, Byte value) = 0;


		/* Sets a specific flag in the status register */
		virtual void setFlag(u8& cycles, u8 flag, bool value) = 0;

		/* Gets a specific flag in the status register */
		virtual bool getFlag(u8& cycles, u8 flag) = 0;


		/* Push a byte onto the stack */
		virtual void pushStackByte(u8& cycles, Byte value) = 0;
		
		/* Push a word onto the stack */
		virtual void pushStackWord(u8& cycles, Word value) = 0;

		/* Pull a byte off the stack */
		virtual Byte pullStackByte(u8& cycles) = 0;

		/* Pull a word off the stack */
		virtual Word pullStackWord(u8& cycles) = 0;


		/* Read the status flags for the CPU */
		virtual FlagUnion getFlags(u8& cycles) = 0;

		/* Write the CPU status flags */
		virtual void setFlags(u8& cycles, FlagUnion flags) = 0;


		/* Read the PC register */
		virtual Word getPC(u8& cycles) = 0;

		/* Write the PC register */
		virtual void setPC(u8& cycles, Word address) = 0;

		/* Add the signed offset to the current PC, uses 1 cycle within a page, 2 if crossing a page boundary */
		virtual void branch(u8& cycles, s8 offset) = 0;


		/* Read the stack pointer register */
		virtual Byte getSP(u8& cycles) = 0;

		/* Write the stack pointer register */
		virtual void setSP(u8& cycles, Byte value) = 0;

		/* Read the byte stored at the location provided by the given reference */
		virtual Byte readReferenceByte(u8& cycles, Reference& ref) = 0;
		
		/* Write the given byte to the location specified by the given reference */
		virtual void writeReferenceByte(u8& cycles, Reference& ref, Byte data) = 0;


		/* Adds the given value to the accumulator (respecting D flag as needed), sets flags (N,V,Z,C) uses 1 cycle */
		virtual void addAccumulator(u8& cycles, Byte operandB) = 0;
		
		/* Subtracts the given value from the accumulator (respecting D flag as needed), sets flags (N,V,Z,C) uses 1 cycle */
		virtual void subAccumulator(u8& cycles, Byte operandB) = 0;

	};

	
	/* This represents CPU with additional methods for emulation management and direct access to CPUState/Memory - should not be used by instructions */
	class CPUInternal : public CPU {

	private:
		InstructionManager* insManager;
		Memory* mainMemory;
		CPUState* currentState;

	public:
		/** Constructor - Note on initialisation the CPU State is undefined, be sure to call reset() before execution */
		CPUInternal(CPUState* initSate, Memory* initMemory, InstructionLoader* loader);

		/* Execute <numInstructions> instructions. Return the number of cycles used. */
		u8 execute(u8 numInstructions);

		/* Same as execute from CPU but allows injecting mock CPU to handler for testing */
		u8 testExecute(u8 numInstructions, CPU* injectToHandler);

		/* Resets the CPU to the standard Initial state, clears registers & memory and sets PC to reset vector */
		void reset();

		/** CPU Overrides */
		virtual Byte readByte(u8& cycles, Word address);
		virtual void writeByte(u8& cycles, Word address, Byte value);

		virtual Word readWord(u8& cycles, Word address);

		virtual Byte readPCByte(u8& cycles);
		virtual Word readPCWord(u8& cycles);

		virtual Byte regValue(u8& cycles, u8 reg);
		virtual void saveToReg(u8& cycles, u8 reg, Byte value);

		virtual void setFlag(u8& cycles, u8 flag, bool value);
		virtual bool getFlag(u8& cycles, u8 flag);

		virtual void pushStackByte(u8& cycles, Byte value);
		virtual void pushStackWord(u8& cycles, Word value);
		virtual Byte pullStackByte(u8& cycles);
		virtual Word pullStackWord(u8& cycles);

		virtual FlagUnion getFlags(u8& cycles);
		virtual void setFlags(u8& cycles, FlagUnion flags);

		virtual Word getPC(u8& cycles);
		virtual void setPC(u8& cycles, Word address);
		virtual void branch(u8& cycles, s8 offset);

		virtual Byte getSP(u8& cycles);
		virtual void setSP(u8& cycles, Byte value);

		virtual Byte readReferenceByte(u8& cycles, Reference& ref) ;
		virtual void writeReferenceByte(u8& cycles, Reference& ref, Byte data);

		virtual void addAccumulator(u8& cycles, Byte operandB);
		virtual void subAccumulator(u8& cycles, Byte operandB);
	};
}

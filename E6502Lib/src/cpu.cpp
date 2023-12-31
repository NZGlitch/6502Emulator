﻿#pragma once
#include <stdio.h>
#include "cpu.h"

namespace E6502 {

	/* Initialises CPU objects */
	CPUInternal::CPUInternal(CPUState* initState, Memory* initMemory, InstructionLoader* loader) {
		insManager = new InstructionManager(loader);
		currentState = initState;
		mainMemory = initMemory;
	}

	/* Execute <numInstructions> instructions. Return the number of cycles used. */
	//TODO consider reading all the bytes for an instruction at the fetch stage and passing them as an array to handles?
	u8 CPUInternal::execute(u8 numInstructions) {
		return testExecute(numInstructions, nullptr);
	}
	
	// inject to handler is used by testframework to test for specific cpu calls during execution and should not be used
	// under normal operation. Note if used, instructions will not be able to affect the state of this CPU!
	u8 CPUInternal::testExecute(u8 numInstructions, CPU* injectToHandler) {
		u8 cyclesUsed = 0;
		while (numInstructions > 0) {
			//Get the next instruction and increment PC
			Byte code = (*mainMemory)[currentState->PC];
			currentState->PC++;
			cyclesUsed++;	//Fetching the instruction uses a cycle

			//Get the handler for this instruction
			const InstructionHandler* handler = (*insManager)[code];
			if (!handler->isLegal) {
				fprintf(stderr, "Executing illegal opcode 0x%02X\n", code);
			}
			if (injectToHandler == nullptr)
				handler->execute(this, cyclesUsed, code);
			else
				handler->execute(injectToHandler, cyclesUsed, code);
			numInstructions--;
		}
		return cyclesUsed;
	}

	/* Resets the CPU state - Until this is called, CPU state is undefined */
	void CPUInternal::reset() {
		currentState->reset();	// Resets the state
		mainMemory->reset();	// Reset Memory
	}


	/** CPU Overrides */
	
	/** Allows an instruction to read a Byte from memory, uses 1 cycle */
	Byte CPUInternal::readByte(u8& cycles, Word address) {
		Byte result = (*mainMemory)[address]; cycles++;
		return result;
	}

	/** Allows an instruction to write a byte to memory, uses 1 cycle */
	void CPUInternal::writeByte(u8& cycles, Word address, Byte value) {
		(*mainMemory)[address] = value; cycles++;
	}

	/** Allows an instruction to read a word from memory (Little endiean), uses 2 cycles*/
	Word CPUInternal::readWord(u8& cycles, Word address) {
		Word result = (*mainMemory)[address++]; cycles++;
		result |=  ( (*mainMemory)[address] << 8) ; cycles++;
		return result;
	}
	
	/** Reads the Byte pointed at by the current PC, increments PC, uses 1 cycle */
	Byte CPUInternal::readPCByte(u8& cycles) {
		Byte result = (*mainMemory)[currentState->PC++]; cycles++;
		return result;
	}

	/** Reads the Word pointed at by the current PC, increments PC, uses 2 cycles */
	Word CPUInternal::readPCWord(u8& cycles) {
		Word result = (*mainMemory)[currentState->PC++]; cycles++;
		result |= ((*mainMemory)[currentState->PC++] << 8 ); cycles++;
		return result;
	}

	/** gets the value of the specified register (returns 0xFF if invalid register specified), uses 0 cycles */
	Byte CPUInternal::regValue(u8& cycles, u8 reg) {
		switch (reg) {
		case REGISTER_A: return currentState->A;
		case REGISTER_X: return currentState->X;
		case REGISTER_Y: return currentState->Y;
		}
		fprintf(stderr, "Attempt to get vaue of invalid register %d ", reg);
		return 0xFF;
	}

	/** Saves the given value to the target register address and sets Z and N status flags based on the value, uses 0 cycles */
	void CPUInternal::saveToReg(u8& cycles, u8 reg, Byte value) {
		switch (reg) {
			case REGISTER_A: currentState->A = value; break;
			case REGISTER_X: currentState->X = value; break;
			case REGISTER_Y: currentState->Y = value; break;
				default: {
					fprintf(stderr, "Invalid register selected for CPUInternal::saveToReg %d", reg);
					return;
				}
		}
	}


	/* Sets a specific flag in the status register */
	void CPUInternal::setFlag(u8& cycles, u8 flag, bool value) {
		Byte mask = (0x01 << flag);
		if (value) 	currentState->FLAGS.byte |= mask;
		else {
			mask = ~mask;
			currentState->FLAGS.byte &= mask;
		}
	}

	/* Gets a specific flag in the status register */
	bool CPUInternal::getFlag(u8& cycles, u8 flag) {
		return (currentState->FLAGS.byte >> flag) & 0x01;
	}


	/* Push 1 byte of data onto the stack */
	void CPUInternal::pushStackByte(u8& cycles, Byte value) {
		(*mainMemory)[0x0100 | currentState->SP--] = value; cycles++;
	}

	/* Push 1 word of data onto the stack (Little end gets pushed first) */
	void CPUInternal::pushStackWord(u8& cycles, Word value) {
		(*mainMemory)[0x0100 | currentState->SP--] = value & 0xFF; cycles++;
		(*mainMemory)[0x0100 | currentState->SP--] = value >> 8; cycles++;
	}

	/* Pull the next byte off the stack */
	Byte CPUInternal::pullStackByte(u8& cycles) {
		Byte result = (*mainMemory)[0x0100 | ++currentState->SP]; cycles++;
		return result;
	}

	/* Pull a word from the stack */
	Word CPUInternal::pullStackWord(u8& cycles) {
		Word result = (*mainMemory)[0x0100 | ++currentState->SP] << 8; cycles++;	// read msb
		result |= (*mainMemory)[0x0100 | ++currentState->SP]; cycles++;				// read lsb
		return result;
	}
	
	/* Get the current processor status flags */
	FlagUnion CPUInternal::getFlags(u8& cycles) {
		cycles++;
		FlagUnion result = FlagUnion();
		result.byte = currentState->FLAGS.byte;
		return result;
	}

	/* Set the processor status flags */
	void CPUInternal::setFlags(u8& cycles, FlagUnion flags) {
		cycles++;
		currentState->FLAGS.byte = flags.byte;
	}

	/* Push the current value of the program counter to the stack, uses 2 cycles */
	Word CPUInternal::getPC(u8& cycles) {
		cycles++;
		return currentState->PC;
	}

	/* Set the program counter to the specified value, uses 0 cycles */
	void CPUInternal::setPC(u8& cycles, Word address) {
		currentState->PC = address; cycles++;
	}

	/* Add the signed offset to the current PC, uses 1 cycle within a page, 2 if crossing a page boundary */
	void CPUInternal::branch(u8& cycles, s8 offset) {
		Word initPC = currentState->PC;
		currentState->PC += offset; cycles++;
		if ((initPC & 0xFF00) != (currentState->PC & 0xFF00)) cycles++;	//Page changed
	}
	
	/* Get the current value of the stack pointer */
	Byte CPUInternal::getSP(u8& cycles) {
		cycles++;
		return currentState->SP;
	}

	/* Set the value of the stack pointer */
	void CPUInternal::setSP(u8& cycles, Byte value) {
		cycles++;
		currentState->SP = value;
	}

	/* Read the byte stored at the location provided by the given reference */
	Byte CPUInternal::readReferenceByte(u8& cycles, Reference& ref) {
		switch (ref.referenceType) {
			case CPU::REFERENCE_REG:
				return regValue(cycles, ref.reg);
			case CPU::REFERENCE_MEM:
				return readByte(cycles, ref.memoryAddress);
			default: {
				fprintf(stderr, "INVALID Reference type in cpu->readReferenceByte!");
				return 0x00;
			}
		}
	}

	/* Write the given byte to the location specified by the given reference */
	void CPUInternal::writeReferenceByte(u8& cycles, Reference& ref, Byte data) {
		switch (ref.referenceType) {
		case CPU::REFERENCE_REG:
			saveToReg(cycles, ref.reg, data);
			break;
		case CPU::REFERENCE_MEM:
			writeByte(cycles, ref.memoryAddress, data);
			break;
		}
	}

	/* Adds the given value to the accumulator (respecting D flag as needed), sets flags, uses 0 cycles */
	void CPUInternal::addAccumulator(u8& cycles, Byte operandB) {
		Byte result = 0;
		Byte operandA = currentState->A;
		if (currentState->FLAGS.bit.D) {
			// Decimal mode	- consider an interrupt to prompt user to seek medical help
			Word al = (operandA & 0x0F) + (operandB & 0x0F) + (currentState->FLAGS.bit.C ? 1 : 0);	// Add LSD
			if (al > 0x09) al = ((al + 0x06) & 0x0F) + 0x10;		// if lsd between A and F, add 6 to LSD to get back in range (+6&$F), add carry to next digit (+0x10)
			al = (operandA & 0xF0) + (operandB & 0xF0) + al;		// Add MSD
			if (al > 0x99) al = al + 0x60;							// if msd between A and F, add 6 to MSD to get back in range (+$60)
			currentState->FLAGS.bit.C = al > 0x99;
			result = (al & 0x00FF);			// Answer is lowets byte of AL
		}
		else {
			// Sensible mode
			result =  operandA + operandB + (currentState->FLAGS.bit.C ? 1 : 0);
			currentState->FLAGS.bit.C = (result < operandB);
		}

		// Set common flags and save result
		currentState->FLAGS.bit.Z = (result == 0x00);
		currentState->FLAGS.bit.N = (result >> 7);
		currentState->FLAGS.bit.V = (result >> 7) != (operandA >> 7);
		currentState->A = result;
	}

	/* Subtracts the given value from the accumulator (respecting D flag as needed), sets flags, uses 0 cycles */
	void CPUInternal::subAccumulator(u8& cycles, Byte operandB) {
		// TODO
	}
}

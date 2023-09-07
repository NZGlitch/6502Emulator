#pragma once
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
	u8 CPUInternal::execute(u8 numInstructions) {
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
			handler->execute(this, cyclesUsed, code);
			numInstructions--;
		}
		return cyclesUsed;
	}

	/* Allows getting all flags in a single byte */
	u8 CPUInternal::getFlags() {
		return currentState->PS;
	}

	/* Pops the most recently added item from the stack */
	Byte CPUInternal::popByteFromStack() {
		Byte res = (*mainMemory)[0x0100 | ++currentState->SP];
		return res;
	}

	/* Resets the CPU state - Until this is called, CPU state is undefined */
	void CPUInternal::reset() {
		currentState->reset();	// Resets the state
		mainMemory->reset();	// Reset Memory
	}

	/* Helper method, allows setting all flags at once */
	void CPUInternal::setFlags(u8 flags) {
		currentState->PS = flags;
	}

	/** CPU Overrides */
	
	/** Allows an instruction to read a Byte from memory, uses 1 cycle */
	Byte CPUInternal::readByte(u8& cycles, Word address) {
		Byte result = (*mainMemory)[address]; cycles++;
		return result;
	}

	/** Allows an instruction to read a word from memory (Little endiean), uses 2 cycles*/
	Word CPUInternal::readWord(u8& cycles, Word address) {
		Word result = (*mainMemory)[address++]; cycles++;
		result |=  ( (*mainMemory)[address] << 8) ; cycles++;
		return result;
	}

	/** Allows an instruction to write a byte to memory, uses 1 cycles */
	void CPUInternal::writeByte(u8& cycles, Word address, Byte value) {
		(*mainMemory)[address] = value; cycles++;
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

	/** Saves the given value to the target register address and sets Z and N status flags based on the value, uses 0 cycles */
	void CPUInternal::saveToRegAndFlagNZ(u8& cycles, u8 reg, Byte value) {
		switch (reg) {
			case REGISTER_A: currentState->A = value; break;
			case REGISTER_X: currentState->X = value; break;
			case REGISTER_Y: currentState->Y = value; break;
				default: {
					fprintf(stderr, "Invalid register selected for CPUInternal::saveToRegAndFlagNZ %d", reg);
					return;
				}
		}
		currentState->Flag.Z = value == 0x00;
		currentState->Flag.N = value >> 7;
	}

	/** Saves the given value to the target register address and sets Z and N status flags based on the value, uses 0 cycles */
	void CPUInternal::saveToRegAndFlagNZC(u8& cycles, u8 reg, Byte value) {
		switch (reg) {
		case REGISTER_A: currentState->A = value; break;
		case REGISTER_X: currentState->X = value; break;
		case REGISTER_Y: currentState->Y = value; break;
		default: {
			fprintf(stderr, "Invalid register selected for CPUInternal::saveToRegAndFlagNZ %d", reg);
			return;
		}
		}
		currentState->Flag.Z = value == 0x00;
		currentState->Flag.N = value >> 7;
	}

	/* Copy the stack pointer to register X */
	void CPUInternal::copyStackToXandFlag(u8& cycles) {
		saveToRegAndFlagNZ(cycles, REGISTER_X, currentState->SP);
		cycles++;
	}

	/* Copy X register to stack pointer */
	void CPUInternal::copyXtoStack(u8& cycles) {
		currentState->SP = currentState->X;
		cycles++;
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

	/* Push the current value of the program counter to the stack, uses 2 cycles */
	void CPUInternal::pushPCToStack(u8& cycles) {
		(*mainMemory)[0x0100 | currentState->SP--] = currentState->PC; cycles++;		// Write lsb
		(*mainMemory)[0x0100 | currentState->SP--] = currentState->PC >> 8; cycles++;	// Write msb
	}

	/* Set the program counter to the specified value, uses 0 cycles */
	void CPUInternal::setPC(u8& cycles, Word address) {
		currentState->PC = address; cycles++;
	}

	/* Pops a word from the stack */
	Word CPUInternal::popStackWord(u8& cycles) {
		Word result = (*mainMemory)[0x0100 | ++currentState->SP] << 8; cycles++;	// read msb
		result |= (*mainMemory)[0x0100 | ++currentState->SP]; cycles++;				// read lsb
		return result;
	}
}

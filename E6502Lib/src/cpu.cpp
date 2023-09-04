// 6502Emulator.cpp : Defines the entry point for the application.
// 
// Emulator built following tutorial here: https://www.youtube.com/watch?v=qJgsuQoy9bc
// 6502 CPU Reference: https://www.obelisk.me.uk/6502/
//
#pragma once
#include <stdio.h>
#include "cpu.h"

namespace E6502 {

	/* Initialises CPU objects */
	CPU::CPU(CPUState* initState, Memory* initMemory, InstructionLoader* loader) {
		insManager = new InstructionManager(loader);
		currentState = initState;
		mainMemory = initMemory;
	}

	/* Resets the CPU state - Until this is called, CPU state is undefined */
	void CPU::reset() {
		currentState->reset();	// Resets the state
		mainMemory->reset();	// Reset Memory
	}

	/* Helper method, allows setting all flags at once */
	void CPU::setFlags(u8 flags) {
		currentState->PS = flags;
	}

	/* Allows getting all flags in a single byte */
	u8 CPU::getFlags() {
		return currentState->PS;
	}

	/* Execute <numInstructions> instructions. Return the number of cycles used. */
	u8 CPU::execute(u8 numInstructions) {
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

	/** Allows an instruction to read a Byte from memory, uses 1 cycle */
	Byte CPU::readByte(u8& cycles, Word address) {
		Byte result = (*mainMemory)[address]; cycles++;
		return result;
	}

	/** Allows an instruction to read a word from memory (Little endiean), uses 2 cycles*/
	Word CPU::readWord(u8& cycles, Word address) {
		Word result = (*mainMemory)[address++]; cycles++;
		result |=  ( (*mainMemory)[address] << 8) ; cycles++;
		return result;
	}

	/** Allows an instruction to write a byte to memory, uses 1 cycles */
	void CPU::writeByte(u8& cycles, Word address, Byte value) {
		(*mainMemory)[address] = value; cycles++;
	}

	/** Reads the Byte pointed at by the current PC, increments PC, uses 1 cycle */
	Byte CPU::readPCByte(u8& cycles) {
		Byte result = (*mainMemory)[currentState->PC++]; cycles++;
		return result;
	}

	/** Reads the Word pointed at by the current PC, increments PC, uses 2 cycles */
	Word CPU::readPCWord(u8& cycles) {
		Word result = (*mainMemory)[currentState->PC++]; cycles++;
		result |= ((*mainMemory)[currentState->PC++] << 8 ); cycles++;
		return result;
	}

	/** Saves the given value to the target register address and sets Z and N status flags based on the value, uses 0 cycles */
	void CPU::saveToRegAndFlag(u8& cycles, u8 reg, Byte value) {
		switch (reg) {
			case REGISTER_A: currentState->A = value; break;
			case REGISTER_X: currentState->X = value; break;
			case REGISTER_Y: currentState->Y = value; break;
				default: {
					fprintf(stderr, "Invalid register selected for CPU::saveToRegAndFlag %d", reg);
					return;
				}
		}
		currentState->Flag.Z = value == 0x00;
		currentState->Flag.N = value >> 7;
	}

	/** gets the value of the specified register (returns 0xFF if invalid register specified), uses 0 cycles */
	Byte CPU::regValue(u8& cycles, u8 reg) {
		switch (reg) {
			case REGISTER_A: return currentState->A;
			case REGISTER_X: return currentState->X;
			case REGISTER_Y: return currentState->Y;
		}
		fprintf(stderr, "Attempt to get vaue of invalid register %d ", reg);
		return 0xFF;
	}

	/* Push the current value of the program counter to the stack, uses 2 cycles */
	void CPU::pushPCToStack(u8& cycles) {
		(*mainMemory)[0x0100 | currentState->SP--] = currentState->PC; cycles++;		// Write lsb
		(*mainMemory)[0x0100 | currentState->SP--] = currentState->PC >> 8; cycles++;	// Write msb
	}

	/* Set the program counter to the specified value, uses 0 cycles */
	void CPU::setPC(u8& cycles, Word address) {
		currentState->PC = address; cycles++;
	}

	/* Pops a word from the stack */
	Word CPU::popStackWord(u8& cycles) {
		Word result = (*mainMemory)[0x0100 | ++currentState->SP] << 8; cycles++;	// read msb
		result |= (*mainMemory)[0x0100 | ++currentState->SP]; cycles++;				// read lsb
		return result;
	}
}

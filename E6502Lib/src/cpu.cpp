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
		currentState->D = 0;											// Clear decimal flag
		currentState->I = 0;											// Clear interrupt flag
		currentState->PC = 0xFFFC;										// Set Program Counter
		currentState->setSP(0xFF);										// Set Stack Pointer
		currentState->A = currentState->X = currentState->Y = 0;		// Reset registers
		mainMemory->initialise();	// Reset Memory
	}

	/* Helper method, allows setting all flags at once */
	void CPU::setFlags(u8 flags) {
		currentState->C = (flags & 0b00000001) > 0;
		currentState->Z = (flags & 0b00000010) > 1;
		currentState->I = (flags & 0b00000100) > 2;
		currentState->D = (flags & 0b00001000) > 3;
		currentState->B = (flags & 0b00010000) > 4;
		currentState->O = (flags & 0b01000000) > 6;
		currentState->N = (flags & 0b10000000) > 7;
	}

	/* Allows getting all flags in a single byte */
	u8 CPU::getFlags() {
		return ((currentState->C << 0) | (currentState->Z << 1) | (currentState->I << 2) |
			(currentState->D << 3) | (currentState->B << 4) | (currentState->O << 6) | (currentState->N << 7));
	}

	/* Execute <numInstructions> instructions. Return the number of cycles used. */
	u8 CPU::execute(u8 numInstructions) {
		u8 cyclesUsed = 0;
		while (numInstructions > 0) {
			//Get the next instruction and increment PC
			Byte code = (*mainMemory)[currentState->PC];
			currentState->PC++;

			//Get the handler for this instruction
			const InstructionHandler* handler = (*insManager)[code];
			if (!handler->isLegal) {
				fprintf(stderr, "Executing illegal opcode 0x%02X\n", code);
			}
			cyclesUsed += handler->execute(this, code);
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

	/** Allows an instruction to wite a word to memory (Little endiean), uses 2 cycles */
	void CPU::writeByte(u8& cycles, Word address, Byte value) {
		(*mainMemory)[address] = value; cycles++;
	}

	/** Reads the Byte pointed at by the current PC, increments PC */
	Byte CPU::dequePCByte(u8& cycles) {
		Byte result = (*mainMemory)[currentState->PC++]; cycles++;
		return result;
	}

	/** Reads the Word pointed at by the current PC, increments PC */
	Word CPU::dequePCWord(u8& cycles) {
		Word result = (*mainMemory)[currentState->PC++]; cycles++;
		result |= ((*mainMemory)[currentState->PC++] << 8 ); cycles++;
		return result;
	}
}

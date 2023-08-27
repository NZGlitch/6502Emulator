#pragma once
#include "../types.h"
#include <stdio.h>

namespace E6502 {

	/**
	 * -----------------
	 * Instruction Base
	 * -----------------
	 *
	 * To create a new instruction class ABC do the following:
	 *
	 * 1) create an h and cpp file in src/instructions
	 * Define opcode(s) for your instruction e.g. const static Byte INS_LDA_INDX = 0xA1;
	 * Extend BaseInstruction with a class that has the following method signatures:
	 *     static constexpr Byte instructions[] = { INS_ABC_1, INS_ABC_2.... };
	 *     static const insHandlerFn ABCInstructionHandler;
	 *     static u8 executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode);
	 *     static void addHandlers(InstructionHandler* handlers[]);
	 *
	 * 2) Define handlers for all instructions by extending INSTRUCTION_BASE
	 * 3) Implement the methods from (1) in cpp file
	 * 4) Update InstructionUtils::loader to use your addHandlers method to add your instructions to the instruction manager
	 * 5) Don't forget to write tests!
	 */

	 /**
	 * Defines proeprties common to ALL instrcutions.
	 */
	struct INSTRUCTION_BASE : InstructionHandler {
		INSTRUCTION_BASE() {
			isLegal = true;
		}
	};

	/**
	* Common data and functions shared by all instructions
	* At present it is not intended that classes are instatiated, rather
	* functionality should be static
	*/
	class BaseInstruction {
	private:
		BaseInstruction();		// Subclasses should only be used statically

	public:
		/** Global Adressing Modes */
		const static Byte INDIRECT_X = 0b000;	// ??? 000 ??
		const static Byte ZERO_PAGE = 0b001;	// ??? 001 ??
		const static Byte IMMEDIATE = 0b010;	// ??? 010 ??
		const static Byte ABSOLUTE = 0b011;	// ??? 011 ??
		const static Byte INDIRECT_Y = 0b100;	// ??? 100 ??
		const static Byte ZERO_PAGE_X = 0b101;	// ??? 101 ??
		const static Byte ABSOLUTE_Y = 0b110;	// ??? 110 ??
		const static Byte ABSOLUTE_X = 0b111;	// ??? 111 ??
	};
}

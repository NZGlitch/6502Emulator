#pragma once
#include <stdio.h>
#include "../types.h"
#include "../instruction_handler.h"
#include "../cpu.h"

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
	 * 
	 *  TODO - Use templating and/or inheritence and a static register to automate all of this?
	 */

	 
	/**
	* Common data and functions shared by all instructions
	* At present it is not intended that classes are instatiated, rather
	* functionality should be static
	*/
	class BaseInstruction {
	private:
		BaseInstruction();		// Subclasses should only be used statically

	public:

		/* Uses Field B (Bits 4,3,2) to determine the addressing mode and returns a reference to the correct location 
		 * DO NOT use for immediate mode instructions!
		 */
		static Reference getReferenceForMode(CPU* cpu, u8& cycles, Byte mode);

		/** Global Adressing Modes - Commented modes need to be rechecked */
		const static Byte ADDRESS_MODE_INDIRECT_X	= 0b000;
		const static Byte ADDRESS_MODE_ZERO_PAGE	= 0b001;
		const static Byte ADDRESS_MODE_ACCUMULATOR	= 0b010;
		const static Byte ADDRESS_MODE_IMMEDIATE	= 0b010;
		const static Byte ADDRESS_MODE_ABSOLUTE		= 0b011;
		const static Byte ADDRESS_MODE_ZERO_PAGE_X	= 0b101;
		const static Byte ADDRESS_MODE_ABSOLUTE_Y	= 0b110;
		const static Byte ADDRESS_MODE_ABSOLUTE_X	= 0b111;		
		//const static Byte INDIRECT_Y = 0b100;	// ??? 100 ??
	};
}

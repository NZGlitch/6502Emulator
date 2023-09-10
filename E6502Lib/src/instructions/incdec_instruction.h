#pragma once
#include "base.h"

namespace E6502 {

	class IncDecInstruction : public BaseInstruction {
	public:

		/** Handles execution of all logical instructions */
		static void incdecHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Logic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Example: DEC Mem By One Field A: 110, Field C: 10*/
	constexpr static InstructionHandler INS_DEC_ABS = { 0x00, true, "DEC - Decrement Memory By One [Absolute]", IncDecInstruction::incdecHandler };
	
	// Array of all logic instructions
	static constexpr InstructionHandler INCDEC_INSTRUCTIONS[] = {
		/** DEC Instruction Definitions */
		INS_DEC_ABS,
	};
}

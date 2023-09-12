#pragma once
#include "base.h"

namespace E6502 {

	class ArithmeticInstruction : public BaseInstruction {
	public:

		/** Handles execution of all arithmetic instructions */
		static void arithmeticHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Arithmetic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// Arithmetic instruction defs
	constexpr static InstructionHandler INS_ADC_IMM = { 0x69, true, "ADC - Add Memory to Accumulator with Carry [Immedate]", ArithmeticInstruction::arithmeticHandler };

	// Array of all Arithmetic instructions
	static constexpr InstructionHandler ARITHMETIC_INSTRUCTIONS[] = {
		INS_ADC_IMM
	};
}

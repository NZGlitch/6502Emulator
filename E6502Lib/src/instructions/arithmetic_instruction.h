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
	constexpr static InstructionHandler INS_ADC_ABS = { 0x6D, true, "ADC - Add Memory to Accumulator with Carry [Absolute]", ArithmeticInstruction::arithmeticHandler };
	constexpr static InstructionHandler INS_ADC_ABX = { 0x7D, true, "ADC - Add Memory to Accumulator with Carry [X-Indexed Absolute]", ArithmeticInstruction::arithmeticHandler };
	constexpr static InstructionHandler INS_ADC_ABY = { 0x79, true, "ADC - Add Memory to Accumulator with Carry [Y-Indexed Absolute]", ArithmeticInstruction::arithmeticHandler };

	// Array of all Arithmetic instructions
	static constexpr InstructionHandler ARITHMETIC_INSTRUCTIONS[] = {
		INS_ADC_IMM, INS_ADC_ABS, INS_ADC_ABX, INS_ADC_ABY
	};
}

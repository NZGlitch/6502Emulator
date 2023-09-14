#pragma once
#include "base.h"

namespace E6502 {

	class ArithmeticInstruction : public BaseInstruction {
	public:

		/** Handles execution of all ADC instructions */
		static void adcHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Handles execution of all SBC instructions */
		static void sbcHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Arithmetic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// ADC instruction defs
	constexpr static InstructionHandler INS_ADC_IMM = { 0x69, true, "ADC - Add Memory to Accumulator with Carry [Immedate]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_ABS = { 0x6D, true, "ADC - Add Memory to Accumulator with Carry [Absolute]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_ABX = { 0x7D, true, "ADC - Add Memory to Accumulator with Carry [X-Indexed Absolute]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_ABY = { 0x79, true, "ADC - Add Memory to Accumulator with Carry [Y-Indexed Absolute]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_ZP0 = { 0x65, true, "ADC - Add Memory to Accumulator with Carry [Zero Page]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_ZPX = { 0x75, true, "ADC - Add Memory to Accumulator with Carry [X-Indexed Zero Page]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_INX = { 0x61, true, "ADC - Add Memory to Accumulator with Carry [X-Indexed Zero Page Indirect]", ArithmeticInstruction::adcHandler };
	constexpr static InstructionHandler INS_ADC_INY = { 0x71, true, "ADC - Add Memory to Accumulator with Carry [Zero Page Y-Indexed Indirect]", ArithmeticInstruction::adcHandler };

	// SBC instruction defs
	constexpr static InstructionHandler INS_SBC_IMM = { 0xE9, true, "ADC - Subtract Memory from Accumulator with Borrow [Immedate]", ArithmeticInstruction::sbcHandler };

	// Array of all Arithmetic instructions
	static constexpr InstructionHandler ARITHMETIC_INSTRUCTIONS[] = {
		// ADC Instructions
		INS_ADC_IMM, INS_ADC_ABS, INS_ADC_ABX, INS_ADC_ABY, 
		INS_ADC_ZP0, INS_ADC_ZPX, INS_ADC_INX, INS_ADC_INY,

		// SBC Instructions
		INS_SBC_IMM,
	};
}

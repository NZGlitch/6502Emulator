#pragma once
#include "base.h"

namespace E6502 {

	class LogicInstruction : public BaseInstruction {
	public:

		// Operations mapped to op field value (Opfield is bits 7,6,5,1,0 of the OpCode)
		constexpr static Byte OP_ASL = 0x2;	// Arithmatic shift Left
		constexpr static Byte OP_ROL = 0x6; // Rotate left
		constexpr static Byte OP_LSR = 0xA;	// Logical shift right

		/** Handles execution of all logical instructions */
		static void logicHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add logic instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);

		/* Helper method actually performs the required operation */
		static void performOp(CPU* cpu, u8& cycles, Byte op, Byte& value, Byte& carry);		
	};

	/** ASL Instruction Definitions Field A: 000, Field C: 10 */
	constexpr static InstructionHandler INS_ASL_ACC = { 0x0A, true, "ASL - Arithmetic Shift Left [Accumulator]",	LogicInstruction::logicHandler };

	/** ROL Instruction Definitions Field A: 001, Field C: 10 */
	constexpr static InstructionHandler INS_ROL_ACC = { 0x2A, true, "ROL - Rotate Left [Accumulator]",				LogicInstruction::logicHandler };

	/** LSR Instruction Definitions Field A: 010, Field C: 10 */
	constexpr static InstructionHandler INS_LSR_ACC = { 0x4A, true, "LSR - Logical Shift Right [Accumulator]",		LogicInstruction::logicHandler };

	// Array of all logic instructions
	static constexpr InstructionHandler LOGIC_INSTRUCTIONS[] = {
		/** ASL Instruction Definitions */
		INS_ASL_ACC,

		/** ROL Instruction Definitions */
		INS_ROL_ACC,

		/** LSR Instruction Definitions */
		INS_LSR_ACC,
	};
}

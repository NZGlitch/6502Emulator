#pragma once
#include "base.h"

namespace E6502 {

	class ShiftInstruction : public BaseInstruction {
	public:

		// Operations mapped to op field value (Opfield is bits 7,6,5,1,0 of the OpCode)
		constexpr static Byte OP_ASL = 0x2;		// Arithmatic shift Left
		constexpr static Byte OP_ROL = 0x6;		// Rotate left
		constexpr static Byte OP_LSR = 0xA;		// Logical shift right
		constexpr static Byte OP_ROR = 0xE;		// Logical shift right

		/** Handles execution of all logical instructions */
		static void shiftHandler(CPU* cpu, u8& cycles, Byte opCode);

		/* Helper method actually performs the required operation, if after the op the carry Byte is not 0 the carry flag will be set, if 0 it will be unset  */
		static void performOp(CPU* cpu, u8& cycles, Byte op, Byte& value, Byte& carry);	

		/** Called to add Shift Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** ASL Instruction Definitions Field A: 000, Field C: 10 */
	constexpr static InstructionHandler INS_ASL_ACC = { 0x0A, true, "ASL - Arithmetic Shift Left [Accumulator]",	ShiftInstruction::shiftHandler };

	/** ROL Instruction Definitions Field A: 001, Field C: 10 */
	constexpr static InstructionHandler INS_ROL_ACC = { 0x2A, true, "ROL - Rotate Left [Accumulator]",				ShiftInstruction::shiftHandler };

	/** LSR Instruction Definitions Field A: 010, Field C: 10 */
	constexpr static InstructionHandler INS_LSR_ACC = { 0x4A, true, "LSR - Logical Shift Right [Accumulator]",		ShiftInstruction::shiftHandler };

	/** ROR Instruction Definitions Field A: 011, Field C: 10 */
	constexpr static InstructionHandler INS_ROR_ACC = { 0x6A, true, "ROR - Rotate Right [Accumulator]",				ShiftInstruction::shiftHandler };

	// Array of all logic instructions
	static constexpr InstructionHandler SHIFT_INSTRUCTIONS[] = {
		/** ASL Instruction Definitions */
		INS_ASL_ACC,

		/** ROL Instruction Definitions */
		INS_ROL_ACC,

		/** LSR Instruction Definitions */
		INS_LSR_ACC,

		/** ROR Instruction Definitions */
		INS_ROR_ACC,
	};
}

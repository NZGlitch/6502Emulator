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
	constexpr static InstructionHandler INS_ASL_ACC = { 0x0A, true, "ASL - Arithmetic Shift Left [Accumulator]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ASL_ABS = { 0x0E, true, "ASL - Arithmetic Shift Left [Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ASL_ABX = { 0x1E, true, "ASL - Arithmetic Shift Left [X-Indexed Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ASL_ZP0 = { 0x06, true, "ASL - Arithmetic Shift Left [Zero Page]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ASL_ZPX = { 0x16, true, "ASL - Arithmetic Shift Left [X-Indexed Zero Page]", ShiftInstruction::shiftHandler };

	/** ROL Instruction Definitions Field A: 001, Field C: 10 */
	constexpr static InstructionHandler INS_ROL_ACC = { 0x2A, true, "ROL - Rotate Left [Accumulator]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROL_ABS = { 0x2E, true, "ROL - Rotate Left [Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROL_ABX = { 0x3E, true, "ROL - Rotate Left [X-Indexed Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROL_ZP0 = { 0x26, true, "ROL - Rotate Left [Zero Page]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROL_ZPX = { 0x36, true, "ROL - Rotate Left [X-Indexed Zero Page]", ShiftInstruction::shiftHandler };

	/** LSR Instruction Definitions Field A: 010, Field C: 10 */
	constexpr static InstructionHandler INS_LSR_ACC = { 0x4A, true, "LSR - Logical Shift Right [Accumulator]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_LSR_ABS = { 0x4E, true, "LSR - Logical Shift Right [Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_LSR_ABX = { 0x5E, true, "LSR - Logical Shift Right [X-Indexed Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_LSR_ZP0 = { 0x46, true, "LSR - Logical Shift Right [Zero Page]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_LSR_ZPX = { 0x56, true, "LSR - Logical Shift Right [X-Indexed Zero Page]", ShiftInstruction::shiftHandler };

	/** ROR Instruction Definitions Field A: 011, Field C: 10 */
	constexpr static InstructionHandler INS_ROR_ACC = { 0x6A, true, "ROR - Rotate Right [Accumulator]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROR_ABS = { 0x6E, true, "ROR - Rotate Right [Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROR_ABX = { 0x7E, true, "ROR - Rotate Right [X-Indexed Absolute]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROR_ZP0 = { 0x66, true, "ROR - Rotate Right [Zero Page]", ShiftInstruction::shiftHandler };
	constexpr static InstructionHandler INS_ROR_ZPX = { 0x76, true, "ROR - Rotate Right [X-Indexed Zero Page]", ShiftInstruction::shiftHandler };

	// Array of all logic instructions
	static constexpr InstructionHandler SHIFT_INSTRUCTIONS[] = {
		/** ASL Instruction Definitions */
		INS_ASL_ACC, INS_ASL_ABS, INS_ASL_ABX, INS_ASL_ZP0, INS_ASL_ZPX,

		/** ROL Instruction Definitions */
		INS_ROL_ACC, INS_ROL_ABS, INS_ROL_ABX, INS_ROL_ZP0, INS_ROL_ZPX,

		/** LSR Instruction Definitions */
		INS_LSR_ACC, INS_LSR_ABS, INS_LSR_ABX, INS_LSR_ZP0, INS_LSR_ZPX,

		/** ROR Instruction Definitions */
		INS_ROR_ACC, INS_ROR_ABS, INS_ROR_ABX, INS_ROR_ZP0, INS_ROR_ZPX,
	};
}

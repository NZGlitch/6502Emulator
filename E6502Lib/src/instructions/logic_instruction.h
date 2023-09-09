#pragma once
#include "base.h"

namespace E6502 {

	class LogicInstruction : public BaseInstruction {
	private:
		
	public:

		// Operations mapped to op field value (Opfield is bits 7,6,5,1,0 of the OpCode)
		constexpr static Byte OP_EOR = 0x9;
		constexpr static Byte OP_AND = 0x5;
		constexpr static Byte OP_ORA = 0x1;
		constexpr static Byte OP_BIT = 0x4;

		static Byte AND(Byte a, Byte b) { return a & b; }
		static Byte EOR(Byte a, Byte b) { return a ^ b; }
		static Byte ORA(Byte a, Byte b) { return a | b; }

		/** Handles execution of all logical instructions */
		static void logicHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Logic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** EOR Instruction Definitions Field A: 010, Field C: 01 */
	constexpr static InstructionHandler INS_EOR_IMM = { 0x49, true, "EOR - 'Exclusive Or' Memory with Accumulator [Immediate]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_EOR_ABS = { 0x4D, true, "EOR - 'Exclusive Or' Memory with Accumulator [Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_EOR_ABX = { 0x5D, true, "EOR - 'Exclusive Or' Memory with Accumulator [X-Indexed Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_EOR_ABY = { 0x59, true, "EOR - 'Exclusive Or' Memory with Accumulator [Y-Indexed Absolute]", LogicInstruction::logicHandler };

	/** AND Instruction Definitions Field A: 001, Field C: 01 */
	constexpr static InstructionHandler INS_AND_IMM = { 0x29, true, "AND - 'AND' Memory with Accumulator [Immediate]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_AND_ABS = { 0x2D, true, "AND - 'AND' Memory with Accumulator [Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_AND_ABX = { 0x3D, true, "AND - 'AND' Memory with Accumulator [X-Indexed Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_AND_ABY = { 0x39, true, "AND - 'AND' Memory with Accumulator [Y-Indexed Absolute]", LogicInstruction::logicHandler };

	/** ORA Instruction Definitions Field A: 000, Field C: 01  */
	constexpr static InstructionHandler INS_ORA_IMM = { 0x09, true, "ORA - 'OR' Memory with Accumulator [Immediate]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_ORA_ABS = { 0x0D, true, "ORA - 'OR' Memory with Accumulator [Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_ORA_ABX = { 0x1D, true, "ORA - 'OR' Memory with Accumulator [X-Indexed Absolute]", LogicInstruction::logicHandler };
	constexpr static InstructionHandler INS_ORA_ABY = { 0x19, true, "ORA - 'OR' Memory with Accumulator [Y-Indexed Absolute]", LogicInstruction::logicHandler };

	/** BIT Instruction Definitions Field A: 001, Field C: 00 */
	constexpr static InstructionHandler INS_BIT_ABS = { 0x2C, true, "BIT - Test Bits in Memory with Accumulator [Absolute]", LogicInstruction::logicHandler };
	
	// Array of all logic instructions
	static constexpr InstructionHandler LOGIC_INSTRUCTIONS[] = {
		/** EOR Instruction Definitions */
		INS_EOR_IMM, INS_EOR_ABS, INS_EOR_ABX, INS_EOR_ABY,

		/** AND Instruction Definitions */
		INS_AND_IMM, INS_AND_ABS, INS_AND_ABX, INS_AND_ABY,

		/** ORA Instruction Definitions */
		INS_ORA_IMM, INS_ORA_ABS, INS_ORA_ABX, INS_ORA_ABY,

		/** BIT Instruction Definitions */
		INS_BIT_ABS,
	};
}

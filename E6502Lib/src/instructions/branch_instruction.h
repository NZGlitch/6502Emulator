#pragma once
#include "base.h"

namespace E6502 {

	class BranchInstruction : public BaseInstruction {
	public:

		/** SCRATCHPAD - Delete as implement, codes are not verified
			BPL	$10	NegClear		000,00		00
			BVC	$50	VClear			010,00		80
			BVS	$70 VSet			011,00		0C
		*/

		// Opcodes for all branch instructions
		constexpr static int OP_BCC		= 0x10;
		constexpr static int OP_BNE		= 0x18;
		constexpr static int OP_BPL		= 0x00;
		constexpr static int OP_BCS		= 0x14;
		constexpr static int OP_BEQ		= 0x1C;
		constexpr static int OP_BMI		= 0x04;
		
		/** Handles execution of all branch instructions */
		static void branchHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Branch Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// Branch instruction defs where checking flag clear
	constexpr static InstructionHandler INS_BCC_REL = { 0x90, true, "BCC - Branch on Carry Clear [Relative]", BranchInstruction::branchHandler };
	constexpr static InstructionHandler INS_BNE_REL = { 0xD0, true, "BNE - Branch on Result Not Zero [Relative]", BranchInstruction::branchHandler };
	constexpr static InstructionHandler INS_BPL_REL = { 0x10, true, "BPL - Branch on Result Plus [Relative]", BranchInstruction::branchHandler };

	// Branch instruction defs where checking flag set
	constexpr static InstructionHandler INS_BCS_REL = { 0xB0, true, "BCS - Branch on Carry Set [Relative]", BranchInstruction::branchHandler };
	constexpr static InstructionHandler INS_BEQ_REL = { 0xF0, true, "BEQ - Branch on Result Zero [Relative]", BranchInstruction::branchHandler };
	constexpr static InstructionHandler INS_BMI_REL = { 0x30, true, "BEQ - Branch on Result Minus [Relative]", BranchInstruction::branchHandler };

	// Array of all Increment/Decrement instructions
	static constexpr InstructionHandler BRANCH_INSTRUCTIONS[] = {
		INS_BCC_REL, INS_BNE_REL, INS_BPL_REL,
		INS_BCS_REL, INS_BEQ_REL, INS_BMI_REL
	};
}

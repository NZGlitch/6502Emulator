#pragma once
#include "base.h"

namespace E6502 {

	class BranchInstruction : public BaseInstruction {
	public:

		/** SCRATCHPAD - Delete as implement, codes are not verified
			BEQ	$f0 ZeroSet			111,00		1C
			BMI	$30 NegSet			001,00		04
			BNE $D0 ZeroClear		110,00		18
			BPL	$10	NegClear		000,00		00
			BVC	$50	VClear			010,00		80
			BVS	$70 VSet			011,00		0C
		*/

		// Opcodes for all branch instructions
		constexpr static int OP_CARRY_CLEAR		= 0x10;
		constexpr static int OP_CARRY_SET		= 0x14;
		
		/** Handles execution of all branch instructions */
		static void branchHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Branch Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// Branch instruction defs
	constexpr static InstructionHandler INS_BCC_REL = { 0x90, true, "BCC - Branch on Carry Clear [Relative]", BranchInstruction::branchHandler };
	constexpr static InstructionHandler INS_BCS_REL = { 0xB0, true, "BCS - Branch on Carry Set [Relative]", BranchInstruction::branchHandler };

	// Array of all Increment/Decrement instructions
	static constexpr InstructionHandler BRANCH_INSTRUCTIONS[] = {
		INS_BCC_REL, INS_BCS_REL
	};
}

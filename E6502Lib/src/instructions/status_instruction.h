#pragma once
#include "base.h"

namespace E6502 {

	class StatusInstruction : public BaseInstruction {
	public:

		/** Handles execution of all status flag instructions */
		static void statusHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Status Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	// Status instruction defs
	constexpr static InstructionHandler INS_CLC_IMP = { 0x18, true, "CLC - Clear Carry Flag [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_SEC_IMP = { 0x38, true, "SEC - Set Carry Flag [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_CLI_IMP = { 0x58, true, "CLI - Clear Interrupt Disable [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_SEI_IMP = { 0x78, true, "SEI - Set Interrupt Disable [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_CLV_IMP = { 0xB8, true, "CLV - Clear Overflow Flag [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_CLD_IMP = { 0xD8, true, "CLD - Clear Decimal Mode [Implied]", StatusInstruction::statusHandler };
	constexpr static InstructionHandler INS_SED_IMP = { 0xF8, true, "SED - Set Decimal Mode [Implied]", StatusInstruction::statusHandler };
	

	// Array of all Status Flag instructions
	static constexpr InstructionHandler STATUS_INSTRUCTIONS[] = {
		INS_CLC_IMP, INS_CLD_IMP, INS_CLI_IMP, INS_CLV_IMP,
		INS_SEC_IMP, INS_SED_IMP, INS_SEI_IMP,
	};
}

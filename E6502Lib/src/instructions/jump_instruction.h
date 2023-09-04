#pragma once
#include "base.h"

namespace E6502 {

	class Jump : public BaseInstruction {
	public:
		
		/** Actually handles execution of JSR instruction */
		static void jsrHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** JSR Instruction Code */
	constexpr static InstructionHandler INS_JSR = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", Jump::jsrHandler };
	constexpr static InstructionHandler INS_JMP_ABS = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", Jump::jsrHandler };
	constexpr static InstructionHandler INS_JMP_ABIN = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", Jump::jsrHandler };
	constexpr static InstructionHandler INS_RTS = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", Jump::jsrHandler };

	// Handy array of all load instructions
	static constexpr InstructionHandler JUMP_INSTRUCTIONS[] = {
		INS_JSR, INS_JMP_ABS, INS_JMP_ABIN, INS_RTS
	};
}

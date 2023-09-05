#pragma once
#include "base.h"

namespace E6502 {

	class Jump : public BaseInstruction {
	public:
		
		/** Actually handles execution of JSR instruction */
		static void jsrHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Actually handles execution of JMP instruction */
		static void jmpHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Actually handles execution of RST instruction */
		static void rstHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add LDA Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** JSR, JMP, RTS Instruction Definitions */
	constexpr static InstructionHandler INS_JSR = { 0x20, true, "JSR - Jump to Subroutine [Absolute]", Jump::jsrHandler };
	constexpr static InstructionHandler INS_JMP_ABS = { 0x4C, true, "JMP - Jump [Absolute]", Jump::jmpHandler };
	constexpr static InstructionHandler INS_JMP_ABIN = { 0x6C, true, "JMP - Jump [Absolute Indirect]", Jump::jmpHandler };
	constexpr static InstructionHandler INS_RTS = { 0x60, true, "RTS - Return from subroutine [Implied]", Jump::rstHandler };

	// Handy array of all load instructions
	static constexpr InstructionHandler JUMP_INSTRUCTIONS[] = {
		INS_JSR, INS_JMP_ABS, INS_JMP_ABIN, INS_RTS
	};
}

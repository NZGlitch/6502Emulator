#pragma once
#include "base.h"

namespace E6502 {

	class TransferInstruction : public BaseInstruction {
	public:

		/** Handler for register transfer operations */
		static void transferRegHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Handler for stack transfer operations */
		static void transferStackHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add TransferInstruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Register transfers */
	constexpr static InstructionHandler INS_TAX = { 0xAA, true, "TAX - Transfer Accumulator to Index X [implied]", TransferInstruction::transferRegHandler };
	constexpr static InstructionHandler INS_TAY = { 0xA8, true, "TAY - Transfer Accumulator to Index Y [implied]", TransferInstruction::transferRegHandler };
	constexpr static InstructionHandler INS_TXA = { 0x8A, true, "TXA - Transfer Index X to Accumulator [implied]", TransferInstruction::transferRegHandler };
	constexpr static InstructionHandler INS_TYA = { 0x98, true, "TYA - Transfer Index Y to Accumulator [implied]", TransferInstruction::transferRegHandler };

	/** Stack transfers */
	constexpr static InstructionHandler INS_TSX = { 0xBA, true, "TSX - Transfer Stack Pointer to Index X [implied]", TransferInstruction::transferStackHandler };
	constexpr static InstructionHandler INS_TXS = { 0x9A, true, "TXS - Transfer Index X to Stack Pointer [implied]", TransferInstruction::transferStackHandler };

	// Handy array of all transfer instructions
	static constexpr InstructionHandler TRANS_INSTRUCTIONS[] = {
		/* Register transfers */
		INS_TAX, INS_TAY, INS_TXA, INS_TYA,

		/** Stack transfers */
		INS_TSX, INS_TXS
	};
}

#include <gmock/gmock.h>
#include "instruction_test.h"
#include "transfer_instruction.h"

namespace E6502 {

	class TestTransferInstruction : public TestInstruction {
	public:

		/* Helper to test correct execution of transfer instructions */
		void testTransfer(InstructionHandler instruction, Byte* sourceReg, Byte* targetReg, u8 expectedCycles, bool checkFlags) {
			// Given:
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			*sourceReg = testValue;
			(*memory)[programSpace] = instruction.opcode;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			if (checkFlags) testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(*sourceReg, testValue);
		}
	};

	/* Test addHandlers function and instruction opcodes */
	TEST_F(TestTransferInstruction, TestLDAaddHandlers) {
		std::vector<InstructionMap> instructions = {
			// Register transfers
			{INS_TAX, 0xAA},
			{INS_TAY, 0xA8},
			{INS_TXA, 0x8A},
			{INS_TYA, 0x98},

			// Stack transfers
			{INS_TSX, 0xBA},
			{INS_TXS, 0x9A},
		};
		testInstructionDef(instructions, TransferInstruction::addHandlers);
	}

	TEST_F(TestTransferInstruction, TestTAX) { testTransfer(INS_TAX, &state->A, &state->X, 2, true); }
	TEST_F(TestTransferInstruction, TestTAY) { testTransfer(INS_TAY, &state->A, &state->Y, 2, true); }
	TEST_F(TestTransferInstruction, TestTXA) { testTransfer(INS_TXA, &state->X, &state->A, 2, true); }
	TEST_F(TestTransferInstruction, TestTYA) { testTransfer(INS_TYA, &state->Y, &state->A, 2, true); }
	TEST_F(TestTransferInstruction, TestTSX) { testTransfer(INS_TSX, &state->SP, &state->X, 2, true); }
	TEST_F(TestTransferInstruction, TestTXS) { testTransfer(INS_TXS, &state->X, &state->SP, 2, false); }
}

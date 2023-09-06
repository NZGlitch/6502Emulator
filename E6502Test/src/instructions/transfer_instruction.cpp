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
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		TransferInstruction::addHandlers(handlers);

		// Register transfers
		EXPECT_EQ(INS_TAX.opcode, 0xAA);	EXPECT_EQ(*handlers[0xAA], INS_TAX);	// 101 010 10
		EXPECT_EQ(INS_TAY.opcode, 0xA8);	EXPECT_EQ(*handlers[0xA8], INS_TAY);	// 101 010 00
		EXPECT_EQ(INS_TXA.opcode, 0x8A);	EXPECT_EQ(*handlers[0x8A], INS_TXA);	// 100 010 10
		EXPECT_EQ(INS_TYA.opcode, 0x98);	EXPECT_EQ(*handlers[0x98], INS_TYA);	// 100 110 00

		// Stack transfers
		EXPECT_EQ(INS_TSX.opcode, 0xBA);	EXPECT_EQ(*handlers[0xBA], INS_TSX);	// 101 110 10
		EXPECT_EQ(INS_TXS.opcode, 0x9A);	EXPECT_EQ(*handlers[0x9A], INS_TXS);	// 100 110 10
	}

	TEST_F(TestTransferInstruction, TestTAX) { testTransfer(INS_TAX, &state->A, &state->X, 2, true); }
	TEST_F(TestTransferInstruction, TestTAY) { testTransfer(INS_TAY, &state->A, &state->Y, 2, true); }
	TEST_F(TestTransferInstruction, TestTXA) { testTransfer(INS_TXA, &state->X, &state->A, 2, true); }
	TEST_F(TestTransferInstruction, TestTYA) { testTransfer(INS_TYA, &state->Y, &state->A, 2, true); }
	TEST_F(TestTransferInstruction, TestTSX) { testTransfer(INS_TSX, &state->SP, &state->X, 2, true); }
	TEST_F(TestTransferInstruction, TestTXS) { testTransfer(INS_TXS, &state->X, &state->SP, 2, false); }
}

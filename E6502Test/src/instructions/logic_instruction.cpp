#include <gmock/gmock.h>
#include "instruction_test.h"
#include "logic_instruction.h"

namespace E6502 {
	class TestLogicInstruction : public TestInstruction {
	private:	

		/* Reused code for finishing tests from helpers below */
		void finishTest(Byte expectedResult, u8 expectedCycles, u8 actualCycles) {
			// Then
			EXPECT_EQ(expectedResult, state->A);
			EXPECT_EQ(expectedCycles, actualCycles);
			testAndResetStatusFlags(expectedResult);
		}

	public:
		/* Helper method for testing operations using immediate addressing mode */
		void testImmOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) {
			// Given
			Byte a = rand();
			Byte b = rand();
			Byte expectedResult = op(a, b);
			state->A = a;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = b;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			finishTest(expectedResult, expectedCycles, cycles);
		}

		/* Helper method for testing operations using absolute addressing mode */
		void testAbsOp(InstructionHandler instruction) {}

		/* Helper method for testing operations using absolute addressing-x mode */
		void testAbsXOp(InstructionHandler) {}

		/* Helper method for testing operations using absolute addressing-y mode */
		void testAbsyOp(InstructionHandler) {}

		/* Helper method for testing operations using zero page mode */
		void testZPOp(InstructionHandler instruction) {}

		/* Helper method for testing operations using X-indexed zero page mode */
		void testZPXOp(InstructionHandler instruction) {}
	};

	TEST_F(TestLogicInstruction, TestFuncAND) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::AND(a, b), a & b);
	}

	TEST_F(TestLogicInstruction, TestFuncEOR) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::EOR(a, b), a ^ b);
	}

	TEST_F(TestLogicInstruction, TestFuncORA) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::ORA(a, b), a | b);
	}

	/* Test defs & addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// AND Instructions
			{INS_AND_IMM, 0x29},

			// BIT Instructions
			

			// EOR Instructions
			{INS_EOR_IMM, 0x49},

			// ORA Instructions
			{INS_ORA_IMM, 0x09},
		};
		testInstructionDef(instructions, LogicInstruction::addHandlers);
	}

	/* Test AND execution */
	// IMM
	TEST_F(TestLogicInstruction, TestANDImmediate) { testImmOp(INS_AND_IMM, LogicInstruction::AND, 2);  }

	/* Test EOR execution */
	// IMM
	TEST_F(TestLogicInstruction, TestEORImmediate) { testImmOp(INS_EOR_IMM, LogicInstruction::EOR, 2); }

	/* Test ORA execution */
	// IMM
	TEST_F(TestLogicInstruction, TestORAImmediate) { testImmOp(INS_ORA_IMM, LogicInstruction::ORA, 2); }
}

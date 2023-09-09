#include <gmock/gmock.h>
#include "instruction_test.h"
#include "shift_instruction.h"

namespace E6502 {
	class TestLogicInstruction : public TestInstruction {
	private:

		/* Reused code for finishing tests from helpers below */
		void finishTest(Byte expectValue, bool carryOut) {
			// Then
			//EXPECT_EQ(state->A, expectValue);
			//EXPECT_EQ(state->FLAGS.bit.C, carryOut);
			//testAndResetStatusFlags(expectValue);
		}

	public:

		/* Helper method for testing operations using immediate addressing mode */
		void testImmOp(InstructionHandler instruction) {}

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

	/* Test defs & addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// AND Instructions
			//{INS_AND_IMM, 0x29},

			// BIT Instructions
			//{INS_BIT_IMM, 0x2A},

			// EOR Instructions
			//{INS_EOR_ACC, 0x4A},

			// ORA Instructions
			//{INS_ORA_IMM, 0x6A},
		};
		testInstructionDef(instructions, ShiftInstruction::addHandlers);
	}

	/* Test AND execution */
	// IMM
	//TEST_F(TestLogicInstruction, TestANDImmediate) { Byte tVal = (rand() & 0x7F);	testImmOp(INS_AND_IMM); }
}

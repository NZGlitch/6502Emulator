#include <gmock/gmock.h>
#include "instruction_test.h"
#include "logic_instruction.h"

namespace E6502 {
	class TestLogicInstruction : public TestInstruction {
	public:

		/* Helper method for testing operations using accumulator addressing mode */
		void testAccOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = testValue;
			state->FLAGS.bit.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(state->A, expectValue);
			EXPECT_EQ(cycles, expectCycles);
			EXPECT_EQ(state->FLAGS.bit.C, carryOut);
			testAndResetStatusFlags(expectValue);
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// ASL Instructions
			{INS_ASL_ACC, 0x0A},
			{INS_ROL_ACC, 0x2A},
			{INS_LSR_ACC, 0x4A},
			{INS_ROR_ACC, 0x6A},
		};
		testInstructionDef(instructions, LogicInstruction::addHandlers);
	}

	/* Test ASL execution */
	TEST_F(TestLogicInstruction, TestASLAccNoCarry) { Byte tVal = (rand() & 0x7F);	testAccOp(INS_ASL_ACC, tVal, tVal << 1, 1, 0, 2); }	
	TEST_F(TestLogicInstruction, TestASLAccWiCarry) { Byte tVal = (rand() | 0x80);	testAccOp(INS_ASL_ACC, tVal, tVal << 1, 0, 1, 2); }

	/* Test ROL Execution */
	TEST_F(TestLogicInstruction, TestROLAccC00) { Byte tVal = rand() & 0x7F; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) & 0xFE, 0, 0, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC01) { Byte tVal = rand() | 0x80; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) & 0xFE, 0, 1, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC10) { Byte tVal = rand() & 0x7F; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) | 0x01, 1, 0, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC11) { Byte tVal = rand() | 0x80; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) | 0x01, 1, 1, 2); }

	/* Test LSR Execution */
	TEST_F(TestLogicInstruction, TestLSRAccNoCarry) { Byte tVal = (rand() & 0xFE);	testAccOp(INS_LSR_ACC, tVal, (tVal >> 1) & 0x7F, 1, 0, 2); }
	TEST_F(TestLogicInstruction, TestLSRAccWiCarry) { Byte tVal = (rand() | 0x01);	testAccOp(INS_LSR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 1, 2); }

	/* Test ROR Execution */
	TEST_F(TestLogicInstruction, TestRORAccC00) { Byte tVal = rand() & 0xFE; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 0, 2); }
	TEST_F(TestLogicInstruction, TestRORAccC01) { Byte tVal = rand() | 0x01; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 1, 2); }
	TEST_F(TestLogicInstruction, TestRORAccC10) { Byte tVal = rand() & 0xFE; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) | 0x80, 1, 0, 2); }
	TEST_F(TestLogicInstruction, TestRORAccC11) { Byte tVal = rand() | 0x01; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) | 0x80, 1, 1, 2); }
}

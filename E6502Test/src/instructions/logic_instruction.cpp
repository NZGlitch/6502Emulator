#include <gmock/gmock.h>
#include "instruction_test.h"
#include "logic_instruction.h"

namespace E6502 {
	class TestLogicInstruction : public TestInstruction {
	public:

		/* Helper method for testing operations using accumulator addressing mode */
		void testAccMode(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = testValue;
			state->Flag.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(state->A, expectValue);
			EXPECT_EQ(cycles, expectCycles);
			EXPECT_EQ(state->Flag.C, carryOut);
			testAndResetStatusFlags(expectValue);
		}
	};

	/* Test defs &  addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// ASL Instructions
			{INS_ASL_ACC, 0x0A},
			{INS_ROL_ACC, 0x2A},
			{INS_LSR_ACC, 0x4A}
		};
		testInstructionDef(instructions, LogicInstruction::addHandlers);

	}

	/* Test ASL execution */
	TEST_F(TestLogicInstruction, TestASLAccNoCarry) {	Byte testValue = (rand() & 0x7F);	testAccMode(INS_ASL_ACC, testValue, testValue << 1, 1, 0, 2); }	
	TEST_F(TestLogicInstruction, TestASLAccCarry) {		Byte testValue = (rand() | 0x80);	testAccMode(INS_ASL_ACC, testValue, testValue << 1, 0, 1, 2); }

	/* Test ROL Execution */
	TEST_F(TestLogicInstruction, TestROLAccC00) { Byte testValue = rand() & 0x7F; testAccMode(INS_ROL_ACC, testValue, (testValue << 1) & 0xFE, 0, 0, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC01) { Byte testValue = rand() | 0x80; testAccMode(INS_ROL_ACC, testValue, (testValue << 1) & 0xFE, 0, 1, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC10) { Byte testValue = rand() & 0x7F; testAccMode(INS_ROL_ACC, testValue, (testValue << 1) | 0x01, 1, 0, 2); }
	TEST_F(TestLogicInstruction, TestROLAccC11) { Byte testValue = rand() | 0x80; testAccMode(INS_ROL_ACC, testValue, (testValue << 1) | 0x01, 1, 1, 2); }


	/* Test LSR Execution */
	TEST_F(TestLogicInstruction, TestLSRAccNoCarry) {	Byte testValue = (rand() & 0xFE);	testAccMode(INS_LSR_ACC, testValue, (testValue >> 1) & 0x7F, 1, 0, 2); }
	TEST_F(TestLogicInstruction, TestLSRAccCarry) {		Byte testValue = (rand() | 0x01);	testAccMode(INS_LSR_ACC, testValue, (testValue >> 1) & 0x7F, 0, 1, 2); }
}

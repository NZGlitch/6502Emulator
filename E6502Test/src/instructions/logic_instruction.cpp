#include <gmock/gmock.h>
#include "instruction_test.h"
#include "logic_instruction.h"

namespace E6502 {
	class TestLogicInstruction : public TestInstruction {};

	/* Test defs &  addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// ASL Instructions
			{INS_ASL_ACC, 0x0A}
		};
		testInstructionDef(instructions, LogicInstruction::addHandlers);

	}

	/* Test ASL execution */
	TEST_F(TestLogicInstruction, TestASLAcc) { 
		// Given:
		Byte testValue = rand();
		Byte expectValue = (testValue << 1) & 0xFE;
		bool expectCarry = (testValue >> 7);
		u8 expectCycles = 2;
		state->A = testValue;
		(*memory)[programSpace] = INS_ASL_ACC.opcode;

		// When
		u8 cycles = cpu->execute(1);

		// Then
		EXPECT_EQ(state->A, expectValue);
		EXPECT_EQ(cycles, expectCycles);
		EXPECT_EQ(state->Flag.C, expectCarry);
		testAndResetStatusFlags(expectValue);
	}	
}

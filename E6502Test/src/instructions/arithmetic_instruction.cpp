#include <gmock/gmock.h>
#include "instruction_test.h"
#include "arithmetic_instruction.h"

namespace E6502 {
	using ::testing::_;
	using ::testing::Return;

	class TestArithmeticInstruction : public TestInstruction {};

	/* Test defs & addHandlers func */
	TEST_F(TestArithmeticInstruction, TestArithmeticHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_ADC_IMM, 0x69}
		};
		testInstructionDef(instructions, ArithmeticInstruction::addHandlers);
	}	
	
	TEST_F(TestArithmeticInstruction, TestADCImmediate) { 
		// Given:
		Byte immediateValue = rand();
		(*memory)[programSpace] = INS_ADC_IMM.opcode;
		(*memory)[programSpace + 1] = immediateValue;

		EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(immediateValue));
		EXPECT_CALL(*mockCPU, addAccumulator(_, immediateValue)).Times(1);

		// When:
		cpu->testExecute(1, mockCPU);

		// Cleanup
		initPS = state->FLAGS;
	}
}

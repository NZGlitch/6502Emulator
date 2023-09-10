#include <gmock/gmock.h>
#include "instruction_test.h"
#include "branch_instruction.h"

namespace E6502 {

	class TestBranchInstruction : public TestInstruction { };

	/* Test defs & addHandlers func */
	TEST_F(TestBranchInstruction, TestBranchHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_BCC_REL, 0x90}
		};
		testInstructionDef(instructions, BranchInstruction::addHandlers);
	}

	TEST_F(TestBranchInstruction, TestBCC) { 
		EXPECT_FALSE(true);
	}
}

#include <gmock/gmock.h>
#include "instruction_test.h"
#include "status_instruction.h"

namespace E6502 {

	class TestStatusInstruction : public TestInstruction {};

	/* Test defs & addHandlers func */
	TEST_F(TestStatusInstruction, TestStatusHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_STATUS, 0x00}
		};
		testInstructionDef(instructions, StatusInstruction::addHandlers);
	}

	TEST_F(TestStatusInstruction, TestAThing) {
		// TODO
	}
}

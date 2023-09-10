#include <gmock/gmock.h>
#include "instruction_test.h"
#include "branch_instruction.h"

namespace E6502 {

	using ::testing::_;
	using ::testing::Return;

	/* Branch logic is covered in CPU tests, so easier to mock this call than work it all out here */
	struct MockCPU : public CPUInternal {
		MockCPU() : CPUInternal(nullptr, nullptr, &InstructionUtils::loader) {}
		MOCK_METHOD(bool, getCarryFlag, (u8& cycles));
		MOCK_METHOD(void, branch, (u8& cycles, s8 offset));
		MOCK_METHOD(Byte, readPCByte, (u8& cycles));
	};

	class TestBranchInstruction : public TestInstruction {};

	/* Test defs & addHandlers func */
	TEST_F(TestBranchInstruction, TestBranchHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_BCC_REL, 0x90}
		};
		testInstructionDef(instructions, BranchInstruction::addHandlers);
	}

	TEST_F(TestBranchInstruction, TestBCCBranch) { 
		// Given:
		MockCPU*  mockCPU = new MockCPU();
		s8 offset = rand();
		(*memory)[programSpace] = INS_BCC_REL.opcode;
		(*memory)[programSpace + 1] = offset;

		// Then:
		EXPECT_CALL(*mockCPU, getCarryFlag(_)).Times(1).WillOnce(Return(false));
		EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return((Byte)offset));
		EXPECT_CALL(*mockCPU, branch(_, offset)).Times(1);

		// When:
		u8 cycles = cpu->testExecute(1, mockCPU);

		delete mockCPU;
	}

	TEST_F(TestBranchInstruction, TestBCCBranchCyclesNoPage) {
		// Given:
		state->FLAGS.bit.C = 0;
		initPS = state->FLAGS;
		programSpace &= 0xFF00;
		(*memory)[programSpace] = INS_BCC_REL.opcode;
		(*memory)[programSpace + 1] = 0x0F;

		// When:
		Byte cycles = cpu->execute(1);

		EXPECT_EQ(cycles, 3);
	}

	TEST_F(TestBranchInstruction, TestBCCBranchCyclesPage) {
		// Given:
		state->FLAGS.bit.C = 0;
		initPS = state->FLAGS;
		programSpace |= 0x00F0;
		state->PC = programSpace;
		(*memory)[programSpace] = INS_BCC_REL.opcode;
		(*memory)[programSpace + 1] = 0x1F;

		// When:
		Byte cycles = cpu->execute(1);

		EXPECT_EQ(cycles, 4);
	}

	TEST_F(TestBranchInstruction, TestBCCNoBranch) {
		state->FLAGS.bit.C = 1;
		initPS = state->FLAGS;
		(*memory)[programSpace] = INS_BCC_REL.opcode;		
		(*memory)[programSpace + 1] = 0x7F;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(programSpace + 2, state->PC);
		EXPECT_EQ(cycles, 2);
	}
}

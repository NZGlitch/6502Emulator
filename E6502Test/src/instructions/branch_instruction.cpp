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

	// Macros for making code more concise - nearly all tests are identical

	/** Mocks methods that test for flag. flagFunc is the cpu getter that must be called, flagFuncReturn is the mock result */
	#define EXPECT_FLAG_CALL(mockCPU, flagFunc, flagFuncReturn) EXPECT_CALL(mockCPU, flagFunc).Times(1).WillOnce(Return(flagFuncReturn));

	/** Tests cases where an instruction should branch
	* @param name	-	Name of the test
	* @param instruction - instruction to execute
	* @param flagFunc - matcher for method on cpu used to get relevant flag
	* @param flagFuncReturn - the result the cpu should return from flagFunc to cause instruction to branch
	*/
	#define TEST_WILL_BRANCH(name, instruction, flagFunc, flagFuncReturn)		\
		TEST_F(TestBranchInstruction, name) {									\
			EXPECT_FLAG_CALL(*mockCPU, flagFunc, flagFuncReturn);				\
			testBranch(instruction);											\
		}

	/** Tests cases where an instruction should NOT branch
	* @param name	-	Name of the test
	* @param instruction - instruction to execute
	* @param flagFunc - matcher for method on cpu used to get relevant flag
	* @param flagFuncReturn - the result the cpu should return from flagFunc to prevent instruction branching
	*/
	#define TEST_NO_BRANCH(name, instruction, flagFunc, flagFuncReturn)		\
		TEST_F(TestBranchInstruction, name) {								\
			EXPECT_FLAG_CALL(*mockCPU, flagFunc, flagFuncReturn);			\
			testNoBranch(instruction);										\
		}

	class TestBranchInstruction : public TestInstruction {
	public:

		MockCPU* mockCPU = nullptr;

		/* Helper method with common code for all branch tests. Provided mock should be configured to ensure branch is taken  */
		void testBranch(InstructionHandler instruction) {
			// Given:
			s8 offset = rand();
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = offset;

			// Then:
			EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return((Byte)offset));
			EXPECT_CALL(*mockCPU, branch(_, offset)).Times(1);

			// When:
			u8 cycles = cpu->testExecute(1, mockCPU);
		}

		void testNoBranch(InstructionHandler instruction) {
			(*memory)[programSpace] = instruction.opcode;

			// Set up method call expectations
			EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1);
			EXPECT_CALL(*mockCPU, branch(_, _)).Times(0);

			// When:
			u8 cycles = cpu->testExecute(1, mockCPU);

			// Then:
			EXPECT_EQ(cycles, 1);		// Because of the mock this will be 1 instead of 2 (unmocked readPCByte would make it 2)
		}

		// Ensure branchOnSet is true iff brancing occurs when test flag is 1, false if branching occurs when test flag is 0
		void testCyclesNoPage(InstructionHandler instruction, bool branchOnSet) {
			// Given:
			if (branchOnSet) state->FLAGS.byte = 0xFF;
			else state->FLAGS.byte = 0x00;
			initPS = state->FLAGS;
			programSpace &= 0xFF00;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = 0x0F;

			// When:
			Byte cycles = cpu->execute(1);

			EXPECT_EQ(cycles, 3);
		}

		// Ensure branchOnSet is true iff brancing occurs when test flag is 1, false if branching occurs when test flag is 0
		void testCyclesPage(InstructionHandler instruction, bool branchOnSet) {
			// Given:
			if (branchOnSet) state->FLAGS.byte = 0xFF;
			else state->FLAGS.byte = 0x00;
			initPS = state->FLAGS;
			programSpace |= 0x00F0;
			state->PC = programSpace;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = 0x1F;

			// When:
			Byte cycles = cpu->execute(1);

			EXPECT_EQ(cycles, 4);
		}

		void SetUp() {
			TestInstruction::SetUp();
			mockCPU = new MockCPU();
		}

		void TearDown() {
			TestInstruction::TearDown();
			delete mockCPU;
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestBranchInstruction, TestBranchHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_BCC_REL, 0x90}, //{INS_BCS_REL, 0xB0}
		};
		testInstructionDef(instructions, BranchInstruction::addHandlers);
	}

	// Test BCC
	TEST_WILL_BRANCH(TestBCCWillBranch, INS_BCC_REL, getCarryFlag(_), false)
	TEST_NO_BRANCH(TestBCCNoBranch, INS_BCC_REL, getCarryFlag(_), true)
	TEST_F(TestBranchInstruction, TestBCCBranchCyclesNoPage) { testCyclesNoPage(INS_BCC_REL, false); }
	TEST_F(TestBranchInstruction, TestBCCBranchCyclesPage) { testCyclesPage(INS_BCC_REL, false); }
}

#include <gmock/gmock.h>
#include "instruction_test.h"
#include "branch_instruction.h"

namespace E6502 {
	using ::testing::_;
	using ::testing::Return;

	/* Branch logic is covered in CPU tests, so easier to mock this call than work it all out here */
	struct MockCPU : public CPUInternal {
		MockCPU() : CPUInternal(nullptr, nullptr, &InstructionUtils::loader) {}
		MOCK_METHOD(bool, getFlag, (u8& cycles, u8 flag));
		MOCK_METHOD(void, branch, (u8& cycles, s8 offset));
		MOCK_METHOD(Byte, readPCByte, (u8& cycles));
	};

	// Macros for making code more concise - nearly all tests are identical

	/** Mocks methods that test for flag. flagFunc is the cpu getter that must be called, flagFuncReturn is the mock result */
	#define EXPECT_FLAG_CALL(mockCPU, flag, flagFuncReturn) EXPECT_CALL(mockCPU, getFlag(_, flag)).Times(1).WillOnce(Return(flagFuncReturn));

	/** Tests cases where an instruction should branch
	* @param name	-	Name of the test
	* @param instruction - instruction to execute
	* @param flagFunc - matcher for method on cpu used to get relevant flag
	* @param flagFuncReturn - the result the cpu should return from flagFunc to cause instruction to branch
	*/
	#define TEST_WILL_BRANCH(name, instruction, flag, flagFuncReturn)		\
		TEST_F(TestBranchInstruction, name) {								\
			EXPECT_FLAG_CALL(*mockCPU, flag, flagFuncReturn);				\
			testBranch(instruction);										\
		}

	/** Tests cases where an instruction should NOT branch
	* @param name	-	Name of the test
	* @param instruction - instruction to execute
	* @param flagFunc - matcher for method on cpu used to get relevant flag
	* @param flagFuncReturn - the result the cpu should return from flagFunc to prevent instruction branching
	*/
	#define TEST_NO_BRANCH(name, instruction, flag, flagFuncReturn)		\
		TEST_F(TestBranchInstruction, name) {								\
			EXPECT_FLAG_CALL(*mockCPU, flag, flagFuncReturn);			\
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
			{INS_BCC_REL, 0x90}, {INS_BNE_REL, 0xD0}, {INS_BPL_REL, 0x10}, {INS_BVC_REL, 0x50},
			{INS_BCS_REL, 0xB0}, {INS_BEQ_REL, 0xF0}, {INS_BMI_REL, 0x30}, {INS_BVS_REL, 0x70},
		};
		testInstructionDef(instructions, BranchInstruction::addHandlers);
	}

	// Test BCC
	TEST_WILL_BRANCH(TestBCCWillBranch, INS_BCC_REL, CPU::FLAG_CARRY, false)
	TEST_NO_BRANCH(TestBCCNoBranch, INS_BCC_REL, CPU::FLAG_CARRY, true)
	TEST_F(TestBranchInstruction, TestBCCBranchCyclesNoPage) { testCyclesNoPage(INS_BCC_REL, false); }
	TEST_F(TestBranchInstruction, TestBCCBranchCyclesPage) { testCyclesPage(INS_BCC_REL, false); }

	// Test BNE
	TEST_WILL_BRANCH(TestBNEWillBranch, INS_BNE_REL, CPU::FLAG_ZERO, false)
	TEST_NO_BRANCH(TestBNENoBranch, INS_BNE_REL, CPU::FLAG_ZERO, true)
	TEST_F(TestBranchInstruction, TestBNEBranchCyclesNoPage) { testCyclesNoPage(INS_BNE_REL, false); }
	TEST_F(TestBranchInstruction, TestBNEBranchCyclesPage) { testCyclesPage(INS_BNE_REL, false); }

	// Test BPL
	TEST_WILL_BRANCH(TestBPLWillBranch, INS_BPL_REL, CPU::FLAG_NEGATIVE, false)
	TEST_NO_BRANCH(TestBPLNoBranch, INS_BPL_REL, CPU::FLAG_NEGATIVE, true)
	TEST_F(TestBranchInstruction, TestBPLBranchCyclesNoPage) { testCyclesNoPage(INS_BPL_REL, false); }
	TEST_F(TestBranchInstruction, TestBPLBranchCyclesPage) { testCyclesPage(INS_BPL_REL, false); }

	// Test BVC
	TEST_WILL_BRANCH(TestBVCWillBranch, INS_BVC_REL, CPU::FLAG_OVERFLOW, false)
	TEST_NO_BRANCH(TestBVCNoBranch, INS_BVC_REL, CPU::FLAG_OVERFLOW, true)
	TEST_F(TestBranchInstruction, TestBVCBranchCyclesNoPage) { testCyclesNoPage(INS_BVC_REL, false); }
	TEST_F(TestBranchInstruction, TestBVCBranchCyclesPage) { testCyclesPage(INS_BVC_REL, false); }

	// Test BCS
	TEST_WILL_BRANCH(TestBCSWillBranch, INS_BCS_REL, CPU::FLAG_CARRY, true)
	TEST_NO_BRANCH(TestBCSNoBranch, INS_BCS_REL, CPU::FLAG_CARRY, false)
	TEST_F(TestBranchInstruction, TestBCSBranchCyclesNoPage) { testCyclesNoPage(INS_BCS_REL, true); }
	TEST_F(TestBranchInstruction, TestBCSBranchCyclesPage) { testCyclesPage(INS_BCS_REL, true); }

	// Test BEQ
	TEST_WILL_BRANCH(TestBEQWillBranch, INS_BEQ_REL, CPU::FLAG_ZERO, true)
	TEST_NO_BRANCH(TestBEQNoBranch, INS_BEQ_REL, CPU::FLAG_ZERO, false)
	TEST_F(TestBranchInstruction, TestBEQBranchCyclesNoPage) { testCyclesNoPage(INS_BEQ_REL, true); }
	TEST_F(TestBranchInstruction, TestBEQBranchCyclesPage) { testCyclesPage(INS_BEQ_REL, true); }

	// Test BMI
	TEST_WILL_BRANCH(TestBMIWillBranch, INS_BMI_REL, CPU::FLAG_NEGATIVE, true)
	TEST_NO_BRANCH(TestBMINoBranch, INS_BMI_REL, CPU::FLAG_NEGATIVE, false)
	TEST_F(TestBranchInstruction, TestBMIBranchCyclesNoPage) { testCyclesNoPage(INS_BMI_REL, true); }
	TEST_F(TestBranchInstruction, TestBMIBranchCyclesPage) { testCyclesPage(INS_BMI_REL, true); }

	// Test BVS
	TEST_WILL_BRANCH(TestBVSWillBranch, INS_BVS_REL, CPU::FLAG_OVERFLOW, true)
	TEST_NO_BRANCH(TestBVSNoBranch, INS_BVS_REL, CPU::FLAG_OVERFLOW, false)
	TEST_F(TestBranchInstruction, TestBVSBranchCyclesNoPage) { testCyclesNoPage(INS_BVS_REL, true); }
	TEST_F(TestBranchInstruction, TestBVSBranchCyclesPage) { testCyclesPage(INS_BVS_REL, true); }
}

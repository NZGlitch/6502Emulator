#include <gmock/gmock.h>
#include "instruction_test.h"
#include "arithmetic_instruction.h"

namespace E6502 {
	//using ::testing::_;
	//using ::testing::Return;

	/* Example CPU mock 
	struct MockCPU : public CPUInternal {
		MockCPU() : CPUInternal(nullptr, nullptr, &InstructionUtils::loader) {}
		MOCK_METHOD(bool, getFlag, (u8& cycles, u8 flag));
		MOCK_METHOD(void, branch, (u8& cycles, s8 offset));
		MOCK_METHOD(Byte, readPCByte, (u8& cycles));
	};

	*/

	class TestArithmeticInstruction : public TestInstruction {
	public:

		/* Helper method with common code for tests.*/
		void testHelper(InstructionHandler instruction) {
		}

		void SetUp() { TestInstruction::SetUp(); }

		void TearDown() { TestInstruction::TearDown(); }
	};

	/* Test defs & addHandlers func */
	TEST_F(TestArithmeticInstruction, TestArithmeticHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_ARITHMETIC, 0xFF}
		};
		testInstructionDef(instructions, BranchInstruction::addHandlers);
	}

	TEST_F(TestArithmeticInstruction, TestExample) { 
		EXPECT_TRUE(false) << "How about implementing some tests?";
	}
}

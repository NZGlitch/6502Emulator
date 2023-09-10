#include <gmock/gmock.h>
#include "instruction_test.h"
#include "incdec_instruction.h"

namespace E6502 {

	class TestIncDecInstruction : public TestInstruction {
	
	public:

		//absolute, absx, zp, zpx, implied

		/* Helper method for testing operations using implied addressing mode */
		void testImplied(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles) { }

		/* Helper method for testing operations using absolute addressing mode */
		void testAbsOp(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles) {
			// Given:
			Byte testValue = genTestValAndSetMem(dataSpace);
			Byte expectValue = op(testValue);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0xFF;
			(*memory)[programSpace + 2] = dataSpace >> 8;

			// When:
			Byte cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[dataSpace], expectValue);
			EXPECT_EQ(expectedCycles, cycles);
			testAndResetStatusFlags(expectValue);
		}

		/* Helper method for testing operations using absolute addressing-x mode */
		void testAbsXOp(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles) {}

		/* Helper method for testing operations using zero page mode */
		void testZPOp(InstructionHandler instruction, Byte(*op)(Byte v), u8 expectedCycles, bool useXIndex) { }
			
	};

	TEST_F(TestIncDecInstruction, TestFuncINC) {
		for (int v = 0; v < 0x100; v++)
			ASSERT_EQ(IncDecInstruction::INC(v), (v+1) & 0xFF);
	}

	TEST_F(TestIncDecInstruction, TestFuncDEC) {
		for (int v = 0; v < 0x100; v++)
			ASSERT_EQ(IncDecInstruction::DEC(v), (v-1) & 0xFF);
	}

	/* Test defs & addHandlers func */
	TEST_F(TestIncDecInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// DEC Instructions
			{INS_DEC_ABS, 0xCE},
			
			// DEX Instructions
			
			// DEY Instructions
			
			// INC Instructions
			{INS_INC_ABS, 0xEE},

			// INX Instructions

			// INY Instructions
			
		};
		testInstructionDef(instructions, IncDecInstruction::addHandlers);
	}

	/* Test execution */
	TEST_F(TestIncDecInstruction, TestDecAbsolute) { testAbsOp(INS_DEC_ABS, IncDecInstruction::DEC, 6); }
	TEST_F(TestIncDecInstruction, TestIncAbsolute) { testAbsOp(INS_INC_ABS, IncDecInstruction::INC, 6); }
}

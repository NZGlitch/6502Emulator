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
		void testMem(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles, bool indexed) {
			Byte mode = (instruction.opcode >> 2) & 0x7;
			// Given:
			Word targetAddress = dataSpace;
			if (indexed) {
				state->X = rand();
				targetAddress += state->X;
			}

			// Set targetto Zero page for zero page instructions
			if (mode == BaseInstruction::ADDRESS_MODE_ZERO_PAGE || mode == BaseInstruction::ADDRESS_MODE_ZERO_PAGE_X) targetAddress &= 0x00FF;

			Byte testValue = genTestValAndSetMem(targetAddress);
			Byte expectValue = op(testValue);
			(*memory)[programSpace] = instruction.opcode;

			// Add MSB if mode is absolute
			if (mode == BaseInstruction::ADDRESS_MODE_ABSOLUTE || mode == BaseInstruction::ADDRESS_MODE_ABSOLUTE_X)
				(*memory)[programSpace + 1] = dataSpace & 0xFF;

			(*memory)[programSpace + 2] = dataSpace >> 8;

			// When:
			Byte cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[targetAddress], expectValue);
			EXPECT_EQ(expectedCycles, cycles);
			testAndResetStatusFlags(expectValue);
		}			
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
			{INS_DEC_ABS, 0xCE}, {INS_DEC_ABX, 0xDE}, {INS_DEC_ZP0, 0xC6},
			
			// DEX Instructions
			
			// DEY Instructions
			
			// INC Instructions
			{INS_INC_ABS, 0xEE}, {INS_INC_ABX, 0xFE}, {INS_INC_ZP0, 0xE6},

			// INX Instructions

			// INY Instructions
			
		};
		testInstructionDef(instructions, IncDecInstruction::addHandlers);
	}

	/* Test DEC execution */
	TEST_F(TestIncDecInstruction, TestDecAbsolute) { testMem(INS_DEC_ABS, IncDecInstruction::DEC, 6, false); }
	TEST_F(TestIncDecInstruction, TestDecAbsoluteX) { testMem(INS_DEC_ABX, IncDecInstruction::DEC, 7, true); }
	TEST_F(TestIncDecInstruction, TestDecZeroPage) { testMem(INS_DEC_ZP0, IncDecInstruction::DEC, 5, false); }

	/* Test INC execution */
	TEST_F(TestIncDecInstruction, TestIncAbsolute) { testMem(INS_INC_ABS, IncDecInstruction::INC, 6, false); }
	TEST_F(TestIncDecInstruction, TestIncAbsoluteX) { testMem(INS_INC_ABX, IncDecInstruction::INC, 7, true); }
	TEST_F(TestIncDecInstruction, TestIncZeroPage) { testMem(INS_INC_ZP0, IncDecInstruction::INC, 5, false); }
}

#include <gmock/gmock.h>
#include "instruction_test.h"
#include "incdec_instruction.h"

namespace E6502 {

	class TestIncDecInstruction : public TestInstruction {
	
	public:

		//absolute, absx, zp, zpx, implied

		/* Helper method for testing operations using implied addressing mode */
		void testImplied(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles, Byte* targetReg) { 
			// Given:
			Byte testValue = genTestValAndSetTargetReg(targetReg);
			Byte expectedValue = op(testValue);
			(*memory)[programSpace] = instruction.opcode;

			// When
			Byte cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ(expectedValue, *targetReg);
			EXPECT_EQ(expectedCycles, cycles);
			testAndResetStatusFlags(expectedValue);
		}

		/* Helper method for testing memory operations */
		void testMem(InstructionHandler instruction, Byte(*op)(Byte v), Byte expectedCycles) {
			Byte mode = (instruction.opcode >> 2) & 0x7;
			bool indexed = (mode == BaseInstruction::ADDRESS_MODE_ABSOLUTE_X || mode == BaseInstruction::ADDRESS_MODE_ZERO_PAGE_X);
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
	TEST_F(TestIncDecInstruction, TestIncDecHandlers) {

		std::vector<InstructionMap> instructions = {
			// DEC Mem Instructions
			{INS_DEC_ABS, 0xCE}, {INS_DEC_ABX, 0xDE}, {INS_DEC_ZP0, 0xC6}, {INS_DEC_ZPX, 0xD6},
			
			// INC Mem Instructions
			{INS_INC_ABS, 0xEE}, {INS_INC_ABX, 0xFE}, {INS_INC_ZP0, 0xE6}, {INS_INC_ZPX, 0xF6},

			// Register Instructions
			{INS_DEX_IMP, 0xCA}, {INS_DEY_IMP, 0x88}, {INS_INX_IMP, 0xE8}, {INS_INY_IMP, 0xC8},
			
			
		};
		testInstructionDef(instructions, IncDecInstruction::addHandlers);
	}

	/* Test DEC execution */
	TEST_F(TestIncDecInstruction, TestDecAbsolute) { testMem(INS_DEC_ABS, IncDecInstruction::DEC, 6); }
	TEST_F(TestIncDecInstruction, TestDecAbsoluteX) { testMem(INS_DEC_ABX, IncDecInstruction::DEC, 7); }
	TEST_F(TestIncDecInstruction, TestDecZeroPage) { testMem(INS_DEC_ZP0, IncDecInstruction::DEC, 5); }
	TEST_F(TestIncDecInstruction, TestDecZeroPageX) { testMem(INS_DEC_ZPX, IncDecInstruction::DEC, 6); }

	/* Test INC execution */
	TEST_F(TestIncDecInstruction, TestIncAbsolute) { testMem(INS_INC_ABS, IncDecInstruction::INC, 6); }
	TEST_F(TestIncDecInstruction, TestIncAbsoluteX) { testMem(INS_INC_ABX, IncDecInstruction::INC, 7); }
	TEST_F(TestIncDecInstruction, TestIncZeroPage) { testMem(INS_INC_ZP0, IncDecInstruction::INC, 5); }
	TEST_F(TestIncDecInstruction, TestIncZeroPageX) { testMem(INS_INC_ZPX, IncDecInstruction::INC, 6); }

	/* Test Implied execution */
	TEST_F(TestIncDecInstruction, TestDEX) { testImplied(INS_DEX_IMP, IncDecInstruction::DEC, 2, &state->X); }
	TEST_F(TestIncDecInstruction, TestDEY) { testImplied(INS_DEY_IMP, IncDecInstruction::DEC, 2, &state->Y); }
	TEST_F(TestIncDecInstruction, TestINX) { testImplied(INS_INX_IMP, IncDecInstruction::INC, 2, &state->X); }
	TEST_F(TestIncDecInstruction, TestINY) { testImplied(INS_INY_IMP, IncDecInstruction::INC, 2, &state->Y); }
}

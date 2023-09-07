#include <gmock/gmock.h>
#include "instruction_test.h"
#include "load_instruction.h"

namespace E6502 {

	class TestLoadInstruction : public TestInstruction {
	public:

		/* Helper function to test Immediate instructions */
		void testImmediate(InstructionHandler instruction, Byte* targetReg, u8 expectedCycles) {
			// Fixtures
			Byte testValue = 0;

			// Given:
			testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = testValue;

			// When:
			u8 cyclesUsed = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, 2);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(InstructionHandler instruction, Byte* targetReg, u8 expectedCycles) {
			// Given:
			Byte testValue = genTestValAndClearTargetReg(targetReg);

			Word zpAddr = rand() & 0x00FF;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			(*memory)[zpAddr] = testValue;

			// When:
			u8 cyclesUsed = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			// Fixtures
			Byte baseAddress = rand();
			Byte testIndex = rand();
			Word targetAddress = (baseAddress + testIndex) & 0x00FF;
			Byte testValue = genTestValAndClearTargetReg(targetReg);
		
			// Load fixtures to memory
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = baseAddress;
			(*memory)[targetAddress] = testValue;
			state->PC = programSpace;

			// Given:
			*indexReg = testIndex;

			// When:
			u8 cyclesUsed = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, expectedCycles);
			
		}

		/* Helper function to test Absolute instructions */
		void testAbsolute(InstructionHandler instruction, Byte* targetReg, u8 expectedCycles) {
			// Given:
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0xFF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[dataSpace] = testValue;

			// When:
			u8 cyclesUsed = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, expectedCycles);
		}

		/* Helper for testing absolute instructions - this test will ensure ABS + INX will NOT cross a page boundary */
		void testAbsluteIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			dataSpace &= 0xFFF0;
			Byte index = rand() & 0x0F;			// dataSpace + index should not cross a page boundary

			*indexReg = index;
			Word targetSpace = dataSpace + index;
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0xFF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[targetSpace] = testValue;

			// Given:
			u8 cycles = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/* Helper for testing absolute instructions - this test will ensure ABS + INX WILL cross a page boundary */
		void testAbsluteIndexPage(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			dataSpace = dataSpace | 0X0080;
			Byte index = rand() | 0x80;			// Ensures that dataSpace + index will cross a page noundary

			*indexReg = index;
			Word targetSpace = dataSpace + index;
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0xFF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[targetSpace] = testValue;

			// Given:
			u8 cycles = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/** Helper for X-Indexed Zerop PAge Indirect Instructions */
		void testIndirectXIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			Byte zpBaseAddr = rand();
			Byte index = rand();
			Byte zpActualAddr = (zpBaseAddr + index) & 0xFF;

			// Given:
			(*memory)[zpActualAddr] = dataSpace & 0xFF;
			(*memory)[zpActualAddr + 1] = dataSpace >> 8;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpBaseAddr;
			(*memory)[dataSpace] = testValue;
			*indexReg = index;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(expectedCycles, cycles);
		}

		/** Helper for ZeroPage Indirect Y-Indexed (No page boundry) */
		void testIndirectYIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			Byte zpAddr = rand();
			Byte index = rand();
			Byte unIndexedAddr = dataSpace;
			Byte indexedAddr = dataSpace + index;

			// Given:
			(*memory)[zpAddr] = unIndexedAddr & 0xFF;
			(*memory)[zpAddr + 1] = unIndexedAddr >> 8;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			(*memory)[indexedAddr] = testValue;
			*indexReg = index;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(expectedCycles, cycles);
		}

		/** Helper for ZeroPage Indirect Y-Indexed (Crosses page boundry) */
		void testIndirectYIndexPage(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles) {
			dataSpace |= 0x0080;	//Ensure dataSpace is in the last half of the page, this with an index of at least 0x80 gaurantees a page boundry cross

			Byte testValue = genTestValAndClearTargetReg(targetReg);
			Byte zpAddr = rand();
			Byte index = rand() | 0x80;
			Word unIndexedAddr = dataSpace;
			Word indexedAddr = dataSpace + index;

			// Given:
			(*memory)[zpAddr] = unIndexedAddr & 0xFF;
			(*memory)[zpAddr + 1] = unIndexedAddr >> 8;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			(*memory)[indexedAddr] = testValue;
			*indexReg = index;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			testAndResetStatusFlags(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(expectedCycles, cycles);
		}
	};

	/* Test addHandlers function and instruction opcodes */
	TEST_F(TestLoadInstruction, TestLDAaddHandlers) {
		std::vector<InstructionMap> instructions = {
			// LDA Instructions 
			{INS_LDA_IMM, 0xA9},
			{INS_LDA_ZP, 0xA5},
			{INS_LDA_ZPX, 0xB5},
			{INS_LDA_ABS, 0xAD},
			{INS_LDA_ABSX, 0xBD},
			{INS_LDA_ABSY, 0xB9},
			{INS_LDA_INDX, 0xA1},
			{INS_LDA_INDY, 0xB1},

			// LDX Instructions
			{INS_LDX_IMM, 0xA2},
			{INS_LDX_ZP, 0xA6},
			{INS_LDX_ZPY, 0xB6},
			{INS_LDX_ABS, 0xAE},
			{INS_LDX_ABSY, 0xBE},

			// LDY Instructions
			{INS_LDY_IMM, 0xA0},
			{INS_LDY_ZP, 0xA4},
			{INS_LDY_ZPX, 0xB4},
			{INS_LDY_ABS, 0xAC},
			{INS_LDY_ABSX, 0xBC},
		};
		testInstructionDef(instructions, LoadInstruction::addHandlers);
	}

	/* Test fetchAndSaveToRegister */
	TEST_F(TestLoadInstruction, TestfetchAndSave) {
		Byte* registers[] = { &state->A, &state->X, &state->Y };
		u8 registerNames[] = { CPU::REGISTER_A, CPU::REGISTER_X, CPU::REGISTER_Y };
		Byte testValues[] = { 0x21, 0x42, 0x84 };
		Word testAddresses[] = { 0x1234, 0xf167, 0x0200 };

		for (int i = 0; i < 3; i++) {
			// Given
			(*memory)[testAddresses[i]] = testValues[i];
			*registers[i] = ~testValues[i];
			u8 cycles = 0;

			// When:
			LoadInstruction::fetchAndSaveToRegister(&cycles, cpu, testAddresses[i], registerNames[i]);

			// Then:
			testAndResetStatusFlags(testValues[i]);
			EXPECT_EQ(*registers[i], testValues[i]);
			EXPECT_EQ(cycles, 1);
		}

	}

	 /* Tests LDA Immediate Instruction */
	TEST_F(TestLoadInstruction, TestLoadAImmediate) { testImmediate(INS_LDA_IMM, &state->A, 2); }
	TEST_F(TestLoadInstruction, TestLoadXImmediate) { testImmediate(INS_LDX_IMM, &state->X, 2); }
	TEST_F(TestLoadInstruction, TestLoadYImmediate) { testImmediate(INS_LDY_IMM, &state->Y, 2); }
	
	/* Tests LD Zero Page Instruction */
	TEST_F(TestLoadInstruction, TestLoadAZeroPage) { testZeroPage(INS_LDA_ZP, &state->A, 3); }
	TEST_F(TestLoadInstruction, TestLoadXZeroPage) { testZeroPage(INS_LDX_ZP, &state->X, 3); }
	TEST_F(TestLoadInstruction, TestLoadYZeroPage) { testZeroPage(INS_LDY_ZP, &state->Y, 3); }

	/* Tests LD Zero Page,X/Y Instruction */
	TEST_F(TestLoadInstruction, TestLoadAZeroPageX) { testZeroPageIndex(INS_LDA_ZPX, &state->X, &state->A, 4); }
	TEST_F(TestLoadInstruction, TestLoadXZeroPageX) { testZeroPageIndex(INS_LDX_ZPY, &state->Y, &state->X, 4); }
	TEST_F(TestLoadInstruction, TestLoadYZeroPageX) { testZeroPageIndex(INS_LDY_ZPX, &state->X, &state->Y, 4); }

	/* Tests LD Absolute Instruction */
	TEST_F(TestLoadInstruction, TestLoadAAbsolute) { testAbsolute(INS_LDA_ABS, &state->A, 4); }
	TEST_F(TestLoadInstruction, TestLoadXAbsolute) { testAbsolute(INS_LDX_ABS, &state->X, 4); }
	TEST_F(TestLoadInstruction, TestLoadYAbsolute) { testAbsolute(INS_LDY_ABS, &state->Y, 4); }

	/* Tests LD Absolute,X/Y Instruction  (No page) */
	TEST_F(TestLoadInstruction, TestLoadAAbsoluteX) { testAbsluteIndex(INS_LDA_ABSX, &state->X, &state->A, 4); }
	TEST_F(TestLoadInstruction, TestLoadXAbsoluteY) { testAbsluteIndex(INS_LDX_ABSY, &state->Y, &state->X, 4); }
	TEST_F(TestLoadInstruction, TestLoadYAbsoluteX) { testAbsluteIndex(INS_LDY_ABSX, &state->X, &state->Y, 4); }

	/* Tests LD Absolute,X/Y Instruction (cross page boundary)  */
	TEST_F(TestLoadInstruction, TestLoadAAbsoluteXPage) { testAbsluteIndexPage(INS_LDA_ABSX, &state->X, &state->A, 5); }
	TEST_F(TestLoadInstruction, TestLoadAAbsoluteYPage) { testAbsluteIndexPage(INS_LDA_ABSY, &state->Y, &state->A, 5); }
	TEST_F(TestLoadInstruction, TestLoadXAbsoluteYPage) { testAbsluteIndexPage(INS_LDX_ABSY, &state->Y, &state->X, 5); }
	TEST_F(TestLoadInstruction, TestLoadYAbsoluteXPage) { testAbsluteIndexPage(INS_LDY_ABSX, &state->X, &state->Y, 5); }

	/* Tests LD Indeirect,X Instruction */
	TEST_F(TestLoadInstruction, TestLoadIndirectX) { testIndirectXIndex(INS_LDA_INDX, &state->X, &state->A, 6); }

	/* Tests LD Indeirect,Y Instruction */
	TEST_F(TestLoadInstruction, TestLoadIndirectY) { testIndirectYIndex(INS_LDA_INDY, &state->Y, &state->A, 5); }
	TEST_F(TestLoadInstruction, TestLoadIndirectYPage) { testIndirectYIndexPage(INS_LDA_INDY, &state->Y, &state->A, 6); }
}

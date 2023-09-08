#include <gmock/gmock.h>
#include "instruction_test.h"
#include "store_instruction.h"

namespace E6502 {

	class TestStoreInstruction : public TestInstruction {
	public:

		/* Helper function to test Absolute instructions */
		void testAbsolute(InstructionHandler instruction, Byte* sourceReg, u8 expectedCycles) {
			// Given:
			Byte testValue = genTestValAndTargetClearMem(dataSpace);
			*sourceReg = testValue;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			
			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[dataSpace], testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/** Helper function to test Absolute Indexed instructions */
		void testAbsoluteIndexed(InstructionHandler instruction, Byte* indexReg, Byte* sourceReg, u8 expectedCycles) {
			// Given:
			Byte index = rand();
			Word targetAddr = dataSpace + index;
			Byte testValue = genTestValAndTargetClearMem(targetAddr);

			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8 ;
			*sourceReg = testValue;
			*indexReg = index;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ(cycles, expectedCycles);
			EXPECT_EQ((*memory)[targetAddr], testValue);
		}

		/** Helper function for Zero Page instructions */
		void testZeroPage(InstructionHandler instruction, Byte* sourceReg, u8 expectedCycles) {
			// Given:
			Byte zpAddr = rand();
			Byte testValue = genTestValAndTargetClearMem(0x00FF & zpAddr);
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			*sourceReg = testValue;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[zpAddr], testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/** Helper method for Zero PAge Indexed instructions */
		void testZeroPageIndex(InstructionHandler instruction, Byte* indexReg, Byte* sourceReg, u8 expectedCycles) {
			// Fixtures
			Byte zpBaseAddr = rand();
			Byte testIndex = rand();
			Word targetAddress = (zpBaseAddr + testIndex) & 0x00FF;
			Byte testValue = genTestValAndTargetClearMem(targetAddress);

			// Given
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpBaseAddr;
			*sourceReg = testValue;
			*indexReg = testIndex;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[targetAddress], testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/** Helper for X-Indexed Zero Page Indirect instructions */
		void testIndirectXIndex(InstructionHandler instruction, Byte* sourceReg, u8 expectedCycles) {
			Byte testValue = genTestValAndTargetClearMem(dataSpace);
			Byte zpBaseAddress = rand();
			Byte testIndex = rand();
			Byte zpActualAddr = (zpBaseAddress + testIndex) & 0x00FF;

			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpBaseAddress;
			(*memory)[zpActualAddr] = dataSpace & 0x00FF;
			(*memory)[zpActualAddr + 1] = dataSpace >> 8;
			*sourceReg = testValue;
			state->X = testIndex;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ((*memory)[dataSpace], testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}

		/** Helper for Zero Page Y-Indexed Indirect instructions */
		void testIndirectYIndex(InstructionHandler instruction, Byte* sourceReg, u8 expectedCycles) {
			Byte testIndex = rand();
			Word targetAddress = dataSpace + testIndex;
			Byte testValue = genTestValAndTargetClearMem(targetAddress);
			Byte zpAddress = rand();
	
			// Given:
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddress;
			(*memory)[zpAddress] = dataSpace & 0x00FF;
			(*memory)[zpAddress + 1] = dataSpace >> 8;
			*sourceReg = testValue;
			state->Y = testIndex;
	
			// When:
			u8 cycles = cpu->execute(1);
				
			// Then:
			EXPECT_EQ((*memory)[targetAddress], testValue);
			EXPECT_EQ(cycles, expectedCycles);
		}
	};

	/* Test addHandlers func adds ST handlers and instructions have the correct opcodes */
	TEST_F(TestStoreInstruction, TestStoreAddHandlers) {

		std::vector<InstructionMap> instructions = {
		/* Absolute and Absolute Indexed Instructions */
		{INS_STA_ABS, 0x8D},
		{INS_STX_ABS, 0x8E},
		{INS_STY_ABS, 0x8C},
		{INS_STA_ABSX, 0x9D},
		{INS_STA_ABSY, 0x99},

		/* Zero Page Instructions */
		{INS_STA_ZP, 0x85},
		{INS_STX_ZP, 0x86},
		{INS_STY_ZP, 0x84},

		/* Zero Page X,Y Indexed Instructions */
		{INS_STA_ZPX, 0x95},
		{INS_STX_ZPY, 0x96},
		{INS_STY_ZPX, 0x94},

		/* X-Indexed Zero Page Indirect instructions */
		{INS_STA_INDX, 0x81},

		/* Zero Page Y-Indexed Indirect instructions */
		{INS_STA_INDY, 0x91},

		};
		testInstructionDef(instructions, StoreInstruction::addHandlers);
	}

	 /* Test Absolute Mode execution */
	TEST_F(TestStoreInstruction, TestStoreAAbsolute) { testAbsolute(INS_STA_ABS, &state->A, 4); }
	TEST_F(TestStoreInstruction, TestStoreXAbsolute) { testAbsolute(INS_STX_ABS, &state->X, 4); }
	TEST_F(TestStoreInstruction, TestStoreYAbsolute) { testAbsolute(INS_STY_ABS, &state->Y, 4); }

	/* Tests STA Absolute,X/Y Instructions */
	TEST_F(TestStoreInstruction, TestStoreAbsoluteX) { testAbsoluteIndexed(INS_STA_ABSX, &state->X, &state->A, 5); }
	TEST_F(TestStoreInstruction, TestStoreAbsoluteY) { testAbsoluteIndexed(INS_STA_ABSY, &state->Y, &state->A, 5); }

	/* Tests LD Zero Page Instruction */
	TEST_F(TestStoreInstruction, TestLStoreAZeroPage) { testZeroPage(INS_STA_ZP, &state->A, 3); }
	TEST_F(TestStoreInstruction, TestLStoreXZeroPage) { testZeroPage(INS_STX_ZP, &state->X, 3); }
	TEST_F(TestStoreInstruction, TestLStoreYZeroPage) { testZeroPage(INS_STY_ZP, &state->Y, 3); }

	/* Tests Store Zero Page,X/Y Instruction */
	TEST_F(TestStoreInstruction, TestStoreAZeroPageX) { testZeroPageIndex(INS_STA_ZPX, &state->X, &state->A, 4); }
	TEST_F(TestStoreInstruction, TestStoreXZeroPageY) { testZeroPageIndex(INS_STX_ZPY, &state->Y, &state->X, 4); }
	TEST_F(TestStoreInstruction, TestStoreYZeroPageX) { testZeroPageIndex(INS_STY_ZPX, &state->X, &state->Y, 4); }

	/* Tests Store Indirect,X Instruction */
	TEST_F(TestStoreInstruction, TestStoreIndirectX) { testIndirectXIndex(INS_STA_INDX, &state->A, 6); }

	/* Tests Store Indirect,Y Instruction */
	TEST_F(TestStoreInstruction, TestStoreIndirectY) { testIndirectYIndex(INS_STA_INDY, &state->A, 6); }
}

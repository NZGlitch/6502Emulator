#include <gmock/gmock.h>
#include "instruction_test.h"
#include "logic_instruction.h"

namespace E6502 {

	struct LogicFixture {
		Byte operandA;
		Byte operandB;
		Byte result;
	};

	class TestLogicInstruction : public TestInstruction {
	private:	
		/* Reused code for test setup for helpers below */
		LogicFixture prepareTest(InstructionHandler instruction, Byte(*op)(Byte a, Byte b)) {
			Byte a = rand();
			Byte b = rand();
			Byte expectedResult = op(a, b);
			(*memory)[programSpace] = instruction.opcode;
			return  { a,b,expectedResult };
		}

		/* Reused code for finishing tests for helpers below */
		void finishTest(Byte expectedResult, u8 expectedCycles, u8 actualCycles) {
			// Then
			EXPECT_EQ(expectedResult, state->A);
			EXPECT_EQ(expectedCycles, actualCycles);
			testAndResetStatusFlags(expectedResult);
		}

	public:
		/* Helper method for testing operations using immediate addressing mode */
		void testImmOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) {
			// Given
			LogicFixture fixtures = prepareTest(instruction, op);
			state->A = fixtures.operandA;
			(*memory)[programSpace + 1] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			finishTest(fixtures.result, expectedCycles, cycles);
		}

		/* Helper method for testing operations using absolute addressing mode */
		void testAbsOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) {
			// Given
			LogicFixture fixtures = prepareTest(instruction, op);
			state->A = fixtures.operandA;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[dataSpace] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			if (instruction == INS_BIT_ABS) {	// BIT instructions dont save result
				EXPECT_EQ(state->A, fixtures.operandA);
				EXPECT_EQ((*memory)[dataSpace], fixtures.operandB);
				testAndResetStatusFlags(fixtures.result);
			}
			else {
				finishTest(fixtures.result, expectedCycles, cycles);
			}
		}

		/* Helper method for testing operations using absolute addressing-x mode */
		void testAbsIdxOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte* indexReg, Byte expectedCycles, bool forcePage) {
			Byte index = rand();
			if (forcePage) {
				// Make sure that dataSpace + Index crosses a page
				dataSpace|= 0x80; index|= 0x80;
			}
			else {
				// Make sure dataSpace + Index does not cross a page
				dataSpace &= 0xFF7F; index &= 0x7F;
			}
			Word targetAddr = dataSpace + index;

			// Given
			LogicFixture fixtures = prepareTest(instruction, op);
			state->A = fixtures.operandA;
			*indexReg = index;

			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[targetAddr] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			finishTest(fixtures.result, expectedCycles, cycles);
		}

		/* Helper method for testing operations using zero page mode */
		void testZPOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), u8 expectedCycles, bool useXIndex) {
			// Given:
			LogicFixture fixtures = prepareTest(instruction, op);
			Byte zpBase = rand();
			Byte index = 0;
			if (useXIndex) {
				index = rand();
				state->X = index;
			}
			Word zpTarget = (0x00FF & (zpBase + index));
			state->A = fixtures.operandA;
			(*memory)[programSpace + 1] = zpBase;
			(*memory)[zpTarget] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			if (instruction == INS_BIT_ZP0) {	// BIT instructions dont save result
				EXPECT_EQ(state->A, fixtures.operandA);
				EXPECT_EQ((*memory)[zpTarget], fixtures.operandB);
				testAndResetStatusFlags(fixtures.result);
			}
			else {
				finishTest(fixtures.result, expectedCycles, cycles);
			}
		}

		/* Helper method for testing operations using X-indexed zero page mode */
		void testXIndOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) {
			// Prepare fixtures and addresses
			LogicFixture fixtures = prepareTest(instruction, op);
			dataSpace |= (rand() & 0x00FF);
			Byte index = rand();
			Byte zpBase = rand();
			Word zpAddr = ((zpBase + index) & 0x00FF);

			// Given:
			(*memory)[programSpace + 1] = zpBase;
			state->X = index;
			(*memory)[zpAddr] = dataSpace & 0x00FF;
			(*memory)[zpAddr + 1] = dataSpace >> 8;
			state->A = fixtures.operandA;
			(*memory)[dataSpace] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			finishTest(fixtures.result, expectedCycles, cycles);
		}

		/* Helper method for testing operations using zreo page Y-indexed page mode */
		void testYIndOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles, bool forcePage) {
			// Prepare fixtures and addresses
			LogicFixture fixtures = prepareTest(instruction, op);
			Byte index = rand();
			dataSpace |= (rand() & 0x00FF);
			if (forcePage) {
				// ensure zpTarget + index > 0xFF
				dataSpace &= 0xFF7F;
				index |= 0x80;
			}
			else {
				// ensure zpTarget + index < 0xFF
				dataSpace |= 0x0080;
				index &= 0x7F;
			}
			
			Byte zpAddr = rand();
			Word zpTarget = dataSpace - index;

			// Given:
			(*memory)[programSpace + 1] = zpAddr;
			state->Y = index;
			(*memory)[zpAddr] = zpTarget & 0x00FF;
			(*memory)[zpAddr + 1] = zpTarget >> 8;
			state->A = fixtures.operandA;
			(*memory)[dataSpace] = fixtures.operandB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			finishTest(fixtures.result, expectedCycles, cycles);
		}
	};

	TEST_F(TestLogicInstruction, TestFuncAND) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::AND(a, b), a & b);
	}

	TEST_F(TestLogicInstruction, TestFuncEOR) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::EOR(a, b), a ^ b);
	}

	TEST_F(TestLogicInstruction, TestFuncORA) {
		for (int a = 0; a < 0x100; a++)
			for (int b = 0; b < 0x100; b++)
				ASSERT_EQ(LogicInstruction::ORA(a, b), a | b);
	}

	/* Test defs & addHandlers func */
	TEST_F(TestLogicInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// EOR Instructions
			{INS_EOR_IMM, 0x49}, {INS_EOR_ABS, 0x4D}, {INS_EOR_ABX, 0x5D}, {INS_EOR_ABY, 0x59}, {INS_EOR_ZP0, 0x45},
			{INS_EOR_ZPX, 0x55}, {INS_EOR_INX, 0x41}, {INS_EOR_INY, 0x51},

			// AND Instructions
			{INS_AND_IMM, 0x29}, {INS_AND_ABS, 0x2D}, {INS_AND_ABX, 0x3D}, {INS_AND_ABY, 0x39}, {INS_AND_ZP0, 0x25},
			{INS_AND_ZPX, 0x35}, {INS_AND_INX, 0x21}, {INS_AND_INY, 0x31},

			// ORA Instructions
			{INS_ORA_IMM, 0x09}, {INS_ORA_ABS, 0x0D}, {INS_ORA_ABX, 0x1D}, {INS_ORA_ABY, 0x19}, {INS_ORA_ZP0, 0x05},
			{INS_ORA_ZPX, 0x15}, {INS_ORA_INX, 0x01}, {INS_ORA_INY, 0x11},

			// BIT Instructions
			{INS_BIT_ABS, 0x2C}, {INS_BIT_ZP0, 0x24},
		};
		testInstructionDef(instructions, LogicInstruction::addHandlers);
	}

	/* Test EOR execution */
	TEST_F(TestLogicInstruction, TestEORImmediate) { testImmOp(INS_EOR_IMM, LogicInstruction::EOR, 2); }
	TEST_F(TestLogicInstruction, TestEORbAsolute) { testAbsOp(INS_EOR_ABS, LogicInstruction::EOR, 4); }
	TEST_F(TestLogicInstruction, TestEORbAsoluteXNoPage) { testAbsIdxOp(INS_EOR_ABX, LogicInstruction::EOR, &state->X, 4, false); }
	TEST_F(TestLogicInstruction, TestEORbAsoluteXPage) { testAbsIdxOp(INS_EOR_ABX, LogicInstruction::EOR, &state->X, 5, true); }
	TEST_F(TestLogicInstruction, TestEORbAsoluteYNoPage) { testAbsIdxOp(INS_EOR_ABY, LogicInstruction::EOR, &state->Y, 4, false); }
	TEST_F(TestLogicInstruction, TestEORbAsoluteYPage) { testAbsIdxOp(INS_EOR_ABY, LogicInstruction::EOR, &state->Y, 5, true); }
	TEST_F(TestLogicInstruction, TestEORZeroPage) { testZPOp(INS_EOR_ZP0, LogicInstruction::EOR, 3, false); }
	TEST_F(TestLogicInstruction, TestEORZeroPageX) { testZPOp(INS_EOR_ZPX, LogicInstruction::EOR, 4, true); }
	TEST_F(TestLogicInstruction, TestEORXIndirect) { testXIndOp(INS_EOR_INX, LogicInstruction::EOR, 6); }
	TEST_F(TestLogicInstruction, TestEORYIndirectNoPage) { testYIndOp(INS_EOR_INY, LogicInstruction::EOR, 5, false); }
	TEST_F(TestLogicInstruction, TestEORYIndirectPage) { testYIndOp(INS_EOR_INY, LogicInstruction::EOR, 6, true); }

	/* Test AND execution */
	TEST_F(TestLogicInstruction, TestANDImmediate) { testImmOp(INS_AND_IMM, LogicInstruction::AND, 2);  }
	TEST_F(TestLogicInstruction, TestANDAbsolute) { testAbsOp(INS_AND_ABS, LogicInstruction::AND, 4); }
	TEST_F(TestLogicInstruction, TestANDAbsoluteXNoPage) { testAbsIdxOp(INS_AND_ABX, LogicInstruction::AND, &state->X, 4, false); }
	TEST_F(TestLogicInstruction, TestANDAbsoluteXPage) { testAbsIdxOp(INS_AND_ABX, LogicInstruction::AND, &state->X, 5, true); }
	TEST_F(TestLogicInstruction, TestANDAbsoluteYNoPage) { testAbsIdxOp(INS_AND_ABY, LogicInstruction::AND, &state->Y, 4, false); }
	TEST_F(TestLogicInstruction, TestANDAbsoluteYPage) { testAbsIdxOp(INS_AND_ABY, LogicInstruction::AND, &state->Y, 5, true); }
	TEST_F(TestLogicInstruction, TestANDZeroPage) { testZPOp(INS_AND_ZP0, LogicInstruction::AND, 3, false); }
	TEST_F(TestLogicInstruction, TestANDZeroPageX) { testZPOp(INS_AND_ZPX, LogicInstruction::AND, 4, true); }
	TEST_F(TestLogicInstruction, TestANDXIndirect) { testXIndOp(INS_AND_INX, LogicInstruction::AND, 6); }
	TEST_F(TestLogicInstruction, TestANDYIndirectNoPage) { testYIndOp(INS_AND_INY, LogicInstruction::AND, 5, false); }
	TEST_F(TestLogicInstruction, TestANDYIndirectPage) { testYIndOp(INS_AND_INY, LogicInstruction::AND, 6, true); }
	
	/* Test ORA execution */
	TEST_F(TestLogicInstruction, TestORAImmediate) { testImmOp(INS_ORA_IMM, LogicInstruction::ORA, 2); }
	TEST_F(TestLogicInstruction, TestORAAbsolute) { testAbsOp(INS_ORA_ABS, LogicInstruction::ORA, 4); }
	TEST_F(TestLogicInstruction, TestORAAbsoluteXNoPage) { testAbsIdxOp(INS_ORA_ABX, LogicInstruction::ORA, &state->X, 4, false); }
	TEST_F(TestLogicInstruction, TestORAAbsoluteXPage) { testAbsIdxOp(INS_ORA_ABX, LogicInstruction::ORA, &state->X, 5, true); }
	TEST_F(TestLogicInstruction, TestORAAbsoluteYNoPage) { testAbsIdxOp(INS_ORA_ABY, LogicInstruction::ORA, &state->Y, 4, false); }
	TEST_F(TestLogicInstruction, TestORAAbsoluteYPage) { testAbsIdxOp(INS_ORA_ABY, LogicInstruction::ORA, &state->Y, 5, true); }
	TEST_F(TestLogicInstruction, TestORAZeroPage) { testZPOp(INS_ORA_ZP0, LogicInstruction::ORA, 3, false); }
	TEST_F(TestLogicInstruction, TestORAZeroPageX) { testZPOp(INS_ORA_ZPX, LogicInstruction::ORA, 4, true); }
	TEST_F(TestLogicInstruction, TestORAXIndirect) { testXIndOp(INS_ORA_INX, LogicInstruction::ORA, 6); }
	TEST_F(TestLogicInstruction, TestORAYIndirectNoPage) { testYIndOp(INS_ORA_INY, LogicInstruction::ORA, 5, false); }
	TEST_F(TestLogicInstruction, TestORAYIndirectPage) { testYIndOp(INS_ORA_INY, LogicInstruction::ORA, 6, true); }

	/* Test BIT execution */
	TEST_F(TestLogicInstruction, TestBITAbsolute) { testAbsOp(INS_BIT_ABS, LogicInstruction::AND, 4); }
	TEST_F(TestLogicInstruction, TestBITZeroPage) { testZPOp(INS_BIT_ZP0, LogicInstruction::AND, 2, false); }
}

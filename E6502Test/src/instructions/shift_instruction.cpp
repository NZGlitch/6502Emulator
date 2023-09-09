#include <gmock/gmock.h>
#include "instruction_test.h"
#include "shift_instruction.h"

namespace E6502 {
	class TestShiftInstruction : public TestInstruction {
	private:
		
		/* Reused code for finishing tests from helpers below */
		void finishTest(Byte expectValue, bool carryOut) {
			// Then
			EXPECT_EQ(state->A, expectValue);
			EXPECT_EQ(state->FLAGS.bit.C, carryOut);
			testAndResetStatusFlags(expectValue);
		}

	public:

		/* Helper method for testing operations using accumulator addressing mode */
		void testAccOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = testValue;
			state->FLAGS.bit.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(cycles, expectCycles);
			finishTest(expectValue, carryOut);
		}

		/* Helper method for testing operations using absolute addressing mode */
		void testAbsOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = ~testValue;
			dataSpace |= (rand() & 0xFF);	// Pick a random location in the data page
			state->FLAGS.bit.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0xFF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[dataSpace] = testValue;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(cycles, expectCycles);
			finishTest(expectValue, carryOut);
		}

		void testAbsXOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = ~testValue;
			dataSpace |= (rand() & 0xFF);	// Pick a random location in the data page
			Byte index = rand();
			Word abs = dataSpace - index;
			state->FLAGS.bit.C = carryIn;
			state->X = index;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = abs & 0xFF;
			(*memory)[programSpace + 2] = abs >> 8;
			(*memory)[dataSpace] = testValue;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(cycles, expectCycles);
			finishTest(expectValue, carryOut);
		}

		void testZPOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = ~testValue;
			Byte zpAddr = rand();
			state->FLAGS.bit.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			(*memory)[0x00FF & zpAddr] = testValue;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(cycles, expectCycles);
			finishTest(expectValue, carryOut);
		}

		void testZPXOp(InstructionHandler instruction, Byte testValue, Byte expectValue, bool carryIn, bool carryOut, u8 expectCycles) {
			// Given:
			state->A = ~testValue;
			Byte zpAddr = rand();
			Byte index = rand();
			state->X = index;
			Word expectAddr = 0x00FF & (zpAddr + index);
			state->FLAGS.bit.C = carryIn;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpAddr;
			(*memory)[expectAddr] = testValue;

			// When
			u8 cycles = cpu->execute(1);

			// Then
			EXPECT_EQ(cycles, expectCycles);
			finishTest(expectValue, carryOut);
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestShiftInstruction, TestShiftHandlers) {

		std::vector<InstructionMap> instructions = {
			// ASL Instructions
			{INS_ASL_ACC, 0x0A}, {INS_ASL_ABS, 0x0E}, {INS_ASL_ABX, 0x1E}, {INS_ASL_ZP0, 0x06}, {INS_ASL_ZPX, 0x16},

			// ROL Instructions
			{INS_ROL_ACC, 0x2A}, {INS_ROL_ABS, 0x2E}, {INS_ROL_ABX, 0x3E}, {INS_ROL_ZP0, 0x26}, {INS_ROL_ZPX, 0x36},

			// LSR Instructions
			{INS_LSR_ACC, 0x4A}, {INS_LSR_ABS, 0x4E}, {INS_LSR_ABX, 0x5E}, {INS_LSR_ZP0, 0x46}, {INS_LSR_ZPX, 0x56},

			// ROR Instructions
			{INS_ROR_ACC, 0x6A}, {INS_ROR_ABS, 0x6E}, {INS_ROR_ABX, 0x7E}, {INS_ROR_ZP0, 0x66}, {INS_ROR_ZPX, 0x76},
		};
		testInstructionDef(instructions, ShiftInstruction::addHandlers);
	}

	/* Test ASL execution */
	// ACC
	TEST_F(TestShiftInstruction, TestASLAccNoCarry) { Byte tVal = (rand() & 0x7F);	testAccOp(INS_ASL_ACC, tVal, tVal << 1, 1, 0, 2); }	
	TEST_F(TestShiftInstruction, TestASLAccWiCarry) { Byte tVal = (rand() | 0x80);	testAccOp(INS_ASL_ACC, tVal, tVal << 1, 0, 1, 2); }

	// ABS
	TEST_F(TestShiftInstruction, TestASLAbsNoCarry) { Byte tVal = (rand() & 0x7F);	testAbsOp(INS_ASL_ABS, tVal, tVal << 1, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestASLAbsWiCarry) { Byte tVal = (rand() | 0x80);	testAbsOp(INS_ASL_ABS, tVal, tVal << 1, 0, 1, 6); }

	// ABS-X
	TEST_F(TestShiftInstruction, TestASLAbsXNoCarry) { Byte tVal = (rand() & 0x7F);	testAbsXOp(INS_ASL_ABX, tVal, tVal << 1, 1, 0, 7); }
	TEST_F(TestShiftInstruction, TestASLAbsXWiCarry) { Byte tVal = (rand() | 0x80);	testAbsXOp(INS_ASL_ABX, tVal, tVal << 1, 0, 1, 7); }

	// ZeroPage
	TEST_F(TestShiftInstruction, TestASLZPNoCarry) { Byte tVal = (rand() & 0x7F);	testZPOp(INS_ASL_ZP0, tVal, tVal << 1, 1, 0, 5); }
	TEST_F(TestShiftInstruction, TestASLZPWiCarry) { Byte tVal = (rand() | 0x80);	testZPOp(INS_ASL_ZP0, tVal, tVal << 1, 0, 1, 5); }

	// X-Indexed ZeroPage 
	TEST_F(TestShiftInstruction, TestASLZPXNoCarry) { Byte tVal = (rand() & 0x7F);	testZPXOp(INS_ASL_ZPX, tVal, tVal << 1, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestASLZPXWiCarry) { Byte tVal = (rand() | 0x80);	testZPXOp(INS_ASL_ZPX, tVal, tVal << 1, 0, 1, 6); }

	/* Test ROL Execution */
	// ACC
	TEST_F(TestShiftInstruction, TestROLAccC00) { Byte tVal = rand() & 0x7F; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) & 0xFE, 0, 0, 2); }
	TEST_F(TestShiftInstruction, TestROLAccC01) { Byte tVal = rand() | 0x80; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) & 0xFE, 0, 1, 2); }
	TEST_F(TestShiftInstruction, TestROLAccC10) { Byte tVal = rand() & 0x7F; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) | 0x01, 1, 0, 2); }
	TEST_F(TestShiftInstruction, TestROLAccC11) { Byte tVal = rand() | 0x80; testAccOp(INS_ROL_ACC, tVal, (tVal << 1) | 0x01, 1, 1, 2); }

	// ABS
	TEST_F(TestShiftInstruction, TestROLAbsC00) { Byte tVal = rand() & 0x7F; testAbsOp(INS_ROL_ABS, tVal, (tVal << 1) & 0xFE, 0, 0, 6); }
	TEST_F(TestShiftInstruction, TestROLAbsC01) { Byte tVal = rand() | 0x80; testAbsOp(INS_ROL_ABS, tVal, (tVal << 1) & 0xFE, 0, 1, 6); }
	TEST_F(TestShiftInstruction, TestROLAbsC10) { Byte tVal = rand() & 0x7F; testAbsOp(INS_ROL_ABS, tVal, (tVal << 1) | 0x01, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestROLAbsC11) { Byte tVal = rand() | 0x80; testAbsOp(INS_ROL_ABS, tVal, (tVal << 1) | 0x01, 1, 1, 6); }

	// ABS-X
	TEST_F(TestShiftInstruction, TestROLAbsXC00) { Byte tVal = rand() & 0x7F; testAbsXOp(INS_ROL_ABX, tVal, (tVal << 1) & 0xFE, 0, 0, 7); }
	TEST_F(TestShiftInstruction, TestROLAbsXC01) { Byte tVal = rand() | 0x80; testAbsXOp(INS_ROL_ABX, tVal, (tVal << 1) & 0xFE, 0, 1, 7); }
	TEST_F(TestShiftInstruction, TestROLAbsXC10) { Byte tVal = rand() & 0x7F; testAbsXOp(INS_ROL_ABX, tVal, (tVal << 1) | 0x01, 1, 0, 7); }
	TEST_F(TestShiftInstruction, TestROLAbsXC11) { Byte tVal = rand() | 0x80; testAbsXOp(INS_ROL_ABX, tVal, (tVal << 1) | 0x01, 1, 1, 7); }

	// Zero Page
	TEST_F(TestShiftInstruction, TestROLZPC00) { Byte tVal = rand() & 0x7F; testZPOp(INS_ROL_ZP0, tVal, (tVal << 1) & 0xFE, 0, 0, 5); }
	TEST_F(TestShiftInstruction, TestROLZPC01) { Byte tVal = rand() | 0x80; testZPOp(INS_ROL_ZP0, tVal, (tVal << 1) & 0xFE, 0, 1, 5); }
	TEST_F(TestShiftInstruction, TestROLZPC10) { Byte tVal = rand() & 0x7F; testZPOp(INS_ROL_ZP0, tVal, (tVal << 1) | 0x01, 1, 0, 5); }
	TEST_F(TestShiftInstruction, TestROLZPC11) { Byte tVal = rand() | 0x80; testZPOp(INS_ROL_ZP0, tVal, (tVal << 1) | 0x01, 1, 1, 5); }

	// X-Indexed Zero Page
	TEST_F(TestShiftInstruction, TestROLZPXC00) { Byte tVal = rand() & 0x7F; testZPXOp(INS_ROL_ZPX, tVal, (tVal << 1) & 0xFE, 0, 0, 6); }
	TEST_F(TestShiftInstruction, TestROLZPXC01) { Byte tVal = rand() | 0x80; testZPXOp(INS_ROL_ZPX, tVal, (tVal << 1) & 0xFE, 0, 1, 6); }
	TEST_F(TestShiftInstruction, TestROLZPXC10) { Byte tVal = rand() & 0x7F; testZPXOp(INS_ROL_ZPX, tVal, (tVal << 1) | 0x01, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestROLZPXC11) { Byte tVal = rand() | 0x80; testZPXOp(INS_ROL_ZPX, tVal, (tVal << 1) | 0x01, 1, 1, 6); }

	/* Test LSR Execution */
	// ACC
	TEST_F(TestShiftInstruction, TestLSRAccNoCarry) { Byte tVal = (rand() & 0xFE);	testAccOp(INS_LSR_ACC, tVal, (tVal >> 1) & 0x7F, 1, 0, 2); }
	TEST_F(TestShiftInstruction, TestLSRAccWiCarry) { Byte tVal = (rand() | 0x01);	testAccOp(INS_LSR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 1, 2); }

	// ABS
	TEST_F(TestShiftInstruction, TestLSRAbsNoCarry) { Byte tVal = (rand() & 0xFE);	testAbsOp(INS_LSR_ABS, tVal, (tVal >> 1) & 0x7F, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestLSRAbsWiCarry) { Byte tVal = (rand() | 0x01);	testAbsOp(INS_LSR_ABS, tVal, (tVal >> 1) & 0x7F, 0, 1, 6); }

	// ABS-X
	TEST_F(TestShiftInstruction, TestLSRAbsXNoCarry) { Byte tVal = (rand() & 0xFE);	testAbsXOp(INS_LSR_ABX, tVal, (tVal >> 1) & 0x7F, 1, 0, 7); }
	TEST_F(TestShiftInstruction, TestLSRAbsXWiCarry) { Byte tVal = (rand() | 0x01);	testAbsXOp(INS_LSR_ABX, tVal, (tVal >> 1) & 0x7F, 0, 1, 7); }
	
	// Zero Page
	TEST_F(TestShiftInstruction, TestLSRZPNoCarry) { Byte tVal = (rand() & 0xFE);	testZPOp(INS_LSR_ZP0, tVal, (tVal >> 1) & 0x7F, 1, 0, 5); }
	TEST_F(TestShiftInstruction, TestLSRZPWiCarry) { Byte tVal = (rand() | 0x01);	testZPOp(INS_LSR_ZP0, tVal, (tVal >> 1) & 0x7F, 0, 1, 5); }

	// X-Indexed Zero Page
	TEST_F(TestShiftInstruction, TestLSRZPXNoCarry) { Byte tVal = (rand() & 0xFE);	testZPXOp(INS_LSR_ZPX, tVal, (tVal >> 1) & 0x7F, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestLSRZPXWiCarry) { Byte tVal = (rand() | 0x01);	testZPXOp(INS_LSR_ZPX, tVal, (tVal >> 1) & 0x7F, 0, 1, 6); }

	/* Test ROR Execution */
	// ACC
	TEST_F(TestShiftInstruction, TestRORAccC00) { Byte tVal = rand() & 0xFE; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 0, 2); }
	TEST_F(TestShiftInstruction, TestRORAccC01) { Byte tVal = rand() | 0x01; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) & 0x7F, 0, 1, 2); }
	TEST_F(TestShiftInstruction, TestRORAccC10) { Byte tVal = rand() & 0xFE; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) | 0x80, 1, 0, 2); }
	TEST_F(TestShiftInstruction, TestRORAccC11) { Byte tVal = rand() | 0x01; testAccOp(INS_ROR_ACC, tVal, (tVal >> 1) | 0x80, 1, 1, 2); }

	// ABS
	TEST_F(TestShiftInstruction, TestRORAbsC00) { Byte tVal = rand() & 0xFE; testAbsOp(INS_ROR_ABS, tVal, (tVal >> 1) & 0x7F, 0, 0, 6); }
	TEST_F(TestShiftInstruction, TestRORAbsC01) { Byte tVal = rand() | 0x01; testAbsOp(INS_ROR_ABS, tVal, (tVal >> 1) & 0x7F, 0, 1, 6); }
	TEST_F(TestShiftInstruction, TestRORAbsC10) { Byte tVal = rand() & 0xFE; testAbsOp(INS_ROR_ABS, tVal, (tVal >> 1) | 0x80, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestRORAbsC11) { Byte tVal = rand() | 0x01; testAbsOp(INS_ROR_ABS, tVal, (tVal >> 1) | 0x80, 1, 1, 6); }

	// ABS-X
	TEST_F(TestShiftInstruction, TestRORAbsXC00) { Byte tVal = rand() & 0xFE; testAbsXOp(INS_ROR_ABX, tVal, (tVal >> 1) & 0x7F, 0, 0, 7); }
	TEST_F(TestShiftInstruction, TestRORAbsXC01) { Byte tVal = rand() | 0x01; testAbsXOp(INS_ROR_ABX, tVal, (tVal >> 1) & 0x7F, 0, 1, 7); }
	TEST_F(TestShiftInstruction, TestRORAbsXC10) { Byte tVal = rand() & 0xFE; testAbsXOp(INS_ROR_ABX, tVal, (tVal >> 1) | 0x80, 1, 0, 7); }
	TEST_F(TestShiftInstruction, TestRORAbsXC11) { Byte tVal = rand() | 0x01; testAbsXOp(INS_ROR_ABX, tVal, (tVal >> 1) | 0x80, 1, 1, 7); }

	// Zero Page
	TEST_F(TestShiftInstruction, TestRORZPC00) { Byte tVal = rand() & 0xFE; testZPOp(INS_ROR_ZP0, tVal, (tVal >> 1) & 0x7F, 0, 0, 5); }
	TEST_F(TestShiftInstruction, TestRORZPC01) { Byte tVal = rand() | 0x01; testZPOp(INS_ROR_ZP0, tVal, (tVal >> 1) & 0x7F, 0, 1, 5); }
	TEST_F(TestShiftInstruction, TestRORZPC10) { Byte tVal = rand() & 0xFE; testZPOp(INS_ROR_ZP0, tVal, (tVal >> 1) | 0x80, 1, 0, 5); }
	TEST_F(TestShiftInstruction, TestRORZPC11) { Byte tVal = rand() | 0x01; testZPOp(INS_ROR_ZP0, tVal, (tVal >> 1) | 0x80, 1, 1, 5); }

	// X-Indexed Zero Page
	TEST_F(TestShiftInstruction, TestRORZPXC00) { Byte tVal = rand() & 0xFE; testZPXOp(INS_ROR_ZPX, tVal, (tVal >> 1) & 0x7F, 0, 0, 6); }
	TEST_F(TestShiftInstruction, TestRORZPXC01) { Byte tVal = rand() | 0x01; testZPXOp(INS_ROR_ZPX, tVal, (tVal >> 1) & 0x7F, 0, 1, 6); }
	TEST_F(TestShiftInstruction, TestRORZPXC10) { Byte tVal = rand() & 0xFE; testZPXOp(INS_ROR_ZPX, tVal, (tVal >> 1) | 0x80, 1, 0, 6); }
	TEST_F(TestShiftInstruction, TestRORZPXC11) { Byte tVal = rand() | 0x01; testZPXOp(INS_ROR_ZPX, tVal, (tVal >> 1) | 0x80, 1, 1, 6); }
}

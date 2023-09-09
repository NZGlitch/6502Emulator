#include <gmock/gmock.h>
#include "instruction_test.h"
#include "stack_instruction.h"

namespace E6502 {

	class TestStackInstruction : public TestInstruction {};

	/* Test addHandlers function and instruction opcodes */
	TEST_F(TestStackInstruction, TestStackAddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		StackInstruction::addHandlers(handlers);

		// Register transfers
		EXPECT_EQ(INS_PHA.opcode, 0x48);	EXPECT_EQ(*handlers[0x48], INS_PHA);
		EXPECT_EQ(INS_PHP.opcode, 0x08);	EXPECT_EQ(*handlers[0x08], INS_PHP);
		EXPECT_EQ(INS_PLA.opcode, 0x68);	EXPECT_EQ(*handlers[0x68], INS_PLA);
		EXPECT_EQ(INS_PLP.opcode, 0x28);	EXPECT_EQ(*handlers[0x28], INS_PLP);
	}

	TEST_F(TestStackInstruction, TestPHA) { 
		// Given:
		Byte initialSP = state->SP;
		Byte testValue = genTestValAndTargetClearMem(0x0100 | initialSP);
		state->A = testValue;
		(*memory)[programSpace] = INS_PHA.opcode;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ((*memory)[0x0100 | initialSP], testValue);
		EXPECT_EQ(state->SP, initialSP - 1);
	}

	TEST_F(TestStackInstruction, TestPHP) {
		// Given:
		Byte initialSP = state->SP;
		Byte testValue = state->FLAGS.byte;	//parent class already randmoises this for us
		(*memory)[0x0100 | state->SP] = ~testValue;
		(*memory)[programSpace] = INS_PHP.opcode;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ((*memory)[0x0100 | initialSP], testValue);
		EXPECT_EQ(state->SP, initialSP - 1);
	}

	TEST_F(TestStackInstruction, TestPLA) {
		// Given:
		Byte testValue = genTestValAndClearTargetReg(&state->A);
		Byte initialSP = state->SP;
		(*memory)[0x0100 | (initialSP + 1)] = testValue;
		(*memory)[programSpace] = INS_PLA.opcode;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		testAndResetStatusFlags(testValue);
		EXPECT_EQ(state->A, testValue);
		EXPECT_EQ(state->SP, (initialSP + 1) & 0xFF);
		EXPECT_EQ(cycles, 4);
	}

	TEST_F(TestStackInstruction, TestPLP) {
		// Given:
		Byte testValue = genTestValAndClearTargetReg(&state->FLAGS.byte);
		Byte initialSP = state->SP;
		Byte initalFlags = state->FLAGS.byte;
		(*memory)[0x0100 | (initialSP + 1)] = testValue;
		(*memory)[programSpace] = INS_PLP.opcode;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		Byte expectedFlags = (initalFlags & 0x30) | testValue & 0xCF;	//Copy bits 4 and 5 from initial, rest from test
		EXPECT_EQ(expectedFlags, state->FLAGS.byte);
		EXPECT_EQ(state->SP, initialSP + 1);

		state->FLAGS.byte = initPS.byte;	// Prevents parent class thorwing an error due to flag changes
	}
}

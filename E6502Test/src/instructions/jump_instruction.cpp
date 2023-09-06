#include <gmock/gmock.h>
#include "instruction_test.h"
#include "jump_instruction.h"

namespace E6502 {
	class TestJSRInstruction : public TestInstruction {};

	/* Test addHandlers func adds JSR handler */
	TEST_F(TestJSRInstruction, TestJSRaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		JumpInstruction::addHandlers(handlers);

		// Then: For the JSR instruction
		EXPECT_EQ(INS_JSR.opcode, 0x20);
		EXPECT_EQ(*handlers[0x20], INS_JSR);

		// Then: For the JMP ABS instruction
		EXPECT_EQ(INS_JMP_ABS.opcode, 0x4C);
		EXPECT_EQ(*handlers[0x4C], INS_JMP_ABS);

		// Then: For the JMP ABIN instruction
		EXPECT_EQ(INS_JMP_ABIN.opcode, 0x6C);
		EXPECT_EQ(*handlers[0x6C], INS_JMP_ABIN);

		// Then: For the RTS instruction
		EXPECT_EQ(INS_RTS.opcode, 0x60);
		EXPECT_EQ(*handlers[0x60], INS_RTS);
	}

	/* Test JSR execution */
	TEST_F(TestJSRInstruction, TestJSRAbsolute) {
		// Given: 
		Byte initialSP = state->SP;
		(*memory)[programSpace]		= INS_JSR.opcode;
		(*memory)[programSpace + 1] = addressSpace & 0xFF;
		(*memory)[programSpace + 2] = addressSpace >> 8;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->PC, addressSpace);					// The PC should be pointed at the target address
		EXPECT_EQ(state->SP, (initialSP - 2));				// SP should decrement by 2
		EXPECT_EQ(cycles, 6);

		Word stackAddr = (*memory)[0x100 | state->SP + 1] << 8 | (*memory)[0x100 | state->SP + 2];
		EXPECT_EQ(stackAddr, programSpace+2);	// Stack should contain original PC + 2
	}

	/* Test JMP Absolute execution */
	TEST_F(TestJSRInstruction, TestJMPPAbsolute) {
		// Given:
		(*memory)[programSpace]		= INS_JMP_ABS.opcode;
		(*memory)[programSpace + 1] = addressSpace & 0xFF;
		(*memory)[programSpace + 2] = addressSpace >> 8;

		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->PC, addressSpace);
		EXPECT_EQ(cycles, 3);
	}

	/* Test JMP Absolute Indirect execution */
	TEST_F(TestJSRInstruction, TestJMPAbsIndirect) {
		// Given:
		(*memory)[programSpace]		= INS_JMP_ABIN.opcode;
		(*memory)[programSpace + 1] = addressSpace & 0xFF;
		(*memory)[programSpace + 2] = addressSpace >> 8;

		(*memory)[addressSpace]		= dataSpace & 0xFF;
		(*memory)[addressSpace + 1]	= dataSpace >> 8;	

		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->PC, dataSpace);
		EXPECT_EQ(cycles, 5);
	}

	/* Test RTS Implied execution */
	TEST_F(TestJSRInstruction, TestRTSImplied) {
		// Given:
		(*memory)[programSpace] = INS_RTS.opcode;

		// Set target address on stack (todo use cpu->push)
		(*memory)[0x1FF] = 0xAB;  
		(*memory)[0x1FE] = 0xCD;
		state->SP = 0x1FD;
		Byte expectedCycles = 6;

		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->PC, 0xCDAC);		// Address on stack + 1
		EXPECT_EQ(state->SP, 0xFF);			// Stack incremented 2
		EXPECT_EQ(cycles, expectedCycles);
	}
}

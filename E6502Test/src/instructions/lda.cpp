#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "lda.h"

class TestLDAInstruction : public testing::Test {

public:

	CPUState* state;
	Memory* memory;

	virtual void SetUp() {
		state = new CPUState;
		memory = new Memory;
	}

	virtual void TearDown() {
		delete state;
		delete memory;
	}

	/* Helper method for testing InstructionHandler properties WARNING: This will delete the object when done*/
	void testPropsAndDelete(InstructionHandler* handler, Byte opCode, const char* name) {
		EXPECT_EQ((*handler).opcode, opCode);
		EXPECT_TRUE((*handler).isLegal);
		EXPECT_STREQ((*handler).name, name);
		delete handler;
	}
};

/************************************************
*                Execution tests                *
* Tests the execute function operates correctly *
*************************************************/

/**
* TODOS:
ZERO_PAGE
IMMEDIATE
ABSOLUTE
INDIRECT_Y
ZERO_PAGE_X
ABSOLUTE_Y
ABSOLUTE_X
*/

/* All LDA instructions place a value in the accumulator */
TEST_F(TestLDAInstruction, TestLDAImmeidate) {
	// Fixtures
	Byte testValueNorm = 0x55;		//TODO - maybe randomise?
	Byte testValueNeg = 0xFB;		//TODO - maybe randomise?
	Byte testValueZero = 0x00;
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x0000] = testValueNorm;
	memory->data[0x0001] = testValueNeg;
	memory->data[0x0002] = testValueZero;

	// Given:	
	state->A = ~testValueNorm;		//TODO - consider - must be different fro test value
	state->setFlags(0x00);
	
	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_IMM));
	
	// Then:
	EXPECT_EQ(state->A, testValueNorm) << "LDA_INDY Did not set Accumulator correctly";
	EXPECT_EQ(state->getFlags(), 0b00000000) << "Expected no flags to be set";
	EXPECT_EQ(cyclesUsed, 0);

	// Given:	
	state->A = ~testValueNeg;		//TODO - consider - must be different fro test value
	state->setFlags(0x00);

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_IMM));

	// Then:
	EXPECT_EQ(state->A, testValueNeg) << "LDA_INDY Did not set Accumulator correctly";
	EXPECT_EQ(state->getFlags(), 0b10000000) << "Expected negative flag to be set";
	EXPECT_EQ(cyclesUsed, 0);

	// Given:	
	state->A = ~testValueZero;		//TODO - consider - must be different fro test value
	state->setFlags(0x00);

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_IMM));

	// Then:
	EXPECT_EQ(state->A, testValueZero) << "LDA_INDY Did not set Accumulator correctly";
	EXPECT_EQ(state->getFlags(), 0b00000010) << "Expected zero flag to be set";
	EXPECT_EQ(cyclesUsed, 0);
}

/************************************************
*              End Execution tests              *
*************************************************


/* Test correct OpCodes */
TEST_F(TestLDAInstruction, TestInstructionDefs) {
	EXPECT_EQ(INS_LDA_IMM, 0xA9);
	EXPECT_EQ(INS_LDA_ZP, 0xA5);
	EXPECT_EQ(INS_LDA_ZPX, 0xB5);
	EXPECT_EQ(INS_LDA_ABS, 0xAD);
	EXPECT_EQ(INS_LDA_ABSX, 0xBD);
	EXPECT_EQ(INS_LDA_ABSY, 0xB9);
	EXPECT_EQ(INS_LDA_INDX, 0xA1);
	EXPECT_EQ(INS_LDA_INDY, 0xB1);
}

TEST_F(TestLDAInstruction, TestLDAaddHandlers) {
	// Given:
	InstructionHandler* handlers[0x100] = {nullptr};

	// When:
	LDA::addHandlers(handlers);

	// Then: For all LDA instructions, Expect *handlers[opcode] to point to a handler with the same opcode
	for (Byte& opcode : LDA::instructions) {
		ASSERT_FALSE(handlers[opcode] == nullptr);
		EXPECT_EQ(((*handlers[opcode]).opcode), opcode);
	}
}
/*********************************************
*				Handler Tests				 *
* Check each handler is configured correctly *
**********************************************/

TEST_F(TestLDAInstruction, TestLDAImmediateHandlerProps) {
	testPropsAndDelete(new LDA::LDA_IMM, INS_LDA_IMM, "LDA - Load Accumulator [Immediate]");
}

TEST_F(TestLDAInstruction, TestLDAZeroPageHandlerProps) {
	testPropsAndDelete(new LDA::LDA_ZP, INS_LDA_ZP, "LDA - Load Accumulator [ZeroPage]");
}

TEST_F(TestLDAInstruction, TestLDAZeroPageXHandlerProps) {
	testPropsAndDelete(new LDA::LDA_ZPX, INS_LDA_ZPX, "LDA - Load Accumulator [ZeroPage-X]");
}

TEST_F(TestLDAInstruction, TestLDAAbsolteHandlerProps) {
	// Given:
	testPropsAndDelete(new LDA::LDA_ABS, INS_LDA_ABS, "LDA - Load Accumulator [Absolute]");
}

TEST_F(TestLDAInstruction, TestLDAAbsoluteXHandlerProps) {
	testPropsAndDelete(new LDA::LDA_ABSX, INS_LDA_ABSX, "LDA - Load Accumulator [Absolute-X]");
}

TEST_F(TestLDAInstruction, TestLDAAbsoluteYHandlerProps) {
	testPropsAndDelete(new LDA::LDA_ABSY, INS_LDA_ABSY, "LDA - Load Accumulator [Absolute-Y]");
}

TEST_F(TestLDAInstruction, TestLDAIndirectXHandlerProps) {
	testPropsAndDelete(new LDA::LDA_INDX, INS_LDA_INDX, "LDA - Load Accumulator [Indirect-X]");
}

TEST_F(TestLDAInstruction, TestLDAIndirectYHandlerProps) {
	testPropsAndDelete(new LDA::LDA_INDY, INS_LDA_INDY, "LDA - Load Accumulator [Indirect-Y]");
}
/*********************************************
*			End of Handler Tests			 *
**********************************************/
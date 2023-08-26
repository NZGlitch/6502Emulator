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

/* Tests setFlags */

TEST_F(TestLDAInstruction, TestLDANoFlags) {
	// Given
	state->setFlags(0xFF);
	state->A = 0x78;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x5D);	// Unset Z and N flags

	// Given
	state->setFlags(0x00);
	state->A = 0x78;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x00); // Flags unchanges
}

TEST_F(TestLDAInstruction, TestLDAZeroFlags) {
	// Given
	state->setFlags(0x02);		//Z set
	state->A = 0x78;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x00);	// Expect Z Unset

	// Given
	state->setFlags(0x00);		//Z Unset
	state->A = 0x00;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x02); // Expect Z Set
}

TEST_F(TestLDAInstruction, TestLDANegFlags) {
	// Given
	state->setFlags(0xDD);		//N set
	state->A = 0x78;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x5D);	// Expect N Unset

	// Given
	state->setFlags(0x00);		//N Unset
	state->A = 0x80;

	// When
	LDA::setFlags(state);
	// Then
	EXPECT_EQ(state->getFlags(), 0x80); // Expect N Set
}

/* Tests LDA  Immediate Instruction */
TEST_F(TestLDAInstruction, TestLDAImmediate) {
	// Fixtures
	Byte testValue = 0x42;		//TODO - maybe randomise?
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x0000] = testValue;

	// Given:	
	state->A = ~testValue;		//TODO - consider - must be different fro test value
	
	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_IMM));
	
	// Then:
	EXPECT_EQ(state->A, testValue) << "LDA_IMM Did not set Accumulator correctly";
	EXPECT_EQ(cyclesUsed, 2);
}

/* Tests LDA Zero Page Instruction */
TEST_F(TestLDAInstruction, TestLDAZeroPage) {
	// Fixtures
	Byte testValue = 0x42;		//TODO - maybe randomise?
	Byte insAddress = 0x84;		//TODO - maybe randmomise?
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x0000] = insAddress;
	memory->data[insAddress] = testValue;

	// Given:	
	state->A = ~testValue;		//TODO - consider - must be different fro test value

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_ZP));

	// Then:
	EXPECT_EQ(state->A, testValue) << "LDA_ZP Did not set Accumulator correctly";
	EXPECT_EQ(cyclesUsed, 3);
}

/* Tests LDA Zero Page,X Instruction (No overflow)*/
TEST_F(TestLDAInstruction, TestLDAZeroPageNorm) {
	// Fixtures
	Byte baseAddress	= 0x84;			//TODO - maybe randmomise?
	Byte testX			= 0x10;					//When this is X we will get 0x94
	Byte testValue		= 0x42;			//TODO - maybe randomise?
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x000] = baseAddress;
	memory->data[0x94] = testValue;

	// Given:	
	state->A = ~testValue;		//TODO - consider - must be different fro test value
	state->X = testX;

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_ZPX));

	// Then:
	EXPECT_EQ(state->A, testValue) << "LDA_ZPX Did not set Accumulator correctly";
	EXPECT_EQ(cyclesUsed, 4);
}

/* Tests LDA Zero Page,X Instruction (With overflow) */
TEST_F(TestLDAInstruction, TestLDAZeroPageOver) {
	// Fixtures
	Byte baseAddress = 0x84;			//TODO - maybe randmomise?
	Byte testX = 0xBE;					//When this is X we will get 0x42 (0x84+0xBE = 0x142 --> 0x0042)
	Byte testValue = 0x24;				//TODO - maybe randomise?
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x000] = baseAddress;
	memory->data[0x42] = testValue;

	// Given:	
	state->A = ~testValue;		//TODO - consider - must be different fro test value
	state->X = testX;

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_ZPX));

	// Then:
	EXPECT_EQ(state->A, testValue) << "LDA_ZPX Did not set Accumulator correctly";
	EXPECT_EQ(cyclesUsed, 4);
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
#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "lda.h"

class TestLDAInstruction : public testing::Test {

public:

	const static bool ABS_IDX_X = true;;
	const static bool ABS_IDX_Y = false;

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


	/** 
	 * Helper method for all the abs mode tests
	 * @param instruction		Instruction code to execude (From LDA::instructions)
	 * @param lsb				least significant byte of base address
	 * @param msb				most significant byre of base address
	 * @param index				index to use
	 * @param idx_mode			ABS_IDX_X if using X register, ABS_IDX_Y if using y register
	 * @param expected_cycles	The number of cycles the execution should take
	 * @param test_name			Name of the test (helps with debugging)
	*/
	void absXHelper(Byte instruction, Byte lsb, Byte msb, Byte index, u8 idx_mode, u8 expected_cycles, char* test_name) {
		// Fixtures
		Byte testValue = 0x42;			//TODO - maybe randomise?
		Word targetAddress = (msb << 8) + lsb + index;
		u8 cyclesUsed = 0;

		// Load fixtures to memory
		memory->data[0x000] = lsb;
		memory->data[0x001] = msb;
		memory->data[targetAddress] = testValue;

		// Given:
		state->A = ~testValue;			//TODO - consider - must be different from test value
		state->PC = 0x0000;
		if (idx_mode == ABS_IDX_X)
			state->X = index;
		else
			state->Y = index;

		// When:
		cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(instruction));

		// Then:
		EXPECT_EQ(state->A, testValue) << "[" << test_name << "] LDA_ABS" << (idx_mode ? "X" : "Y:") << " Did not set Accumulator correctly";
		EXPECT_EQ(cyclesUsed, expected_cycles);
	}
};

/************************************************
*                Execution tests                *
* Tests the execute function operates correctly *
*************************************************/

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

/* Tests LDA Absolute Instruction */
TEST_F(TestLDAInstruction, TestLDAAbsolute) {
	// Fixtures
	Byte testValue = 0x42;		//TODO - maybe randomise?
	Byte lsb = 0x84;			//TODO - maybe randomise?
	Byte msb = 0xBE;			//TODO - maybe randomise?
	Word targetAddress = 0xBE84;
	u8 cyclesUsed = 0;

	// Load fixtures to memory
	state->PC = 0x0000;
	memory->data[0x000] = lsb;
	memory->data[0x001] = msb;
	memory->data[targetAddress] = testValue;


	// Given:	
	state->A = ~testValue;		//TODO - consider - must be different fro test value

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_ABS));

	// Then:
	EXPECT_EQ(state->A, testValue) << "LDA_ABS Did not set Accumulator correctly";
	EXPECT_EQ(cyclesUsed, 4);
}

/* Tests LDA Absolute,X Instruction */
TEST_F(TestLDAInstruction, TestLDAAbsoluteX) {
	Byte lsb = 0x84;			//TODO - maybe randomise?
	Byte msb = 0xFF;			//TODO - maybe randomise?
	Byte index = 0x00;
	u8 cyclesUsed = 0;

	index = 0x10;
	absXHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, 4, "LDA ABS (no overflow or page)");

	index = 0xA5;
	absXHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, 5, "LDA ABS (overflow)");

	msb = 0x37;
	index = 0xA1;
	absXHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, 5, "LDA ABS (page boundry)");
}

/* Tests LDA Absolute,Y Instruction */
TEST_F(TestLDAInstruction, TestLDAAbsoluteY) {
	Byte lsb = 0x84;			//TODO - maybe randomise?
	Byte msb = 0xFF;			//TODO - maybe randomise?
	Byte index = 0x00;
	u8 cyclesUsed = 0;

	index = 0x10;
	absXHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, 4, "LDA ABS (no overflow or page)");

	index = 0xA5;
	absXHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, 5, "LDA ABS (overflow)");

	msb = 0x37;
	index = 0xA1;
	absXHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, 5, "LDA ABS (page boundry)");
}

/* Tests LDA Indeirect,X Instruction */
TEST_F(TestLDAInstruction, TestLDAIndirectX) {
	Byte testValue = 0x42;
	
	Byte base = 0x20;
	Byte index = 0x10;
	Byte testIndirectAddress = 0x30;
	Word targetAddress = 0xABCD;
	
	Byte indexWrap = 0xFF;
	Byte testIndirectAddressWrap = 0x1F;
	Word targetAddressWrap = 0xDCBA;

	u8 cyclesUsed;

	// Given:
	state->PC = 0x0000;
	state->X = index;
	state->A = ~testValue;		//TODO - consider - must be different fro test value
	memory->data[0x0000] = base;
	memory->data[testIndirectAddress] = targetAddress & 0xFF;
	memory->data[testIndirectAddress+1] = (targetAddress >> 8) & 0xFF;
	memory->data[targetAddress] = testValue;
	

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_INDX));

	// Then:
	EXPECT_EQ(state->A, testValue);
	EXPECT_EQ(cyclesUsed,6);

	// Given (Wrap):
	state->PC = 0x0000;
	state->X = indexWrap;
	state->A = ~(testValue+1);		//TODO - consider - must be different fro test value
	memory->data[0x0000] = base;
	memory->data[testIndirectAddressWrap] = targetAddressWrap & 0xFF;
	memory->data[testIndirectAddressWrap + 1] = (targetAddressWrap >> 8) & 0xFF;
	memory->data[targetAddressWrap] = (testValue+1);

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_INDX));

	// Then:
	EXPECT_EQ(state->A, (testValue+1));
	EXPECT_EQ(cyclesUsed, 6);
}

/* Tests LDA Indeirect,Y Instruction */
TEST_F(TestLDAInstruction, TestLDAIndirectY) {
	Byte testValue = 0x42;

	Byte base = 0x20;
	Byte index = 0x10;
	Byte testIndirectAddress = 0x30;
	Word targetAddress = 0xABCD;

	Byte indexWrap = 0xFF;
	Byte testIndirectAddressWrap = 0x1F;
	Word targetAddressWrap = 0xDCBA;		
	Word targetAddressWrapCarry = 0xDDBA;	// The carry from 0x20+0xFF is added to the 'DC' in zp[20] to make the actual target DDBA

	u8 cyclesUsed;

	// Given:
	state->PC = 0x0000;
	state->Y = index;
	state->A = ~testValue;		//TODO - consider - must be different fro test value
	memory->data[0x0000] = base;
	memory->data[testIndirectAddress] = targetAddress & 0xFF;
	memory->data[testIndirectAddress + 1] = (targetAddress >> 8) & 0xFF;
	memory->data[targetAddress] = testValue;


	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_INDY));

	// Then:
	EXPECT_EQ(state->A, testValue);
	EXPECT_EQ(cyclesUsed, 6);

	// Given (Wrap):
	state->PC = 0x0000;
	state->Y = indexWrap;
	state->A = ~(testValue + 1);		//TODO - consider - must be different fro test value
	memory->data[0x0000] = base;
	memory->data[testIndirectAddressWrap] = targetAddressWrap & 0xFF;
	memory->data[testIndirectAddressWrap + 1] = (targetAddressWrap >> 8) & 0xFF;
	memory->data[targetAddressWrapCarry] = (testValue + 1);

	// When:
	cyclesUsed = LDA::LDAInstructionHandler(memory, state, &InstructionCode(INS_LDA_INDY));

	// Then:
	EXPECT_EQ(state->A, (testValue + 1));
	EXPECT_EQ(cyclesUsed, 6);
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

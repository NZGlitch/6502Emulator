#include <gmock/gmock.h>
#include "cpu.h"
#include "lda.h"

class TestLDAInstruction : public testing::Test {

public:

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};


/* Example test */
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


TEST_F(TestLDAInstruction, TestLDAImmediateHandlerProps) {
	// Given:
	const LDA::LDA_IMM handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_IMM);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Immediate]");
}

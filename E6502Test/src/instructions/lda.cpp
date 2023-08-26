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

TEST_F(TestLDAInstruction, TestLDAZeroPageHandlerProps) {
	// Given:
	const LDA::LDA_ZP handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_ZP);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [ZeroPage]");
}

TEST_F(TestLDAInstruction, TestLDAZeroPageXHandlerProps) {
	// Given:
	const LDA::LDA_ZPX handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_ZPX);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [ZeroPage-X]");
}

TEST_F(TestLDAInstruction, TestLDAAbsolteHandlerProps) {
	// Given:
	const LDA::LDA_ABS handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_ABS);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Absolute]");
}

TEST_F(TestLDAInstruction, TestLDAAbsoluteXHandlerProps) {
	// Given:
	const LDA::LDA_ABSX handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_ABSX);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Absolute-X]");
}

TEST_F(TestLDAInstruction, TestLDAAbsoluteYHandlerProps) {
	// Given:
	const LDA::LDA_ABSY handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_ABSY);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Absolute-Y]");
}

TEST_F(TestLDAInstruction, TestLDAIndirectXHandlerProps) {
	// Given:
	const LDA::LDA_INDX handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_INDX);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Indirect-X]");
}

TEST_F(TestLDAInstruction, TestLDAIndirectYHandlerProps) {
	// Given:
	const LDA::LDA_INDY handler;

	// Then
	EXPECT_EQ(handler.opcode, INS_LDA_INDY);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "LDA - Load Accumulator [Indirect-Y]");
}

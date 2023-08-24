#include <gmock/gmock.h>
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

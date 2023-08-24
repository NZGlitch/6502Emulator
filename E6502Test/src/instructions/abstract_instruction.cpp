#include <gmock/gmock.h>
#include "instructions/abstract_instruction.h"

class TestAbstractInstruction : public testing::Test {

public:
	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};


/* Example test */
TEST_F(TestAbstractInstruction, TestExample) {
	EXPECT_EQ(0, 0);
}

#include <gmock/gmock.h>
#include "types.h"
#include "instruction_manager.h"
#include "instructions/instruction_utils.h"
#include "instructions/lda.h"

class TestInstructionUtils : public testing::Test {

public:
	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};


/* Test all LDA instructions are correctly added */
TEST_F(TestInstructionUtils, TestLDAInstructionDefs) {
	// Given:
	InstructionHandler* handlers[0x100];

	// When:
	InstructionUtils::loader.load(handlers);

	// Then:
	for (Byte& opcode : LDA::instructions) {
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
	}
}

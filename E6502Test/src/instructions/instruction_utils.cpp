#include <gmock/gmock.h>
#include "types.h"
#include "instruction_manager.h"
#include "instructions/instruction_utils.h"
#include "instructions/lda.h"

class TestInstructionUtils : public testing::Test {

public:
	InstructionManager inMan = InstructionManager(&InstructionUtils::loader);

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};


/* Example test */
TEST_F(TestInstructionUtils, TestLDAInstructionDefs) {
	for (Byte& opcode : LDA::instructions) {
			EXPECT_EQ((inMan[opcode]->opcode), opcode);
	}
}

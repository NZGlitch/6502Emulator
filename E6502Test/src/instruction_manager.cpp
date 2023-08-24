#include <gmock/gmock.h>
#include "instruction_manager.h"

/**
* The Instruction Manager contains all the handlers for the various instructions
*/
class TestInstructionManager : public testing::Test {
public:

	InstructionManager inMan;

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};

/* Check default handler works as expected */
TEST_F(TestInstructionManager, TestDefaultHandler) {
	// Values should be correct
	ASSERT_EQ(inMan.defaultHandler.opcode, 0xEA);
	ASSERT_EQ(inMan.defaultHandler.isLegal, false);
	ASSERT_STREQ(inMan.defaultHandler.name, "Unsupported OP");
	
	// Memory should be unchanged
	Memory* mem = new Memory();
	mem->initialise();

	// Test execute function
	Byte cycles = inMan.defaultHandler.execute(mem);

	// Should take no cycles
	ASSERT_EQ(cycles, 0);

	// Should not affect CPU state
	for (int i = 0; i <= 0xFF; i++) ASSERT_EQ(mem->data[i], 0x00);

	delete mem;
}

/* Test array access to handlers works correctly */
TEST_F(TestInstructionManager, TestHandlers) {
	//InstructionHandler test = inMan[0];
	ASSERT_EQ(&inMan.defaultHandler, inMan[0]);
}

/* Test OpCodes are defined correctly */
TEST_F(TestInstructionManager, TestOpCodes) {
	ASSERT_EQ(INS_NOP, 0xEA);
}
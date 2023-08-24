#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "instruction_manager.h"

using::testing::_;

struct MockLoader : public InstructionUtils::InstructionLoader {
	MOCK_METHOD(void, load, (InstructionHandler* handlers[]));
};

/**
* The Instruction Manager contains all the handlers for the various instructions
*/
class TestInstructionManager : public testing::Test {
public:
	InstructionManager* inMan; 

	virtual void SetUp() {
		inMan = new InstructionManager(&InstructionUtils::loader);
	}

	virtual void TearDown() {
		delete inMan;
	}
};

/* Check default handler works as expected */
TEST_F(TestInstructionManager, TestDefaultHandler) {
	// Values should be correct
	ASSERT_EQ(inMan->defaultHandler.opcode, 0xEA);
	ASSERT_EQ(inMan->defaultHandler.isLegal, false);
	ASSERT_STREQ(inMan->defaultHandler.name, "Unsupported OP");
	
	// Memory should be unchanged
	Memory* mem = new Memory();
	mem->initialise();

	// Test execute function
	Byte cycles = inMan->defaultHandler.execute(mem);

	// Should take no cycles
	ASSERT_EQ(cycles, 0);

	// Should not affect CPU state
	for (int i = 0; i <= 0xFF; i++) ASSERT_EQ(mem->data[i], 0x00);

	delete mem;
}

/* Test it loads instructions from the loader  */
TEST_F(TestInstructionManager, TestAddsKnownInstructions) {
	MockLoader mockLoader;

	// Then: load is called on the loader
	EXPECT_CALL(mockLoader, load(_)).Times(1);

	//Given & When:
	InstructionManager test(&mockLoader);
}

/* Test array access to handlers works correctly */
TEST_F(TestInstructionManager, TestArrayAccess) {
	EXPECT_EQ(&(inMan->defaultHandler), (*inMan)[0]);
}

/* Test OpCodes are defined correctly */
TEST_F(TestInstructionManager, TestOpCodes) {
	EXPECT_EQ(INS_NOP, 0xEA);
}

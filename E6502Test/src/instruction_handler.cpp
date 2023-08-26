#include <gmock/gmock.h>
#include "types.h"

/**
* The Instruction Handle actually 'executes' instructions
*/
class TestInstructionHandler : public testing::Test {
public:

	InstructionHandler testHandler = {};

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};

/* Check handler fields */
TEST_F(TestInstructionHandler, TestDefaultHandler) {
	//Opcode
	testHandler.opcode = 0xFF;
	ASSERT_EQ(testHandler.opcode, 0xFF);
	ASSERT_EQ(sizeof(testHandler.opcode), sizeof(Byte));

	//isLegal
	testHandler.isLegal = false;
	ASSERT_EQ(testHandler.isLegal, false);
	ASSERT_EQ(sizeof(testHandler.isLegal), sizeof(bool));

	//name
	testHandler.name = "test";
	ASSERT_STREQ(testHandler.name, "test");
	ASSERT_EQ(sizeof(testHandler.name), sizeof(char*));

	//execute
	insHandlerFn testFun =  [](Memory* mem, CPUState* state) { return 0;} ;
	testHandler.execute = testFun;
	ASSERT_EQ(testHandler.execute, testFun);
	ASSERT_EQ(sizeof(testHandler.execute), sizeof(insHandlerFn*));
}

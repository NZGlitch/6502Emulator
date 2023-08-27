#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "jsr.h"

class TestJSRInstruction : public testing::Test {
public:

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
};

/************************************************
*                Execution tests                *
* Tests the execute function operates correctly *
*************************************************/

/* Test JSR execution */
TEST_F(TestJSRInstruction, TestJSRAbsolute) {
	EXPECT_TRUE(false);	//TODO
}

/************************************************
*              End Execution tests              *
*************************************************

/* Test correct OpCodes */
TEST_F(TestJSRInstruction, TestInstructionDefs) {
	EXPECT_EQ(INS_JSR, 0x20);
}

/* Test addHandlers func adds JSR handler */
TEST_F(TestJSRInstruction, TestJSRaddHandlers) {
	// Given:
	InstructionHandler* handlers[0x100] = { nullptr };

	// When:
	JSR::addHandlers(handlers);

	// Then: For the JSR instruction, Expect *handlers[opcode] to point to a handler with the same opcode
	for (const Byte& opcode : JSR::instructions) {
		ASSERT_FALSE(handlers[opcode] == nullptr);
		EXPECT_EQ(((*handlers[opcode]).opcode), opcode);
	}
}

/*********************************************
*				Handler Tests				 *
* Check each handler is configured correctly *
**********************************************/

TEST_F(TestJSRInstruction, TestJSRAbsoluteHandlerProps) {
	InstructionHandler handler = JSR_ABS();
	EXPECT_EQ(handler.opcode, INS_JSR);
	EXPECT_TRUE(handler.isLegal);
	EXPECT_STREQ(handler.name, "JSR - Jump to Subroutine [Absolute]");
}
/*********************************************
*			End of Handler Tests			 *
**********************************************/

#include <gmock/gmock.h>
#include "types.h"
#include "instruction_manager.h"
#include "instructions/instruction_utils.h"
#include "instructions/jsr.h"
#include "instructions/ldaxy.h"

namespace E6502 {

	class TestInstructionUtils : public testing::Test {

	public:
		virtual void SetUp() {
		}

		virtual void TearDown() {
		}
	};


	/* Test all instructions are correctly added */
	TEST_F(TestInstructionUtils, TestInstructionDefs) {
		// Given:
		InstructionHandler* handlers[0x100];
		for (int i = 0; i < 0x100; i++) handlers[i] = nullptr;

		// When:
		InstructionUtils::loader.load(handlers);

		// Then (LDAXY):
		for (const InstructionHandler& handler : LDAXY::instructions) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (JSR):
		for (const Byte& opcode : JSR::instructions) {
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}
	}
}
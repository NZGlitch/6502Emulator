#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"

namespace E6502 {

	using::testing::_;

	struct MockLoader : public InstructionLoader {
		MOCK_METHOD(void, load, (InstructionHandler* handlers[]));
	};

	class TestInstructionManager : public testing::Test {
	public:
		InstructionLoader loader;
		InstructionManager* inMan;

		virtual void SetUp() {
			inMan = new InstructionManager(&loader);
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

		// Given:
		CPUState originalState = CPUState();
		CPUState testState = CPUState();
		Memory* mem = new Memory();
		CPU* cpu = new CPU(&testState, mem, &loader);
		Byte code = 0x00;
		mem->initialise();

		// When:
		Byte cycles = inMan->defaultHandler.execute(cpu, code);

		// Then:
		EXPECT_EQ(cycles, 2);
		for (int i = 0; i <= 0xFF; i++) EXPECT_EQ(mem->data[i], 0x00);
		EXPECT_EQ(originalState, testState);

		// Cleanup:
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
}
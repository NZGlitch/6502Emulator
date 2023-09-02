#include <gmock/gmock.h>
#include "types.h"

namespace E6502 {

	class TestMemory : public testing::Test {
	public:
		Memory memory;

		virtual void SetUp() {
		}

		virtual void TearDown() {
		}

	};

	/* Test the memory initialisation function */
	TEST_F(TestMemory, TestMemoryInit) {
		// Given:
		for (int i = 0; i < memory.MAX_MEM; i++)
			memory.data[i] = 0xFF;					// Non-zero data in mem

		// When:
		memory.initialise();

		// Then:
		for (int i = 0; i < memory.MAX_MEM; i++)
			ASSERT_EQ(memory.data[i], 0x00);		// Data is 0
	}


	/* Test the loadProgram function */
	TEST_F(TestMemory, TestLoadProgram) {
		// Given:
		const Word programSize = 0x1000;
		Word loadAddress = 0xFF00;
		Byte program[programSize] = {};
		for (Word i = 0; i < programSize; i++) {
			Byte ins = (i&0xFF);
			program[i] = ins;
		}

		// When: 
		memory.loadProgram(loadAddress, program, programSize);

		// Then
		for (u16 i = 0; i < programSize; i++) {
			Word nextAddr = loadAddress + i;
			Byte expect = i;
			EXPECT_EQ(memory.data[nextAddr], expect);
		}
	}
}

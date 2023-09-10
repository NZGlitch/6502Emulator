#include <gmock/gmock.h>
#include "types.h"
#include "memory.h"

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
	TEST_F(TestMemory, TestMemoryReset) {
		// Given:
		for (int i = 0; i <MAX_MEM; i++)
			memory[i] = 0xFF;					// Non-zero data in mem

		// When:
		memory.reset();

		// Then:
		for (int i = 0; i < MAX_MEM; i++)
			EXPECT_EQ(memory[i], 0x00);		// Data is 0
	}

	/* Test read access */
	TEST_F(TestMemory, TestReadData) {
		// Given:
		memory[0x1234] = 0x00;
		EXPECT_EQ(memory[0x1234], 0x00);

		// When:
		memory[0x1234] = 0x42;

		// Then:
		EXPECT_EQ(memory[0x1234], 0x42);
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
			Byte expect = (Byte)i;
			EXPECT_EQ(memory[nextAddr], expect);
		}
	}
}

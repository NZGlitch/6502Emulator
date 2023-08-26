#include <gmock/gmock.h>
#include "types.h"

/**
* Test the memory structure
*/
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

/* Test the readByte function */
TEST_F(TestMemory, TestMemReadByte) {
	// Given:
	u8 cycles = 0;
	Word address = (rand() & 0xFFFF);
	Byte data = rand() & 0xFF;
	memory.data[address] = data;

	// When:
	Byte result = memory.readByte(cycles, address);

	// Then:
	EXPECT_EQ(cycles, 1);
	EXPECT_EQ(result, data);
}

/* Test the writeByte function */
TEST_F(TestMemory, TestMemWriteByte) {
	// Given:
	u8 cycles = 0;
	Word address = rand() & 0xFFFF;
	Byte data = rand() & 0xFF;
	memory.data[address] = ((data + 1) & 0xFF);

	// When:
	memory.writeByte(cycles, address, data);

	// Then:
	EXPECT_EQ(cycles, 1);
	EXPECT_EQ(memory.data[address], data);
}

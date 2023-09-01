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

	/* Test the readWord function */
	TEST_F(TestMemory, TestMemReadWord) {
		// Given:
		u8 cycles = 0;
		Word address = (rand() & 0xFFFF);
		Byte lsb = rand() & 0xFF;
		Byte msb = rand() & 0xFF;
		memory.data[address] = lsb;
		memory.data[(address + 1) & 0xFFFF] = msb;

		// When:
		Word result = memory.readWord(cycles, address);

		// Then:
		Word expectResult = (msb << 8) | lsb;
		EXPECT_EQ(cycles, 2);
		EXPECT_EQ(expectResult, result);
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

	/* Test the loadProgram function */
	TEST_F(TestMemory, TestLoadProgram) {
		// Given:
		const Word programSize = 0x1000;
		Word loadAddress = 0xFF00;
		Byte program[programSize] = {};
		for (Word i = 0; i < programSize; i++) {
			Byte ins = i;
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

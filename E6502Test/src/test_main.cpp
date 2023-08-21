#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "main.h"

class E6502Test : public testing::Test {
	public:
	Mem mem;
	CPU cpu;

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}

};

struct MockMem : public Mem {
	MOCK_METHOD(void, initialise, ());
};

TEST_F(E6502Test, TestMemoryInit) {
	// Given:
	for (u16 i = 0; i < mem.MAX_MEM; i++)
		mem.data[i] = 0xFF;					// Non-zero data in mem

	// When:
	mem.initialise();

	// Then:
	for (u16 i = 0; i < mem.MAX_MEM; i++)
		ASSERT_EQ(mem.data[i], 0x00);		// Data is 0
}

TEST_F(E6502Test, TestCPUReset) {
	//Given:
	cpu.PC = 0x0000;
	cpu.SP = 0x0000;
	cpu.A = cpu.X = cpu.Y = 0x42;
	cpu.D = 1;
	cpu.I = 1;

	MockMem mMem;
	EXPECT_CALL(mMem, initialise()).Times(1);

	//When:
	cpu.reset(mMem);

	//Then:
	ASSERT_EQ(cpu.PC, 0xFFFC);	// Program Counter set to correct address
	ASSERT_EQ(cpu.D, 0);		// Clear Decimal Flag
	ASSERT_EQ(cpu.I, 0);		// Clear Interrupt Disable Flag
	ASSERT_EQ(cpu.SP, 0x0100);	// Set the stack pointer to the bottom of page 1

	// Registers reset to 0
	ASSERT_EQ(cpu.A, 0);
	ASSERT_EQ(cpu.X, 0);
	ASSERT_EQ(cpu.Y, 0);

	// Memory is initialised

};

TEST_F(E6502Test, TestCPUSetAndGetFlags) {
	// Given:
	cpu.reset(mem);
	Byte flagMask = 0x0b11011111;						//Bit 5 is never set
	
	for (u16 i = 0x00; i <= 0x00; i++) {
		//when:
		cpu.setFlags(i & 0xFF);

		//then
		EXPECT_EQ(i & flagMask & 0xFF, cpu.getFlags());
		u8 flags = (cpu.C << 0) | (cpu.Z << 1) | (cpu.I << 2) | (cpu.D << 3) | (cpu.B << 4) | (cpu.O << 6) | (cpu.N << 7);
		EXPECT_EQ(i & flagMask & 0xFF, flags);
	}
};

TEST_F(E6502Test, TestCPUExecuteFunction) {
	// Given:
	Byte flagMask = 0x0b11011111;						//Bit 5 is never set
	cpu.reset(mem);
	Byte initFlags = (rand() & 0xFF & flagMask);
	cpu.setFlags(initFlags);
	ASSERT_EQ(cpu.PC, 0xFFFC);
	Byte cycles = 0;
	mem.writeByte(cycles, 0xFFFC, 0xEA); // instruction 0xEA is a NOP

	//when:
	u16 cyclesExecuted = cpu.execute(1, mem);

	//then
	EXPECT_EQ(cpu.PC, (0xFFFC) + 1);	//PC should be incremented
	EXPECT_EQ(cyclesExecuted, 2);
	EXPECT_EQ(initFlags, cpu.getFlags());

};

TEST_F(E6502Test, TestMemReadByte) {
	// Given:
	u8 cycles = 0;
	Word address = (rand() & 0xFFFF);
	Byte data = rand() & 0xFF;
	mem.data[address] = data;
	
	// When:
	Byte result = mem.readByte(cycles, address);

	// Then:
	EXPECT_EQ(cycles, 1);
	EXPECT_EQ(result, data);
}

TEST_F(E6502Test, TestMemWriteByte) {
	// Given:
	u8 cycles = 0;
	Word address = rand() & 0xFFFF;
	Byte data = rand() & 0xFF;
	mem.data[address] = ((data + 1) & 0xFF);

	// When:
	mem.writeByte(cycles, address, data);

	// Then:
	EXPECT_EQ(cycles, 1);
	EXPECT_EQ(mem.data[address], data);
}
#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"

struct MockMem : public Memory {
	MOCK_METHOD(void, initialise, ());
};

class TestCPU : public testing::Test {

public:

	Memory* mem = NULL;
	MockMem* mMem = NULL;
	CPU* cpu = NULL;

	virtual void SetUp() {
		cpu = new CPU();
		mem = new Memory();
		mMem = new MockMem();
	}

	virtual void TearDown() {
		delete cpu;
		delete mem;
		delete mMem;
	}
};


/* Test reset function*/
TEST_F(TestCPU, TestCPUReset) {
	
	//Given:
	cpu->PC = 0x0000;
	cpu->SP = 0x0000;
	cpu->A = cpu->X = cpu->Y = 0x42;
	cpu->D = 1;
	cpu->I = 1;

	// Memory is initialised
	EXPECT_CALL(*mMem, initialise()).Times(1);

	//When:
	cpu->reset(mMem);

	//Then:
	ASSERT_EQ(cpu->PC, 0xFFFC);	// Program Counter set to correct address
	ASSERT_EQ(cpu->D, 0);		// Clear Decimal Flag
	ASSERT_EQ(cpu->I, 0);		// Clear Interrupt Disable Flag
	ASSERT_EQ(cpu->SP, 0x0100);	// Set the stack pointer to the bottom of page 1

	// Registers reset to 0
	ASSERT_EQ(cpu->A, 0);
	ASSERT_EQ(cpu->X, 0);
	ASSERT_EQ(cpu->Y, 0);
};

TEST_F(TestCPU, TestCPUSetAndGetFlags) {
	// Given:
	cpu->reset(mem);
	Byte flagMask = 0b11011111;						//Bit 5 is never set

	for (u16 i = 0x00; i <= 0x00; i++) {
		//when:
		cpu->setFlags(i & 0xFF);

		//then
		EXPECT_EQ(i & flagMask & 0xFF, cpu->getFlags());
		u8 flags = (cpu->C << 0) | (cpu->Z << 1) | (cpu->I << 2) | (cpu->D << 3) | (cpu->B << 4) | (cpu->O << 6) | (cpu->N << 7);
		EXPECT_EQ(i & flagMask & 0xFF, flags);
	}
};

TEST_F(TestCPU, TestCPUExecuteFunction) {
	// Given:
	Byte flagMask = 0b11011111;						//Bit 5 is never set
	cpu->reset(mem);
	Byte initFlags = (rand() & 0xFF & flagMask);
	cpu->setFlags(initFlags);
	ASSERT_EQ(cpu->PC, 0xFFFC);
	Byte cycles = 0;
	mem->writeByte(cycles, 0xFFFC, 0xEA); // instruction 0xEA is a NOP

	//when:
	u16 cyclesExecuted = cpu->execute(1, mem);

	//then
	EXPECT_EQ(cpu->PC, (0xFFFC) + 1);	//PC should be incremented
	EXPECT_EQ(cyclesExecuted, 2);
	EXPECT_EQ(initFlags, cpu->getFlags());

};

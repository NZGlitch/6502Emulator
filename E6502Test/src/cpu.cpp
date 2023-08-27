#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"

struct MockMem : public Memory {
	MOCK_METHOD(void, initialise, ());
};

class TestCPU : public testing::Test {

public:
	CPUState* state = NULL;
	Memory* mem = NULL;
	MockMem* mMem = NULL;
	CPU* cpu = NULL;

	virtual void SetUp() {
		state = new CPUState;
		cpu = new CPU(state);
		mem = new Memory();
		mMem = new MockMem();
	}

	virtual void TearDown() {
		delete cpu;
		delete mem;
		delete mMem;
	}
};

/* Test reset function */
TEST_F(TestCPU, TestCPUReset) {
	
	// Given:
	state->PC = 0x0000;
	state->SP = 0x0000;
	state->A = state->X = state->Y = 0x42;
	state->D = 1;
	state->I = 1;

	// Memory is initialised
	EXPECT_CALL(*mMem, initialise()).Times(1);

	// When:
	cpu->reset(mMem);

	// Then:
	EXPECT_EQ(state->PC, 0xFFFC);	// Program Counter set to correct address
	EXPECT_EQ(state->D, 0);			// Clear Decimal Flag
	EXPECT_EQ(state->I, 0);			// Clear Interrupt Disable Flag
	EXPECT_EQ(state->SP, 0x00);		// Set the stack pointer to the bottom of page 1

	// Registers reset to 0
	EXPECT_EQ(state->A, 0);
	EXPECT_EQ(state->X, 0);
	EXPECT_EQ(state->Y, 0);
};

/* Test CPU Flags Getter & Setter */
TEST_F(TestCPU, TestCPUSetAndGetFlags) {
	// Given:
	cpu->reset(mem);
	Byte flagMask = 0b11011111;		// Bit 5 is never set

	for (u16 i = 0x00; i <= 0x00; i++) {
		// When:
		cpu->setFlags(i & 0xFF);

		// Then
		EXPECT_EQ(i & flagMask & 0xFF, cpu->getFlags());
		u8 flags = (state->C << 0) | (state->Z << 1) | (state->I << 2) | (state->D << 3) | (state->B << 4) | (state->O << 6) | (state->N << 7);
		EXPECT_EQ(i & flagMask & 0xFF, flags);
	}
};

TEST_F(TestCPU, TestCPUExecuteFunction) {
	// Given:
	Byte flagMask = 0b11011111;		// Bit 5 is never set
	cpu->reset(mem);
	Byte initFlags = (rand() & 0xFF & flagMask);
	cpu->setFlags(initFlags);
	ASSERT_EQ(state->PC, 0xFFFC);
	Byte cycles = 0;
	mem->writeByte(cycles, 0xFFFC, 0xEA); // Instruction 0xEA is a NOP - TODO replace with NOP instruction

	//when:
	u16 cyclesExecuted = cpu->execute(1, mem);

	//then
	EXPECT_EQ(state->PC, (0xFFFC) + 1);			// PC should be incremented
	EXPECT_EQ(cyclesExecuted, 2);				// NOP uses 2 cycles
	EXPECT_EQ(initFlags, cpu->getFlags());		// Flags should not change
};

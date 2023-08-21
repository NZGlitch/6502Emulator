#include <stdio.h>
#include <gtest/gtest.h>
#include "main.h"

class E6502Test : public testing::Test {
public:
	CPU cpu;

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}

};

TEST_F(E6502Test, TestCPUReset) {
	//Given:
	cpu.PC = 0x0000;
	cpu.SP = 0x0000;
	cpu.A = cpu.X = cpu.Y = 0x42;
	cpu.D = 1;
	cpu.I = 1;

	//When:
	cpu.reset();

	//Then:
	ASSERT_EQ(cpu.PC, 0xFFFC);	// Program Counter set to correct address
	ASSERT_EQ(cpu.D, 0);		// Clear Decimal Flag
	ASSERT_EQ(cpu.I, 0);		// Clear Interrupt Disable Flag
	ASSERT_EQ(cpu.SP, 0x0100);	// Set the stack pointer to the bottom of page 1
	
	// Registers reset to 0
	ASSERT_EQ(cpu.A, 0);
	ASSERT_EQ(cpu.X, 0);
	ASSERT_EQ(cpu.Y, 0);
}
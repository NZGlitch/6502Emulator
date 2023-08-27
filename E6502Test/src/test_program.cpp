#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"

class TestProgram : public testing::Test {
public:
	/* Size of program */
	const static Word programSize = 0x6;

	/* Sample program to test */
	Byte program[programSize] = {
		INS_LDA_IMM,	0x42,			//Load 0x42 into A
		INS_LDA_ZP,		0x02,			//Load mem[0x0002] into A (should also be 0xA5 - the INS_LDA_ZP instruction)
		INS_LDA_ZPX,	0xFF			//(Need to manually set X to 0x02 to get 0xFF = 0x02 = 0x01 = addr of 0x42)
	};

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}

};

/* Test the memory initialisation function */
TEST_F(TestProgram, TestProgram1) {
	// Initialise objects
	Memory* mem = new Memory();
	CPUState cpuState;
	CPU* cpu = new CPU(&cpuState);
	cpu->reset(mem);
	u8 cyclesExecuted = 0;

	// Load progrram and set initial state
	mem->loadProgram(0x0000, program, programSize);	//Load program @ 0x0000
	cpuState.PC = 0x0000;							//Set PC to start of program
	
	// Execute instructions
	cyclesExecuted = cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0x42);

	cyclesExecuted += cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0xA5);

	cpuState.X = 0x02;	// Have to set manually for now
	cyclesExecuted = cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0x42);
}

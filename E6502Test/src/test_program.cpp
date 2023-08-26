#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"

/**
* Test a sample program
*/
class TestProgram : public testing::Test {
public:

	Byte program[0x100] = {
		INS_LDA_IMM,	0x42,			//Load 0x42 into A
		INS_LDA_ZP,		0x02,			//Load mem[0x0002] into A (should also be 0xA5 - the INS_LDA_ZP instruction)
		INS_LDA_ZPX,	0xFF			//(Need to manually set X to 0x02 to get 0xFF = 0x02 = 0x01 = addr of 0x42)

	};

	u16 programSize = 6;

	virtual void SetUp() {
	}

	virtual void TearDown() {
	}

};

/* Test the memory initialisation function */
TEST_F(TestProgram, TestProgram1) {
	Memory* mem = new Memory();
	CPUState cpuState;
	CPU* cpu = new CPU(&cpuState);
	cpu->reset(mem);
	mem->loadProgram(0x0000, program, programSize);	//Load program @ 0x0000
	cpuState.PC = 0x0000;							//Set PC to start of program
	u8 cyclesExecuted = cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0x42);
	cyclesExecuted += cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0xA5);

	cpuState.X = 0x02;	// Have to set manually for now
	cyclesExecuted = cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0x42);
}

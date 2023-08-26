#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"

/**
* Test a sample program
*/
class TestProgram : public testing::Test {
public:

	Byte program[0x100] = {
		INS_LDA_IMM, 0x42
	};

	u16 programSize = 2;

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
	mem->loadProgram(0xFFFC, program, programSize);
	u8 cyclesExecuted = cpu->execute(1, mem);
	EXPECT_EQ(cpuState.A, 0x42);
}

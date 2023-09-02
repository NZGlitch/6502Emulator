#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"
namespace E6502 {

	struct MockMem : public Memory {
		MOCK_METHOD(void, initialise, ());
	};

	struct MockLoader : public InstructionLoader {};

	class TestCPU : public testing::Test {

	public:

		CPUState state; 
		Memory memory; 
		InstructionLoader loader;
		CPU* cpu;

		virtual void SetUp() {
			cpu = new CPU(&state, &memory, &loader);
		}

		virtual void TearDown() {
			delete cpu;
		}
	};

	/* Test reset function */
	TEST_F(TestCPU, TestCPUReset) {
		MockMem mem;
		CPU* testCPU = new CPU(&state, &mem, &loader);

		// Given:
		state.PC = 0x0000;
		state.setSP(0x00);
		state.A = state.X = state.Y = 0x42;
		state.D = 1;
		state.I = 1;

		// Memory is initialised
		EXPECT_CALL(mem, initialise()).Times(1);

		// When:
		testCPU->reset();

		// Then:
		EXPECT_EQ(state.PC, 0xFFFC);		// Program Counter set to correct address
		EXPECT_EQ(state.D, 0);				// Clear Decimal Flag
		EXPECT_EQ(state.I, 0);				// Clear Interrupt Disable Flag
		EXPECT_EQ(state.getSP(), 0x01FF);	// Set the stack pointer to the top of page 1

		// Registers reset to 0
		EXPECT_EQ(state.A, 0);
		EXPECT_EQ(state.X, 0);
		EXPECT_EQ(state.Y, 0);

	};

	/* Test CPU Flags Getter & Setter */
	TEST_F(TestCPU, TestCPUSetAndGetFlags) {
		// Given:
		cpu->reset();
		Byte flagMask = 0b11011111;		// Bit 5 is never set

		for (u16 i = 0x00; i <= 0x100; i++) {
			// When:
			cpu->setFlags(i & 0xFF);

			// Then:
			EXPECT_EQ(i & flagMask & 0xFF, cpu->getFlags());
			u8 flags = (state.C << 0) | (state.Z << 1) | (state.I << 2) | (state.D << 3) | (state.B << 4) | (state.O << 6) | (state.N << 7);
			EXPECT_EQ(i & flagMask & 0xFF, flags);
		}
	};

	/* Test cpu execute function */
	TEST_F(TestCPU, TestCPUExecuteFunction) {
		// Given:
		Byte flagMask = 0b11011111;		// Bit 5 is never set
		cpu->reset();
		Byte initFlags = (rand() & 0xFF & flagMask);
		cpu->setFlags(initFlags);
		ASSERT_EQ(state.PC, 0xFFFC);
		Byte cycles = 0;

		// When:
		u16 cyclesExecuted = cpu->execute(1);

		// Then
		EXPECT_EQ(state.PC, (0xFFFC) + 1);			// PC should be incremented
		EXPECT_EQ(cyclesExecuted, 2);				// NOP uses 2 cycles
	};

	/* Test the readByte function */
	TEST_F(TestCPU, TestMemReadByte) {
		// Given:
		u8 cycles = 0;
		Word address = (rand() & 0xFFFF);
		Byte data = rand() & 0xFF;
		memory.data[address] = data;

		// When:
		Byte result = cpu->readByte(cycles, address);

		// Then:
		EXPECT_EQ(cycles, 1);
		EXPECT_EQ(result, data);
	}

	/* Test the readWord function */
	TEST_F(TestCPU, TestMemReadWord) {
		// Given:
		u8 cycles = 0;
		Word address = (rand() & 0xFFFF);
		Byte lsb = rand() & 0xFF;
		Byte msb = rand() & 0xFF;
		memory.data[address] = lsb;
		memory.data[(address + 1) & 0xFFFF] = msb;

		// When:
		Word result = cpu->readWord(cycles, address);

		// Then:
		Word expectResult = (msb << 8) | lsb;
		EXPECT_EQ(cycles, 2);
		EXPECT_EQ(expectResult, result);
	}

	/* Test the writeByte function */
	TEST_F(TestCPU, TestMemWriteByte) {
		// Given:
		u8 cycles = 0;
		Word address = rand() & 0xFFFF;
		Byte data = rand() & 0xFF;
		memory.data[address] = ~data;

		// When:
		cpu->writeByte(cycles, address, data);

		// Then:
		EXPECT_EQ(cycles, 1);
		EXPECT_EQ(memory.data[address], data);
	}
}

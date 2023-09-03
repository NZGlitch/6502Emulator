#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"

namespace E6502 {

	struct MockMem : public Memory {
		MOCK_METHOD(void, reset, ());
	};

	struct MockState : public CPUState {
		MOCK_METHOD(void, reset, ());
	};

	struct MockLoader : public InstructionLoader {};

	class TestCPU : public testing::Test {

	public:

		CPUState* state; 
		Memory* memory; 
		InstructionLoader loader;
		CPU* cpu;

		TestCPU() {
			state = new CPUState;
			memory = new Memory;
			cpu = nullptr;
		}

		~TestCPU() {
			delete state;
			delete memory;
		}

		virtual void SetUp() {
			cpu = new CPU(state, memory, &loader);
		}

		virtual void TearDown() {
			delete cpu;
		}

		/** Helper for setFlags test - note given state is reset before and after the test */
		void testFlags(u8 targetReg, Byte initFlags, Byte testValue, Byte expectFlags, char* test_name) {
			state->reset();	//always reset to ensure nothing leaks between tests
			Byte cycles = 0;

			// Given:
			state->setFlags(initFlags);

			// When:
			cpu->saveToRegAndFlag(cycles, targetReg, testValue);

			// Then:
			EXPECT_EQ(cycles, 0);	//Currently this appears to be a free operation
			EXPECT_EQ(state->getFlags(), expectFlags);
			state->reset();
		}
	};

	/* Test reset function */
	TEST_F(TestCPU, TestCPUReset) {
		// Given:
		MockMem* mMem = new MockMem;
		MockState* mState = new MockState;
		CPU* testCPU = new CPU(mState, mMem, &loader);

		// Memory is initialised
		EXPECT_CALL(*mMem, reset()).Times(1);
		EXPECT_CALL(*mState, reset()).Times(1);

		// When:
		testCPU->reset();

		delete mMem;
		delete mState;
		delete testCPU;

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
			u8 flags = (state->C << 0) | (state->Z << 1) | (state->I << 2) | (state->D << 3) | (state->B << 4) | (state->O << 6) | (state->N << 7);
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
		ASSERT_EQ(state->PC, 0xFFFC);
		Byte cycles = 0;

		// When:
		u16 cyclesExecuted = cpu->execute(1);

		// Then
		EXPECT_EQ(state->PC, (0xFFFC) + 1);			// PC should be incremented
		EXPECT_EQ(cyclesExecuted, 2);				// NOP uses 2 cycles
	};

	/* Test the readByte function */
	TEST_F(TestCPU, TestMemReadByte) {
		// Given:
		u8 cycles = 0;
		Word address = rand();
		Byte data = rand();
		(*memory)[address] = data;

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
		(*memory)[address] = lsb;
		(*memory)[(address + 1) & 0xFFFF] = msb;

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
		(*memory)[address] = ~data;

		// When:
		cpu->writeByte(cycles, address, data);

		// Then:
		EXPECT_EQ(cycles, 1);
		EXPECT_EQ((*memory)[address], data);
	}

	/* Test fetching a byte from PC (With auto increment) */
	TEST_F(TestCPU, TestIncPCAndReadByte) {
		Word testStart = rand();
		Word expectEnd = testStart + 1;

		// Given:
		state->PC = testStart;
		u8 cycles = 0;
		(*memory)[expectEnd] = 0x42;


		// When:
		Byte value = cpu->incPCandReadByte(cycles);

		// Then:
		EXPECT_EQ(value, 0x42);
		EXPECT_EQ(state->PC, expectEnd);
		EXPECT_EQ(cycles, 1);
	}

	/* Test fetching a word from PC (With auto increment) */
	TEST_F(TestCPU, TestdequeuePCWord) {
		Word testStart = rand();
		Word expectEnd = testStart + 2;

		// Given:
		state->PC = testStart;
		u8 cycles = 0;
		(*memory)[testStart + 1] = 0x42;
		(*memory)[testStart + 2] = 0x84;


		// When:
		Word value = cpu->incPCandReadWord(cycles);

		// Then:
		EXPECT_EQ(value, 0x8442);
		EXPECT_EQ(state->PC, expectEnd);
		EXPECT_EQ(cycles, 2);
	}

	/* Test values are saved to the correct register */
	TEST_F(TestCPU, TestCPUSaveToReg) {
		// Given:
		Byte regA = 0x21;
		Byte regX = 0x42;
		Byte regY = 0x84;
		u8 cycles = 0;

		// When:
		cpu->saveToRegAndFlag(cycles, CPU::REGISTER_A, regA);
		cpu->saveToRegAndFlag(cycles, CPU::REGISTER_X, regX);
		cpu->saveToRegAndFlag(cycles, CPU::REGISTER_Y, regY);

		// Then:
		EXPECT_EQ(state->A, regA);
		EXPECT_EQ(state->X, regX);
		EXPECT_EQ(state->Y, regY);
	}

	/* Tests setFlags when N and Z flags 0 */
	TEST_F(TestCPU, TestRegisterSaveAndSetFlags00) {
		// No Flags (unset exiting)
		testFlags(CPU::REGISTER_A, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_A) NO ZN - change");
		testFlags(CPU::REGISTER_X, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_X) NO ZN - change");
		testFlags(CPU::REGISTER_Y, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_Y) NO ZN - change");

		// No Flags (Unchange existing)
		testFlags(CPU::REGISTER_A, 0x00, 0x78, 0x00, "setFlags(REGISTER_A) NO ZN - no change");
		testFlags(CPU::REGISTER_X, 0x00, 0x78, 0x00, "setFlags(REGISTER_X) NO ZN - no change");
		testFlags(CPU::REGISTER_Y, 0x00, 0x78, 0x00, "setFlags(REGISTER_Y) NO ZN - no change");
	}

	/* Tests setFlags when Z flag changes */
	TEST_F(TestCPU, TestRegisterSaveAndSetFlagsZ) {
		// Z-Flag should be unset
		testFlags(CPU::REGISTER_A, 0x02, 0x78, 0x00, "setFlags(REGISTER_A) unset Z");
		testFlags(CPU::REGISTER_X, 0x02, 0x78, 0x00, "setFlags(REGISTER_X) unset Z");
		testFlags(CPU::REGISTER_Y, 0x02, 0x78, 0x00, "setFlags(REGISTER_Y) unset Z");

		// Z-Flag sould be set
		testFlags(CPU::REGISTER_A, 0x00, 0x00, 0x02, "setFlags(REGISTER_A) set Z");
		testFlags(CPU::REGISTER_X, 0x00, 0x00, 0x02, "setFlags(REGISTER_X) set Z");
		testFlags(CPU::REGISTER_Y, 0x00, 0x00, 0x02, "setFlags(REGISTER_Y) set Z");
	}

	/* Tests setFlags when N flag changes */
	TEST_F(TestCPU, TestRegisterSaveAndSetFlagsN) {
		// N-Flag should be unset
		testFlags(CPU::REGISTER_A, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_A) unset N");
		testFlags(CPU::REGISTER_X, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_X) unset N");
		testFlags(CPU::REGISTER_Y, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_Y) unset N");

		// N-Flag sould be set
		testFlags(CPU::REGISTER_A, 0x00, 0x80, 0x80, "setFlags(REGISTER_A) set N");
		testFlags(CPU::REGISTER_X, 0x00, 0x80, 0x80, "setFlags(REGISTER_X) set N");
		testFlags(CPU::REGISTER_Y, 0x00, 0x80, 0x80, "setFlags(REGISTER_Y) set N");
	}

	/* Tests regValue */
	TEST_F(TestCPU, TestregValue) {
		// Given:
		state->A = 0x21;
		state->X = 0x42;
		state->Y = 0x84;
		Byte cycles = 0;

		// When:
		Byte a = cpu->regValue(cycles, CPU::REGISTER_A);
		EXPECT_EQ(cycles, 0);
		Byte x = cpu->regValue(cycles, CPU::REGISTER_X);
		EXPECT_EQ(cycles, 0);
		Byte y = cpu->regValue(cycles, CPU::REGISTER_Y);

		// Then:
		EXPECT_EQ(cycles, 0);
		EXPECT_EQ(a, 0x21);
		EXPECT_EQ(x, 0x42);
		EXPECT_EQ(y, 0x84);
		
	}

}

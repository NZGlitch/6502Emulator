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

		Byte registers[3] = { CPU::REGISTER_A, CPU::REGISTER_X, CPU::REGISTER_Y };
		CPUState* state;
		Memory* memory;
		InstructionLoader loader;
		CPUInternal* cpu;

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
			cpu = new CPUInternal(state, memory, &loader);
		}

		virtual void TearDown() {
			delete cpu;
		}

		/** Helper for setFlags test - note given state is reset before and after the test */
		void testFlags(u8 targetReg, Byte initFlags, Byte testValue, Byte expectFlags, char* test_name) {
			state->reset();	//always reset to ensure nothing leaks between tests
			Byte cycles = 0;

			// Given:
			state->FLAGS.byte = initFlags;

			// When:
			cpu->saveToRegAndFlag(cycles, targetReg, testValue);

			// Then:
			EXPECT_EQ(cycles, 0);	//Currently this appears to be a free operation
			EXPECT_EQ(state->FLAGS.byte, expectFlags);
			state->reset();
		}
	};

	/* Test reset function */
	TEST_F(TestCPU, TestCPUReset) {
		// Given:
		MockMem* mMem = new MockMem;
		MockState* mState = new MockState;
		CPUInternal* testCPU = new CPUInternal(mState, mMem, &loader);

		// Memory is initialised
		EXPECT_CALL(*mMem, reset()).Times(1);
		EXPECT_CALL(*mState, reset()).Times(1);

		// When:
		testCPU->reset();

		delete mMem;
		delete mState;
		delete testCPU;

	};

	/* Test cpu execute function */
	TEST_F(TestCPU, TestCPUExecuteFunction) {
		// Given:
		Byte flagMask = 0b11011111;		// Bit 5 is never set
		cpu->reset();
		Byte initFlags = (rand() & 0xFF & flagMask);
		state->FLAGS.byte = initFlags;
		ASSERT_EQ(state->PC, 0xFFFC);
		(*memory)[0xFFFC] = INS_NOP.opcode;		//insert a NOP instruction for the test
		Byte cycles = 0;

		// When:
		u8 cyclesExecuted = cpu->execute(1);

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
	TEST_F(TestCPU, TestreadPCByte) {
		Word testStart = rand();
		Word expectEnd = testStart + 1;

		// Given:
		state->PC = testStart;
		u8 cycles = 0;
		(*memory)[testStart] = 0x42;


		// When:
		Byte value = cpu->readPCByte(cycles);

		// Then:
		EXPECT_EQ(value, 0x42);
		EXPECT_EQ(state->PC, expectEnd);
		EXPECT_EQ(cycles, 1);
	}

	/* Test fetching a word from PC (With auto increment) */
	TEST_F(TestCPU, TestreadPCWord) {
		Word testStart = rand();
		Word expectEnd = testStart + 2;

		// Given:
		state->PC = testStart;
		u8 cycles = 0;
		(*memory)[testStart] = 0x42;
		(*memory)[testStart + 1] = 0x84;


		// When:
		Word value = cpu->readPCWord(cycles);

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
		cpu->saveToReg(cycles, CPU::REGISTER_A, regA);
		cpu->saveToReg(cycles, CPU::REGISTER_X, regX);
		cpu->saveToReg(cycles, CPU::REGISTER_Y, regY);

		// Then:
		EXPECT_EQ(state->A, regA);
		EXPECT_EQ(state->X, regX);
		EXPECT_EQ(state->Y, regY);
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

	/* Test getPC() */
	TEST_F(TestCPU, TestgetPC) {
		// Given:
		Word initPC = rand();
		state->PC = initPC;
		u8 cycles = 0;

		// When:
		Word value = cpu->getPC(cycles);

		// Then:
		EXPECT_EQ(value, initPC);
		EXPECT_EQ(state->PC, initPC);
		EXPECT_EQ(cycles, 1);
	}

	/* Test pullStackWord - should work well with pushWordToStack(inop) */
	TEST_F(TestCPU, TestPushPullStackWord) {
		u8 cycles = 0;
		// Given:
		Word testWord = rand();
		Byte initialSP = (rand() & 0x7F) | 0x07;	// Set the stack to a random place away from the top or bottom
		state->SP = initialSP;
		
		// Clear memory
		(*memory)[state->SP] = ~testWord & 0xFF;
		(*memory)[state->SP - 1] = ~testWord >> 8;
		cycles = 0;

		// When
		cpu->pushStackWord(cycles, testWord);

		// Then:
		EXPECT_EQ(cycles, 2);
		EXPECT_EQ(state->SP, initialSP - 2);
		EXPECT_EQ((*memory)[0x0100 | initialSP], testWord & 0xFF);
		EXPECT_EQ((*memory)[0x0100 | initialSP - 1], testWord >> 8);

		// When
		Word result = cpu->pullStackWord(cycles);

		// Then:
		EXPECT_EQ(cycles, 4);
		EXPECT_EQ(state->SP, initialSP);
		EXPECT_EQ(testWord, result);
		
	}

	/* Test setPC */
	TEST_F(TestCPU, TestsetPC) {
		// Given:
		state->PC = 0x1234;
		u8 cycles = 0;

		// When:
		cpu->setPC(cycles, 0x9876);

		// Then:
		EXPECT_EQ(state->PC, 0x9876);
		EXPECT_EQ(cycles, 1);
	}

	/* Test CPU Flags Getter & Setter */
	TEST_F(TestCPU, TestCPUSetAndGetFlags) {
		// Given:
		cpu->reset();
		FlagUnion testValue = FlagUnion();

		for (u16 i = 0x00; i <= 0x100; i++) {
			// When:
			Byte cycles = 0;
			testValue.byte = i;
			cpu->setFlags(cycles, testValue);

			// Then:
			EXPECT_EQ(cycles, 1);
			EXPECT_EQ(testValue.byte, state->FLAGS.byte);
			EXPECT_EQ(testValue.byte, cpu->getFlags(cycles).byte);
			EXPECT_EQ(cycles, 2);
			EXPECT_EQ(cycles, 2);
			
		}
	};

	/* Test pullStackByte */
	TEST_F(TestCPU, pullStackByte) {
		// Given:
		u8 cycles = 0;
		Byte initialSP = (rand() | 0x80);
		Byte testValue = rand();

		state->SP = initialSP;
		(*memory)[0x0100 | initialSP + 1] = testValue;

		// When:
		Byte result = cpu->pullStackByte(cycles);

		// Then:
		EXPECT_EQ(state->SP, initialSP + 1);
		EXPECT_EQ(result, testValue);
		EXPECT_EQ(cycles, 1);
	}

	/* Test pushStackByte(u8& cycles, Byte value) */
	TEST_F(TestCPU, pushStackByte) {
		// Given:
		u8 cycles = 0;
		Byte initialSP = (rand() | 0x80);
		Byte testValue = rand();

		state->SP = initialSP;
		(*memory)[0x0100 | initialSP] = ~testValue;

		// When:
		cpu->pushStackByte(cycles, testValue);

		// Then:
		EXPECT_EQ(state->SP, initialSP - 1);
		EXPECT_EQ((*memory)[0x0100 | initialSP], testValue);
		EXPECT_EQ(cycles, 1);
	}

	/* Test Byte getSP(u8& cycles) */
	TEST_F(TestCPU, getSP) {
		// Given:
		Byte cycles = 0;
		Byte testValue = rand();
		state->SP = testValue;


		// When:
		Byte result = cpu->getSP(cycles);

		// Then:
		EXPECT_EQ(result, testValue);
		EXPECT_EQ(state->SP, testValue);
		EXPECT_EQ(cycles, 1);
	}

	/* Test setSP(u8& cycles, Byte value) */
	TEST_F(TestCPU, setSP) { 
		// Given:
		Byte cycles = 0;
		Byte testValue = rand();
		state->SP = ~testValue;

		// When:
		cpu->setSP(cycles, testValue);

		// Then:
		EXPECT_EQ(state->SP, testValue);
		EXPECT_EQ(cycles, 1);
	}

	/* Test getCarry */
	TEST_F(TestCPU, getCarry) {
		// if it is set to false...
		Byte cycles = 0;
		state->Flag.C = false;
		EXPECT_FALSE(cpu->getCarryFlag(cycles));
		EXPECT_EQ(cycles, 0);

		// if it is set to true
		cycles = 0;
		state->Flag.C = true;
		EXPECT_TRUE(cpu->getCarryFlag(cycles));
		EXPECT_EQ(cycles, 0);
	}

	/* Test setCarry */
	TEST_F(TestCPU, setCarryTrue) {
		// Given:
		state->Flag.C = 0;
		u8 cycles = 0;

		// When:
		cpu->setCarryFlag(cycles, true);

		// Then:
		EXPECT_EQ(state->Flag.C, 1);
		EXPECT_EQ(cycles, 0);
	}

	/* Test setCarry */
	TEST_F(TestCPU, setCarryFalse) {
		// Given:
		state->Flag.C = 1;
		u8 cycles = 0;

		// When:
		cpu->setCarryFlag(cycles, false);

		// Then:
		EXPECT_EQ(state->Flag.C, 0);
		EXPECT_EQ(cycles, 0);
	}
	/* Test setNegative */
	TEST_F(TestCPU, setNegativeTrue) {
		// Given:
		state->Flag.N = 0;
		u8 cycles = 0;

		// When:
		cpu->setNegativeFlag(cycles, true);

		// Then:
		EXPECT_EQ(state->Flag.N, 1);
		EXPECT_EQ(cycles, 0);
	}

	/* Test setNegative */
	TEST_F(TestCPU, setNegativeFalse) {
		// Given:
		state->Flag.N = 1;
		u8 cycles = 0;

		// When:
		cpu->setNegativeFlag(cycles, false);

		// Then:
		EXPECT_EQ(state->Flag.N, 0);
		EXPECT_EQ(cycles, 0);
	}

	/* Test setZeroy */
	TEST_F(TestCPU, setZeroTrue) {
		// Given:
		state->Flag.Z = 0;
		u8 cycles = 0;

		// When:
		cpu->setZeroFlag(cycles, true);

		// Then:
		EXPECT_EQ(state->Flag.Z, 1);
		EXPECT_EQ(cycles, 0);
	}

	/* Test setZero */
	TEST_F(TestCPU, setZeroFalse) {
		// Given:
		state->Flag.Z = 1;
		u8 cycles = 0;

		// When:
		cpu->setZeroFlag(cycles, false);

		// Then:
		EXPECT_EQ(state->Flag.Z, 0);
		EXPECT_EQ(cycles, 0);
	}
}

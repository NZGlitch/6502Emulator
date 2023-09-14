#include <stdio.h>
#include <cstdlib>
#include <gmock/gmock.h>
#include "cpu.h"
#include "instructions/base.h"

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

		/** returns a FlagUnion of the flags that would be expected after adding the provided operands */
		FlagUnion getExpectedFlagsForBinaryADCOp(Byte operandA, Byte operandB, bool carryIn) {
			Byte result = operandA + operandB + (carryIn ? 1 : 0);
			FlagUnion flagResult{ 0x00 };
			if (result < operandB) flagResult.bit.C = 1;
			if (result == 0x00) flagResult.bit.Z = 1;
			if (result >> 7 != operandA >> 7) flagResult.bit.V = 1;
			flagResult.bit.N = result >> 7;
			return flagResult;
		}

		// Helper for testing all flag getter and setters.
		void testFlagGetterSetter(u8 flag) {
			// if it is set to false...
			Byte cycles = 0;
			cpu->setFlag(cycles, flag, false);
			EXPECT_FALSE(cpu->getFlag(cycles, flag));
			EXPECT_EQ(cycles, 0);

			// if it is set to true
			cycles = 0;
			cpu->setFlag(cycles, flag, true);
			EXPECT_TRUE(cpu->getFlag(cycles, flag));
			EXPECT_EQ(cycles, 0);
		}

		// Helper for addAccumulator instructions
		void testAddAccumulator(bool decimal, bool carryIn) {
			// Maybe not efficent, but easy to implment - test every possible operation
			for (int a = 0x00; a < 0x100; a++) {
				for (int b = 0x00; b < 0x100; b++) {
					// Default to binary
					Byte expectResult = a + b + carryIn;
					FlagUnion expectFlags = getExpectedFlagsForBinaryADCOp(a, b, carryIn);

					// Change values for decimal
					if (decimal) {
						Word AL = 0x0000;
						AL = (a & 0x0f) + (b & 0x0F) + carryIn;
						if (AL > 0x09) AL = ((AL + 0x06) & 0x0F) + 0x10;
						AL = ((a & 0xF0) + (b & 0x0F0)) + AL;
						if (AL > 0x99) AL = AL + 0x60;
						expectResult = AL;
						
						// Flags
						expectFlags.bit.C = AL > 0x99;
						expectFlags.bit.V = (expectResult >> 7) != (a >> 7);
						expectFlags.bit.Z = (expectResult == 0);
						expectFlags.bit.N = (expectResult >> 7);
					}
					
					// Given:
					
					state->A = a;
					state->FLAGS.byte = ~expectFlags.byte;		// Ensures operation must change all the required flags
					state->FLAGS.bit.C = carryIn;				// (Carry is special as it is used on both sides of the operation)
					state->FLAGS.bit.D = decimal;
					Byte cycles = 0;


					

					// When:
					cpu->addAccumulator(cycles, b);

					// Then:
					// Check result
					char* mode = decimal ? "Decimal" : "Binary";
					char* carry = carryIn ? "1" : "0";

					if (state->A != expectResult) {
						fprintf(stderr, "Invalid result in %s addAccumulator %X + %X + %s, expected %X got %X\n", mode, a, b, carry, expectResult, state->A);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}

					// Check flags
					if (state->FLAGS.bit.C != expectFlags.bit.C) {
						fprintf(stderr, "Carry bit not set corrctly in %s addAccumulator %X + %X + %s = %X - Expected (%d)\n", mode, a, b, carry, state->A, expectFlags.bit.C);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}
					if (state->FLAGS.bit.Z != expectFlags.bit.Z) {
						fprintf(stderr, "Zero bit not set correctly in %s addAccumulator %X + %X + %s = %X - Expected (%d)\n", mode, a, b, carry, state->A, expectFlags.bit.Z);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}
					if (state->FLAGS.bit.V != expectFlags.bit.V) {
						fprintf(stderr, "Overflow bit not set correctly in %s addAccumulator %X + %X + %s = %X - Expected (%d)\n", mode, a, b, carry, state->A, expectFlags.bit.V);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}
					if (state->FLAGS.bit.N != expectFlags.bit.N) {
						fprintf(stderr, "Negative bit not set correctly in %s addAccumulator %X + %X + %s = %X - Expected (%d)\n", mode, a, b, carry, state->A, expectFlags.bit.N);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}

					// Check cycles
					if (cycles != 0) {
						fprintf(stderr, "Incorrect cycles used in %s addAccumulator %X + %X + %s = %X. Expected 1 got %d\n", mode, a, b, carry, state->A, cycles);
						ASSERT_TRUE(false) << "Error testing addAccumulator, see stderr for details.";
					}
				}
			}
		}
		
		virtual void SetUp() {
			cpu = new CPUInternal(state, memory, &loader);
		}

		virtual void TearDown() {
			delete cpu;
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
		(*memory)[0xFFFC] = INS_NOP_IMP.opcode;		//insert a NOP instruction for the test
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

	/* Test Branch */
	TEST_F(TestCPU, TestBranchUpNoPage) {
		// Given:
		state->PC = rand() & 0xFF7F;	// Bottom half of page
		s8 offset = (rand() & 0x7F);	// Disable sign bit
		Word expectPC = (state->PC + offset);
		u8 cycles = 0;

		// When:
		cpu->branch(cycles, offset);

		// Then:
		EXPECT_EQ(state->PC, expectPC);
		EXPECT_EQ(cycles, 1);
	}
	TEST_F(TestCPU, TestBranchDownNoPage) {
		// Given:
		state->PC = rand() | 0x0080;	// Top half of page
		s8 offset = (rand() | 0x80);	// enable sign bit
		Word expectPC = (state->PC + offset);
		u8 cycles = 0;

		// When:
		cpu->branch(cycles, offset);

		// Then:
		EXPECT_EQ(state->PC, expectPC);
		EXPECT_EQ(cycles, 1);
	}
	TEST_F(TestCPU, TestBranchUpPage) {
		// Given:
		state->PC = rand() | 0x00F0;	// Near top of page
		s8 offset = (rand() & 0x7F);	// Disable sign bit
		offset |= 0x10;					// Must be at least this big to cross boundary
		Word expectPC = (state->PC + offset);
		u8 cycles = 0;

		// When:
		cpu->branch(cycles, offset);

		// Then:
		EXPECT_EQ(state->PC, expectPC);
		EXPECT_EQ(cycles, 2);
	}
	TEST_F(TestCPU, TestBranchDownPage) {
		// Given:
		state->PC = rand() & 0xFF0F;	// Near bottom of page
		s8 offset = (rand() | 0x80);	// enable sign bit, 
		offset &= 0x8F;					// Ensures it is negtive enough to cross tha page
		Word expectPC = (state->PC + offset);
		u8 cycles = 0;

		// When:
		cpu->branch(cycles, offset);

		// Then:
		EXPECT_EQ(state->PC, expectPC);
		EXPECT_EQ(cycles, 2);
	}

	/* Test pullStackWord and pushStackWord */
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
			testValue.byte = (Byte)i;
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
		EXPECT_EQ(state->SP, (initialSP + 1) & 0x00FF);
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

	/* Test carry getter & setter */
	TEST_F(TestCPU, carryGetSet) {
		EXPECT_EQ(CPU::FLAG_CARRY, 0);
		testFlagGetterSetter(CPU::FLAG_CARRY);
	}

	/* Test zero getter & setter */
	TEST_F(TestCPU, zeroGetSet) {
		EXPECT_EQ(CPU::FLAG_ZERO, 1);
		testFlagGetterSetter(CPU::FLAG_ZERO);
	}

	/* Test inerupt disable getter & setter */
	TEST_F(TestCPU, interuptDisableGetSet) {
		EXPECT_EQ(CPU::FLAG_INTERRUPT_DISABLE, 2);
		testFlagGetterSetter(CPU::FLAG_INTERRUPT_DISABLE);
	}

	/* Test decimal getter & setter */
	TEST_F(TestCPU, decimalGetSet) {
		EXPECT_EQ(CPU::FLAG_DECIMAL, 3);
		testFlagGetterSetter(CPU::FLAG_DECIMAL);
	}

	/* Test break getter & setter */
	TEST_F(TestCPU, breakGetSet) {
		EXPECT_EQ(CPU::FLAG_BREAK, 4);
		testFlagGetterSetter(CPU::FLAG_BREAK);
	}

	/* Test unused getter & setter */
	TEST_F(TestCPU, unusedGetSet) {
		EXPECT_EQ(CPU::FLAG_UNUSED, 5);
		testFlagGetterSetter(CPU::FLAG_UNUSED);
	}
	
	/* Test overflow getter & setter */
	TEST_F(TestCPU, overflowGetSet) {
		EXPECT_EQ(CPU::FLAG_OVERFLOW, 6);
		testFlagGetterSetter(CPU::FLAG_OVERFLOW);
	}

	/* Test negative getter & setter */
	TEST_F(TestCPU, negativeGetSet) {
		EXPECT_EQ(CPU::FLAG_NEGATIVE, 7);
		testFlagGetterSetter(CPU::FLAG_BREAK);
	}

	/* Test readReference Byte can read from register */
	TEST_F(TestCPU, readReferenceByteReg) {
		u8 registerIdx[] = { CPU::REGISTER_A, CPU::REGISTER_X, CPU::REGISTER_Y };
		Byte* registerRef[] = { &state->A, &state->X, &state->Y };
		for (int i = 0; i < 3; i++) {
			// Given:
			Reference ref = Reference{ CPU::REFERENCE_REG, registers[i]};
			Byte testValue = rand();
			*registerRef[i] = testValue;
			Byte cycles = 0;

			// When:
			Byte result = cpu->readReferenceByte(cycles, ref);

			// Then:
			EXPECT_EQ(cycles, 0);
			EXPECT_EQ(result, testValue);
		}
		
	}

	/* Test writeReferenceByte can write to register */
	TEST_F(TestCPU, writeReferenceByteReg) {
		u8 registerIdx[] = { CPU::REGISTER_A, CPU::REGISTER_X, CPU::REGISTER_Y };
		Byte* registerRef[] = { &state->A, &state->X, &state->Y };
		for (int i = 0; i < 3; i++) {
			// Given:
			Reference ref = Reference{ CPU::REFERENCE_REG, registers[i] };
			Byte testValue = rand();
			*registerRef[i] = ~testValue;
			Byte cycles = 0;

			// When:
			cpu->writeReferenceByte(cycles, ref, testValue);

			// Then:
			EXPECT_EQ(cycles, 0);
			EXPECT_EQ(*registerRef[i], testValue);
		}
	}

	/* Test readReference Byte can read from memory */
	TEST_F(TestCPU, readReferenceByteMem) {
		// Given:
		Byte testAddress = rand();
		Byte testValue = rand();
		Reference ref = Reference{ CPU::REFERENCE_MEM, testAddress };
		(*memory)[testAddress] = testValue;
		Byte cycles = 0;
		
		// When:
		Byte result = cpu->readReferenceByte(cycles, ref);

		// Then:
		EXPECT_EQ(cycles, 1);
		EXPECT_EQ(result, testValue);
	}

	/* Test writeReferenceByte can write to memory */
	TEST_F(TestCPU, writeReferenceByteMem) {
		// Given:
		Byte testAddress = rand();
		Byte testValue = rand();
		Reference ref = Reference{ CPU::REFERENCE_MEM, testAddress };
		(*memory)[testAddress] = ~testValue;
		Byte cycles = 0;

		// When:
		cpu->writeReferenceByte(cycles, ref, testValue);

		// Then:
		EXPECT_EQ(cycles, 1);
		EXPECT_EQ((*memory)[testAddress], testValue);
	}

	/* Test the add accumulator function */
	TEST_F(TestCPU, testAddAccumulatorBinaryNoCarryIn) { testAddAccumulator(false, false); }
	TEST_F(TestCPU, testAddAccumulatorBinaryWithCarryIn) { testAddAccumulator(false, true); }
	TEST_F(TestCPU, testAddAccumulatorDecimalNoCarryIn) { testAddAccumulator(true, false); }
	TEST_F(TestCPU, testAddAccumulatorDecimalWithCarryIn) { testAddAccumulator(true, true); }
		
	/* Test the sub accumulator function 
	TEST_F(TestCPU, testSubAccumulatorBinaryNoCarryIn) { EXPECT_TRUE(false); }	// TODO
	TEST_F(TestCPU, testSubAccumulatorBinaryWithCarryIn) { EXPECT_TRUE(false); }	// TODO
	TEST_F(TestCPU, testSubAccumulatorDecimalNoCarryIn) { EXPECT_TRUE(false);  }	// TODO
	TEST_F(TestCPU, testSubAccumulatorDecimalWithCarryIn) { EXPECT_TRUE(false); }	// TODO
	*/
}

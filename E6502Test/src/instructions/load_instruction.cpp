#include <gmock/gmock.h>
#include "instruction_utils.h"
#include "load_instruction.h"

namespace E6502 {
	using::testing::_;

	struct MockState : public CPUState {
		MOCK_METHOD(void, saveToRegAndFlag, (Byte* reg, Byte value));
	};


	class TestLoadInstruction : public testing::Test {
	public:
		Memory* memory;
		MockState* state;
		CPU* cpu;
		InstructionLoader loader;

		TestLoadInstruction() {
			memory = nullptr;
			state = nullptr;
			cpu = nullptr;
		}

		virtual void SetUp() {
			memory = new Memory;
			state = new MockState;
			cpu = new CPU(state, memory, &loader);

		}

		virtual void TearDown() {
			delete memory;
			delete state;
			delete cpu;
		}

		/* Creates a test value (if not provided), ensures the target reg doesnt contain it and returns the testvalue */
		Byte genTestValAndClearTargetReg(Byte* targetReg, Byte testValue = 0x00) {
			if (testValue == 0x00) {
				// Either no value was provided or 0 was. 0 is not a good option for testing
				testValue = 0x42; //TODO randomise?
			}
			(*targetReg) = ~testValue;
			return testValue;
		}

		/* Helper function to test Immediate instructions */
		void testImmediate(InstructionHandler instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			memory->data[0x0000] = testValue;

			// Expected call
			EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValue)).Times(1);

			// When:
			cyclesUsed = LoadInstruction::immediateHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, 2);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(InstructionHandler instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Byte insAddress = 0x84;		// TODO - maybe randmomise?
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			memory->data[0x0000] = insAddress;
			memory->data[insAddress] = testValue;

			// Expected call
			EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValue)).Times(1);

			// When:
			cyclesUsed = LoadInstruction::zeroPageHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte baseAddress[] = { 0x84, 0x12, 0x44 };			// TODO - maybe randmomise?
			Byte testIndex[] = { 0x10, 0xFF, 0x00 };			// TODO - maybe randmomise?
			Word targetAddress[] = { 0x0094, 0x0011, 0x0044 };	// = baseAddress[i] + testX[i] | 0xFF
			Byte testValue[] = { 0x42, 0xF0, 0x01 };			// TODO - maybe randomise?
			u8 cyclesUsed = 0;

			for (u8 i = 0; i < 3; i++) {
				// Load fixtures to memory
				state->PC = 0x0000;
				memory->data[0x000] = baseAddress[i];
				memory->data[targetAddress[i]] = testValue[i];

				// Given:
				*indexReg = testIndex[i];
				genTestValAndClearTargetReg(targetReg, testValue[i]);

				// Expected call
				EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValue[i])).Times(1);

				// When:
				cyclesUsed = LoadInstruction::zeroPageIndexedHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(cyclesUsed, expected_cycles);
			}
		}

		/* Helper function to test Absolute instructions */
		void testAbsolute(InstructionHandler instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte lsb = 0x84;			// TODO - maybe randomise?
			Byte msb = 0xBE;			// TODO - maybe randomise?
			Word targetAddress = 0xBE84;
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			memory->data[0x000] = lsb;
			memory->data[0x001] = msb;
			memory->data[targetAddress] = testValue;

			// Expected call
			EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValue)).Times(1);


			// When:
			cyclesUsed = LoadInstruction::absoluteHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper function to test Absolute Indexed instructions */
		void absIndexedHelper(InstructionHandler instruction, Byte lsb, Byte msb, Byte index, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Word targetAddress = (msb << 8) + lsb + index;
			u8 cyclesUsed = 0;

			// Load fixtures to memory
			memory->data[0x000] = lsb;
			memory->data[0x001] = msb;
			testValue = genTestValAndClearTargetReg(targetReg);

			// Expected call
			EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValue)).Times(1);

			// Given:
			memory->data[targetAddress] = testValue;
			state->PC = 0x0000;
			*indexReg = index;

			// When:
			cyclesUsed = LoadInstruction::absoluteHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper for indrect indexed and indexed indirect instructions */
		void testIndirectXIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		//TODO Randomise?
			u8 cyclesUsed;

			memory->data[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				// Given:
				state->PC = 0x0000;
				genTestValAndClearTargetReg(targetReg, testValues[i]);
				memory->data[dataAddress[i]] = testValues[i];
				*indexReg = testArguments[i];
				Byte zpAddr = zpBaseAddress + testArguments[i];
				memory->data[zpAddr] = dataAddress[i] & 0x00FF;
				memory->data[zpAddr + 1] = dataAddress[i] >> 8;

				// Expected call
				EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValues[i])).Times(1);

				// When:
				cyclesUsed = LoadInstruction::indirectHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(cyclesUsed, expectedCycles);
			}
		}

		/** Helper for indirect indexed and indexed indirect instructions */
		void testIndirectYIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		// TODO Randomise?
			Byte cyclePageCorrection[] = { 0 , 1 };			// Add 1 to expected cycles for INDY when crossing page
			u8 cyclesUsed;

			memory->data[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				u8 testCycles = expectedCycles;

				// Given:
				state->PC = 0x0000;
				genTestValAndClearTargetReg(targetReg, testValues[i]);
				memory->data[dataAddress[i]] = testValues[i];
				*indexReg = testArguments[i];
				Word unIndexed = dataAddress[i] - testArguments[i];
				memory->data[zpBaseAddress] = unIndexed & 0x00FF;
				memory->data[zpBaseAddress + 1] = unIndexed >> 8;
				testCycles += cyclePageCorrection[i];	// Increase cycles by 1 if crossing page

				// Expected call
				EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValues[i])).Times(1);

				// When:
				cyclesUsed = LoadInstruction::indirectHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(cyclesUsed, testCycles);
			}
		}
	};

	/* Test correct OpCodes */
	TEST_F(TestLoadInstruction, TestInstructionDefs) {
		// LDA Instructions
		EXPECT_EQ(INS_LDA_IMM.opcode, 0xA9);
		EXPECT_EQ(INS_LDA_ZP.opcode, 0xA5);
		EXPECT_EQ(INS_LDA_ZPX.opcode, 0xB5);
		EXPECT_EQ(INS_LDA_ABS.opcode, 0xAD);
		EXPECT_EQ(INS_LDA_ABSX.opcode, 0xBD);
		EXPECT_EQ(INS_LDA_ABSY.opcode, 0xB9);
		EXPECT_EQ(INS_LDA_INDX.opcode, 0xA1);
		EXPECT_EQ(INS_LDA_INDY.opcode, 0xB1);

		// LDX Instructions
		EXPECT_EQ(INS_LDX_IMM.opcode, 0xA2);
		EXPECT_EQ(INS_LDX_ZP.opcode, 0xA6);
		EXPECT_EQ(INS_LDX_ZPY.opcode, 0xB6);
		EXPECT_EQ(INS_LDX_ABS.opcode, 0xAE);
		EXPECT_EQ(INS_LDX_ABSY.opcode, 0xBE);

		// LDY Instructions
		EXPECT_EQ(INS_LDY_IMM.opcode, 0xA0);
		EXPECT_EQ(INS_LDY_ZP.opcode, 0xA4);
		EXPECT_EQ(INS_LDY_ZPX.opcode, 0xB4);
		EXPECT_EQ(INS_LDY_ABS.opcode, 0xAC);
		EXPECT_EQ(INS_LDY_ABSX.opcode, 0xBC);
	}

	/* Test addHandlers function */
	TEST_F(TestLoadInstruction, TestLDAaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		LoadInstruction::addHandlers(handlers);

		// Then: For all LD instructions, Expect *handlers[opcode] to point to a handler with the same opcode
		EXPECT_EQ(handlers[INS_LDA_IMM.opcode]->opcode, INS_LDA_IMM.opcode);
		EXPECT_EQ(handlers[INS_LDA_ZP.opcode]->opcode, INS_LDA_ZP.opcode);
		EXPECT_EQ(handlers[INS_LDA_ZPX.opcode]->opcode, INS_LDA_ZPX.opcode);
		EXPECT_EQ(handlers[INS_LDA_ABS.opcode]->opcode, INS_LDA_ABS.opcode);
		EXPECT_EQ(handlers[INS_LDA_ABSX.opcode]->opcode, INS_LDA_ABSX.opcode);
		EXPECT_EQ(handlers[INS_LDA_ABSY.opcode]->opcode, INS_LDA_ABSY.opcode);
		EXPECT_EQ(handlers[INS_LDA_INDX.opcode]->opcode, INS_LDA_INDX.opcode);
		EXPECT_EQ(handlers[INS_LDA_INDY.opcode]->opcode, INS_LDA_INDY.opcode);

		EXPECT_EQ(handlers[INS_LDX_IMM.opcode]->opcode, INS_LDX_IMM.opcode);
		EXPECT_EQ(handlers[INS_LDX_ZP.opcode]->opcode, INS_LDX_ZP.opcode);
		EXPECT_EQ(handlers[INS_LDX_ZPY.opcode]->opcode, INS_LDX_ZPY.opcode);
		EXPECT_EQ(handlers[INS_LDX_ABS.opcode]->opcode, INS_LDX_ABS.opcode);
		EXPECT_EQ(handlers[INS_LDX_ABSY.opcode]->opcode, INS_LDX_ABSY.opcode);

		EXPECT_EQ(handlers[INS_LDY_IMM.opcode]->opcode, INS_LDY_IMM.opcode);
		EXPECT_EQ(handlers[INS_LDY_ZP.opcode]->opcode, INS_LDY_ZP.opcode);
		EXPECT_EQ(handlers[INS_LDY_ZPX.opcode]->opcode, INS_LDY_ZPX.opcode);
		EXPECT_EQ(handlers[INS_LDY_ABS.opcode]->opcode, INS_LDY_ABS.opcode);
		EXPECT_EQ(handlers[INS_LDY_ABSX.opcode]->opcode, INS_LDY_ABSX.opcode);
	}

	/* Test fetchAndSaveToRegister */
	TEST_F(TestLoadInstruction, TestfetchAndSave) {
		Byte* registers[] = { &state->A, &state->X, &state->Y };
		Byte testValues[] = { 0x21, 0x42, 0x84 };
		Word testAddresses[] = { 0x1234, 0xf167, 0x0200 };

		for (int i = 0; i < 3; i++) {
			// Given
			memory->data[testAddresses[i]] = testValues[i];
			*registers[i] = ~testValues[i];
			u8 cycles = 0;

			// Expect to set flags
			EXPECT_CALL(*state, saveToRegAndFlag(registers[i], testValues[i])).Times(1);

			// When:
			LoadInstruction::fetchAndSaveToRegister(&cycles,cpu, testAddresses[i], registers[i]);

			// Then:
			EXPECT_EQ(cycles, 1);
		}
	}

	 /***     Execution tests     ***/

	 /* Tests LD Immediate Instruction */
	TEST_F(TestLoadInstruction, TestLoadImmediate) {
		testImmediate(INS_LDA_IMM, &state->A, 2, "LDA_IMM");
		testImmediate(INS_LDX_IMM, &state->X, 2, "LDX_IMM");
		testImmediate(INS_LDY_IMM, &state->Y, 2, "LDY_IMM");
	}

	/* Tests LD Zero Page Instruction */
	TEST_F(TestLoadInstruction, TestLoadZeroPage) {
		testZeroPage(INS_LDA_ZP, &state->A, 3, "LDA_ZP");
		testZeroPage(INS_LDX_ZP, &state->X, 3, "LDX_ZP");
		testZeroPage(INS_LDY_ZP, &state->Y, 3, "LDY_ZP");
	}

	/* Tests LD Zero Page,X/Y Instruction */
	TEST_F(TestLoadInstruction, TestLoadZeroPageX) {
		testZeroPageIndex(INS_LDY_ZPX, &state->X, &state->Y, 4, "LDY_ZPX");
		testZeroPageIndex(INS_LDA_ZPX, &state->X, &state->A, 4, "LDA_ZPX");
		testZeroPageIndex(INS_LDX_ZPY, &state->Y, &state->X, 4, "LDX_ZPY");
	}

	/* Tests LD Absolute Instruction */
	TEST_F(TestLoadInstruction, TestLoadAbsolute) {
		testAbsolute(INS_LDA_ABS, &state->A, 4, "LDA_ABS");
		testAbsolute(INS_LDX_ABS, &state->X, 4, "LDX_ABS");
		testAbsolute(INS_LDY_ABS, &state->Y, 4, "LDY_ABS");
	}

	/* Tests LD Absolute,X/Y Instruction */
	TEST_F(TestLoadInstruction, TestLoadAbsoluteXY) {
		Byte lsb = 0x84;			// TODO - maybe randomise?
		Byte msb = 0xFF;			// TODO - maybe randomise?
		Byte index = 0x00;
		u8 cyclesUsed = 0;

		index = 0x10;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, &state->X, &state->A, 4, "LDA ABSX (no overflow or page)");
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, &state->Y, &state->A, 4, "LDA ABSY (no overflow or page)");
		absIndexedHelper(INS_LDX_ABSY, lsb, msb, index, &state->Y, &state->X, 4, "LDX ABSY (no overflow or page)");
		absIndexedHelper(INS_LDY_ABSX, lsb, msb, index, &state->X, &state->Y, 4, "LDY ABSX (no overflow or page)");

		index = 0xA5;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, &state->X, &state->A, 5, "LDA ABSX (overflow)");
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, &state->Y, &state->A, 5, "LDA ABSY (overflow)");
		absIndexedHelper(INS_LDX_ABSY, lsb, msb, index, &state->Y, &state->X, 5, "LDX ABSY (overflow)");
		absIndexedHelper(INS_LDY_ABSX, lsb, msb, index, &state->X, &state->Y, 5, "LDY ABSX (overflow)");

		msb = 0x37;
		index = 0xA1;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, &state->X, &state->A, 5, "LDA ABSX (page boundry)");
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, &state->Y, &state->A, 5, "LDA ABSY (page boundry)");
		absIndexedHelper(INS_LDX_ABSY, lsb, msb, index, &state->Y, &state->X, 5, "LDX ABSY (page boundry)");
		absIndexedHelper(INS_LDY_ABSX, lsb, msb, index, &state->X, &state->Y, 5, "LDY ABSX (page boundry)");
	}

	/* Tests LD Indeirect,X Instruction */
	TEST_F(TestLoadInstruction, TestLoadIndirectX) {
		testIndirectXIndex(INS_LDA_INDX, &state->X, &state->A, 6, "LDA_INDX");
	}

	/* Tests LD Indeirect,Y Instruction */
	TEST_F(TestLoadInstruction, TestLoadIndirectY) {
		testIndirectYIndex(INS_LDA_INDY, &state->Y, &state->A, 5, "LDA_INDY");
	}
}

#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "ldaxy.h"

namespace E6502 {
	using::testing::_;

	struct MockState : public CPUState {
		MOCK_METHOD(void, saveToRegAndFlag, (Byte* reg, Byte value));
	};

	class TestLDAXYInstruction : public testing::Test {
	public:
		Memory* memory;
		MockState* state;

		virtual void SetUp() {
			memory = new Memory;
			state = new MockState;
		}

		virtual void TearDown() {
			delete memory;
			delete state;
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

		/* Helper fucntion to test Immediate instructions */
		void testImmediate(Byte instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
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
			cyclesUsed = LDAXY::immediateHandler(memory, state, &InstructionCode(instruction));

			// Then:
			EXPECT_EQ(cyclesUsed, 2);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(Byte instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
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
			cyclesUsed = LDAXY::zeroPageHandler(memory, state, &InstructionCode(instruction));

			// Then:
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(Byte instruction, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
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
				cyclesUsed = LDAXY::zeroPageIndexedHandler(memory, state, &InstructionCode(instruction));

				// Then:
				EXPECT_EQ(cyclesUsed, expected_cycles);

			}
		}

		/* Helper function ro test Absolute instructions */
		void testAbsolute(Byte instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte lsb = 0x84;			//TODO - maybe randomise?
			Byte msb = 0xBE;			//TODO - maybe randomise?
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
			cyclesUsed = LDAXY::absoluteHandler(memory, state, &InstructionCode(instruction));

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper function to test Absolute Indexed INstructions */
		void absIndexedHelper(Byte instruction, Byte lsb, Byte msb, Byte index, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
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
			cyclesUsed = LDAXY::absoluteHandler(memory, state, &InstructionCode(instruction));

			// Then:'
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper for indrect indexed and indexed indirect instructions */
		void testIndirectXIndex(Byte instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles, char* test_name) {
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
				cyclesUsed = LDAXY::indirectHandler(memory, state, &InstructionCode(instruction));

				//Then:
				EXPECT_EQ(cyclesUsed, expectedCycles);
			}
		}

		/** Helper for indrect indexed and indexed indirect instructions */
		void testIndirectYIndex(Byte instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		//TODO Randomise?
			Byte cyclePageCorrection[] = { 0 , 1 };			//Add 1 to expected cycles for INDY when crossing page
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
				testCycles += cyclePageCorrection[i];	//Increase cycles by 1 if crossing page

				// Expected call
				EXPECT_CALL(*state, saveToRegAndFlag(targetReg, testValues[i])).Times(1);

				// When:
				cyclesUsed = LDAXY::indirectHandler(memory, state, &InstructionCode(instruction));

				//Then:
				EXPECT_EQ(cyclesUsed, testCycles);
			}
		}
	};

	/* Test correct OpCodes */
	TEST_F(TestLDAXYInstruction, TestInstructionDefs) {
		EXPECT_EQ(INS_LDA_IMM, 0xA9);
		EXPECT_EQ(INS_LDA_ZP, 0xA5);
		EXPECT_EQ(INS_LDA_ZPX, 0xB5);
		EXPECT_EQ(INS_LDA_ABS, 0xAD);
		EXPECT_EQ(INS_LDA_ABSX, 0xBD);
		EXPECT_EQ(INS_LDA_ABSY, 0xB9);
		EXPECT_EQ(INS_LDA_INDX, 0xA1);
		EXPECT_EQ(INS_LDA_INDY, 0xB1);
	}

	/* Test addHandlers func adds all LDA handlers */
	TEST_F(TestLDAXYInstruction, TestLDAaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		LDAXY::addHandlers(handlers);

		// Then: For all LDA instructions, Expect *handlers[opcode] to point to a handler with the same opcode
		for (const InstructionHandler handler : LDAXY::instructions) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ(((*handlers[opcode]).opcode), opcode);
		}
	}

	/* Test  fetchAndSaveToRegister */ //(u8* cycles, Memory* memory, Byte* reg);
	TEST_F(TestLDAXYInstruction, TestfetchAndSave) {
		Byte* registers[] = { &state->A, &state->X, &state->Y };
		Byte testValues[] = { 0x21, 0x42, 0x84 };
		Word testAddresses[] = { 0x1234, 0xf167, 0x0200 };

		for (int i = 0; i < 3; i++) {
			// Given
			memory->data[testAddresses[i]] = testValues[i];
			*registers[i] = ~testValues[i];
			u8 cycles = 0;

			//Expect to set flags
			EXPECT_CALL(*state, saveToRegAndFlag(registers[i], testValues[i])).Times(1);

			// When:
			LDAXY::fetchAndSaveToRegister(&cycles, memory, state, testAddresses[i], registers[i]);

			// Then
			EXPECT_EQ(cycles, 1);
		}


	}

	TEST_F(TestLDAXYInstruction, TestGetRegFromInstruction) {
		Byte* testRegs[] = { &state->Y, &state->A, &state->X };		// Maps opcodes 0x00->Y, 0x01->A, 0x02->X

		for (InstructionHandler handler : LDAXY::instructions) {
			// Given:
			InstructionCode* instruction = new InstructionCode(handler.opcode);

			// When:
			Byte* result = LDAXY::getRegFromInstruction(instruction, state);

			// Then:
			EXPECT_EQ(result, testRegs[handler.opcode & 0x03]);

			// Clean Up
			delete instruction;
		}
	}

	/***************************************
	*        Execution tests follow        *
	* Tests insturctions execute correctly *
	****************************************/
	/* Tests LDA Immediate Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYImmediate) {
		testImmediate(INS_LDA_IMM, &state->A, 2, "LDA_IMM");
		testImmediate(INS_LDX_IMM, &state->X, 2, "LDX_IMM");
		testImmediate(INS_LDY_IMM, &state->Y, 2, "LDY_IMM");
	}

	/* Tests LD(A/X/Y) Zero Page Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPage) {
		testZeroPage(INS_LDA_ZP, &state->A, 3, "LDA_ZP");
		testZeroPage(INS_LDX_ZP, &state->X, 3, "LDX_ZP");
		testZeroPage(INS_LDY_ZP, &state->Y, 3, "LDY_ZP");
	}

	/* Tests LDA Zero Page,X/Y Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPageX) {
		testZeroPageIndex(INS_LDY_ZPX, &state->X, &state->Y, 4, "LDY_ZPX");
		testZeroPageIndex(INS_LDA_ZPX, &state->X, &state->A, 4, "LDA_ZPX");
		testZeroPageIndex(INS_LDX_ZPY, &state->Y, &state->X, 4, "LDX_ZPY");
	}

	/* Tests LDA Absolute Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYAbsolute) {
		testAbsolute(INS_LDA_ABS, &state->A, 4, "LDA_ABS");
		testAbsolute(INS_LDX_ABS, &state->X, 4, "LDX_ABS");
		testAbsolute(INS_LDY_ABS, &state->Y, 4, "LDY_ABS");
	}

	/* Tests LDA Absolute,X Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYAbsoluteXY) {
		Byte lsb = 0x84;			//TODO - maybe randomise?
		Byte msb = 0xFF;			//TODO - maybe randomise?
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

	/* Tests LDA Indeirect,X Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAIndirectX) {
		testIndirectXIndex(INS_LDA_INDX, &state->X, &state->A, 6, "LDA_INDX");
	}

	/* Tests LDA Indeirect,Y Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAIndirectY) {
		testIndirectYIndex(INS_LDA_INDY, &state->Y, &state->A, 5, "LDA_INDY");
	}
}

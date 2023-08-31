#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "ldaxy.h"

// TODO - Im not happy with flag checks, there is no test that actually checks instructions set flags correctly

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

		/* Helper method for testing InstructionHandler properties WARNING: This will delete the object when done*/
		void testPropsAndDelete(InstructionHandler* handler, Byte opCode, const char* name) {
			EXPECT_EQ((*handler).opcode, opCode);
			EXPECT_TRUE((*handler).isLegal);
			EXPECT_STREQ((*handler).name, name);
			delete handler;
		}

		/* Helper to test if a given values was saved to a given register 
		void testSave(u8 save_reg, Byte testValue, CPUState* testState, char* test_name) {
			Byte regValue = !testValue;		//TODO confirm this is valid
			switch (save_reg) {
				case CPUState::REGISTER_A:
					regValue = testState->A; break;
				case CPUState::REGISTER_X:
					regValue = testState->X; break;
				case CPUState::REGISTER_Y:
					regValue = testState->Y; break;
				default: {
					EXPECT_TRUE(false) << test_name << ": testSave() -> invalid target register provided";
				}
			}

			EXPECT_EQ(regValue, testValue) << test_name << " Did not set register correctly";
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

		/**
		 * Helper method for all the abs mode tests
		 * @param instruction		Instruction code to execude (From LDA::instructions)
		 * @param lsb				least significant byte of base address
		 * @param msb				most significant byre of base address
		 * @param index				index to use
		 * @param idx_mode			ABS_IDX_X if using X register, ABS_IDX_Y if using y register
		 * @param expected_cycles	The number of cycles the execution should take
		 * @param test_name			Name of the test (helps with debugging)
		*/
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
			
			// Set the index register if
			*indexReg = index;

			// When:
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:'
			//testSave(targetReg, testValue, testState, test_name);
			EXPECT_EQ(cyclesUsed, expected_cycles);
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
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
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
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

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
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(Byte instruction, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte baseAddress[]		= { 0x84, 0x12, 0x44 };			// TODO - maybe randmomise?
			Byte testIndex[]		= { 0x10, 0xFF, 0x00 };			// TODO - maybe randmomise?
			Word targetAddress[]	= { 0x0094, 0x0011, 0x0044 };	// = baseAddress[i] + testX[i] | 0xFF
			Byte testValue[]		= { 0x42, 0xF0, 0x01 };			// TODO - maybe randomise?
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
				cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

				// Then:
				EXPECT_EQ(cyclesUsed, expected_cycles);
				
			}
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
				cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

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
				cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

				//Then:
				EXPECT_EQ(cyclesUsed, testCycles);
			}
		}
	};

	/************************************************
	*                Execution tests                *
	* Tests the execute function operates correctly *
	*************************************************/
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

	/************************************************
	*              End Execution tests              *
	*************************************************

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
		for (const Byte& opcode : LDAXY::instructions) {
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ(((*handlers[opcode]).opcode), opcode);
		}
	}

	/*********************************************
	*				Handler Tests				 *
	* Check each handler is configured correctly *
	**********************************************/

	TEST_F(TestLDAXYInstruction, TestLDAXYImmediateHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_IMM), INS_LDA_IMM, "LDA - Load Accumulator [Immediate]");
		testPropsAndDelete(new LDAXYHandler(INS_LDX_IMM), INS_LDX_IMM, "LDX - Load Index Register X [Immediate]");
		testPropsAndDelete(new LDAXYHandler(INS_LDY_IMM), INS_LDY_IMM, "LDY - Load Index Register Y [Immediate]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPageHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ZP), INS_LDA_ZP, "LDA - Load Accumulator [Zero Page]");
		testPropsAndDelete(new LDAXYHandler(INS_LDX_ZP), INS_LDX_ZP, "LDX - Load Index Register X [Zero Page]");
		testPropsAndDelete(new LDAXYHandler(INS_LDY_ZP), INS_LDY_ZP, "LDY - Load Index Register Y [Zero Page]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPageXYHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ZPX), INS_LDA_ZPX, "LDA - Load Accumulator [X-Indexed Zero Page]");
		testPropsAndDelete(new LDAXYHandler(INS_LDX_ZPY), INS_LDX_ZPY, "LDX - Load Index Register X [Y-Indexed Zero Page]");
		testPropsAndDelete(new LDAXYHandler(INS_LDY_ZPX), INS_LDY_ZPX, "LDY - Load Index Register Y [X-Indexed Zero Page]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYAbsolteHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ABS), INS_LDA_ABS, "LDA - Load Accumulator [Absolute]");
		testPropsAndDelete(new LDAXYHandler(INS_LDX_ABS), INS_LDX_ABS, "LDX - Load Index Register X [Absolute]");
		testPropsAndDelete(new LDAXYHandler(INS_LDY_ABS), INS_LDY_ABS, "LDY - Load Index Register Y [Absolute]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYAbsoluteXYHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ABSX), INS_LDA_ABSX, "LDA - Load Accumulator [X-Indexed Absolute]");
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ABSY), INS_LDA_ABSY, "LDA - Load Accumulator [Y-Indexed Absolute]");

		testPropsAndDelete(new LDAXYHandler(INS_LDX_ABSY), INS_LDX_ABSY, "LDX - Load Accumulator [Y-Indexed Absolute]");

		testPropsAndDelete(new LDAXYHandler(INS_LDY_ABSX), INS_LDY_ABSX, "LDY - Load Accumulator [X-Indexed Absolute]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYIndirectXHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_INDX), INS_LDA_INDX, "LDA - Load Accumulator [X-Indexed Zero Page Indirect]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYIndirectYHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_INDY), INS_LDA_INDY, "LDA - Load Accumulator [Zero Page Indirect Y-Indexed]");
	}

	/*********************************************
	*			End of Handler Tests			 *
	**********************************************/
}

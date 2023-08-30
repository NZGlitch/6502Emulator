#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "ldaxy.h"

namespace E6502 {
	class TestLDAXYInstruction : public testing::Test {
	public:

		const static u8 ABS_IDX_X	= 0;		// Used by absIndexedHelper to indicate register X should be used for index
		const static u8 ABS_IDX_Y	= 1;		// Used by absIndexedHelper to indicate register Y should be used for index

		CPUState* state;
		Memory* memory;

		virtual void SetUp() {
			state = new CPUState;
			memory = new Memory;
		}

		virtual void TearDown() {
			delete state;
			delete memory;
		}

		/* Helper method for testing InstructionHandler properties WARNING: This will delete the object when done*/
		void testPropsAndDelete(InstructionHandler* handler, Byte opCode, const char* name) {
			EXPECT_EQ((*handler).opcode, opCode);
			EXPECT_TRUE((*handler).isLegal);
			EXPECT_STREQ((*handler).name, name);
			delete handler;
		}

		/* Helper to test if a given values was saved to a gien register */
		void testSave(u8 save_reg, Byte testValue, char* test_name) {
			Byte regValue = !testValue;		//TODO confirm this is valid
			switch (save_reg) {
				case CPUState::REGISTER_A:
					regValue = state->A; break;
				case CPUState::REGISTER_X:
					regValue = state->X; break;
				case CPUState::REGISTER_Y:
					regValue = state->Y; break;
				default: {
					EXPECT_TRUE(false) << test_name << ": testSave() -> invalid target register provided";
				}
			}

			EXPECT_EQ(regValue, testValue) << test_name << " Did not set register correctly";
		}

		/* Creates a test value (if not provided), ensures the target reg doesnt contain it and returns the testvalue */
		Byte genTestValAndClearTargetReg(u8 targetReg, Byte testValue=0x00) {
			if (testValue == 0x00) {
				// Either no value was provided or 0 was. 0 is not a good option for testing
				testValue = 0x42; //TODO randomise?
			}
			switch (targetReg) {
				case CPUState::REGISTER_A:
					state->A = ~testValue; break;
				case CPUState::REGISTER_X:
					state->X = ~testValue; break;
				case CPUState::REGISTER_Y:
					state->Y = ~testValue; break;
				default: {
					fprintf(stderr, "LD(AXY) Test: invalid target register provided");
					EXPECT_TRUE(false);
					return testValue;
				}
			}
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
		void absIndexedHelper(Byte instruction, Byte lsb, Byte msb, Byte index, u8 idx_mode, u8 targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Word targetAddress = (msb << 8) + lsb + index;
			u8 cyclesUsed = 0;

			// Load fixtures to memory
			memory->data[0x000] = lsb;
			memory->data[0x001] = msb;
			testValue = genTestValAndClearTargetReg(targetReg);

			// Given:
			memory->data[targetAddress] = testValue;	

			state->PC = 0x0000;
			// Set the X or Y register if needed
			if (idx_mode == ABS_IDX_X)
				state->X = index;
			else if (idx_mode == ABS_IDX_Y)
				state->Y = index;

			// When:
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:'
			testSave(targetReg, testValue, test_name);
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/* Helper function ro test Absolute instructions */
		void testAbsolute(Byte instruction, u8 targetReg, u8 expected_cycles, char* test_name) {
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
			

			// When:
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:
			testSave(targetReg, testValue, test_name);
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}
		
		/* Helper fucntion to test Immediate instructions */
		void testImmediate(Byte instruction, u8 targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			memory->data[0x0000] = testValue;

			// When:
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:
			testSave(targetReg, testValue, test_name);
			EXPECT_EQ(cyclesUsed, 2);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(Byte instruction, u8 targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Byte insAddress = 0x84;		// TODO - maybe randmomise?
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			memory->data[0x0000] = insAddress;
			memory->data[insAddress] = testValue;

			// When:
			cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

			// Then:
			testSave(targetReg, testValue, test_name);
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(Byte instruction, u8 indexReg, u8 targetReg, u8 expected_cycles, char* test_name) {
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
				switch (indexReg) {
					case CPUState::REGISTER_X: state->X = testIndex[i]; break;
					case CPUState::REGISTER_Y: state->Y = testIndex[i]; break;
					default: {
						EXPECT_TRUE(false) << test_name << ": testZeroPageIndex() -> invalid index register provided";
					}
				}
				genTestValAndClearTargetReg(targetReg, testValue[i]);
				
				// When:
				cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

				// Then:
				testSave(targetReg, testValue[i], test_name);
				EXPECT_EQ(cyclesUsed, expected_cycles);
			}
		}

		/** Helper for indrect indexed and indexed indirect instructions */
		void testIndirectIndex(Byte instruction, u8 indexReg, u8 targetReg, u8 expectedCycles, char* test_name) {
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

				if (indexReg == CPUState::REGISTER_X) {
					// If IndirectX, store dataAddress in ZP[zpBaseAddress + X]
					state->X = testArguments[i];
					Byte zpAddr = zpBaseAddress + testArguments[i];
					memory->data[zpAddr] = dataAddress[i] & 0x00FF;
					memory->data[zpAddr + 1] = dataAddress[i] >> 8;
				}
				else if (indexReg == CPUState::REGISTER_Y) {
					// Else if indirectY, store [dataAddress-Y] in ZP[arg]
					state->Y = testArguments[i];
					Word unIndexed = dataAddress[i] - testArguments[i];
					memory->data[zpBaseAddress] = unIndexed & 0x00FF;
					memory->data[zpBaseAddress + 1] = unIndexed >> 8;
					testCycles += cyclePageCorrection[i];	//Increase cycles by 1 if crossing page
				}
				else {
					// Invalid register provided
					EXPECT_TRUE(false) << test_name << ": testIndirectIndex() -> invalid index register provided";
				}

				// When:
				cyclesUsed = LDAXY::executeHandler(memory, state, &InstructionCode(instruction));

				//Then:
				testSave(targetReg, testValues[i], test_name);
				EXPECT_EQ(cyclesUsed, testCycles);
			}
		}

		/** Helper for setFlags test */
		void testFlags(u8 targetReg, Byte initFlags, Byte testValue, Byte expectFlags, char* test_name) {
			// Given:
			CPUState* testState = new CPUState;			//To avoid interference, use a fresh state each time
			testState->setFlags(initFlags);

			switch (targetReg) {
				case CPUState::REGISTER_A: testState->A = testValue; break;
				case CPUState::REGISTER_X: testState->X = testValue; break;
				case CPUState::REGISTER_Y: testState->Y = testValue; break;
				default: {
					EXPECT_TRUE(false) << test_name << ": testFlags() -> invalid target register provided";
				}
			}

			// When:
			LDAXY::setFlags(testState, targetReg);

			// Then:
			EXPECT_EQ(testState->getFlags(), expectFlags);	// Expect Z Unset

			delete testState;
		}
	};


	/************************************************
	*                Execution tests                *
	* Tests the execute function operates correctly *
	*************************************************/
	/* Tests LDA Immediate Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAImmediate) {
		testImmediate(INS_LDA_IMM, CPUState::REGISTER_A, 2, "LDA_IMM");
		testImmediate(INS_LDX_IMM, CPUState::REGISTER_X, 2, "LDX_IMM");
		testImmediate(INS_LDY_IMM, CPUState::REGISTER_Y, 2, "LDY_IMM");
	}

	/* Tests LD(A/X/Y) Zero Page Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPage) {
		testZeroPage(INS_LDA_ZP, CPUState::REGISTER_A, 3, "LDA_ZP");
		testZeroPage(INS_LDX_ZP, CPUState::REGISTER_X, 3, "LDX_ZP");
		testZeroPage(INS_LDY_ZP, CPUState::REGISTER_Y, 3, "LDY_ZP");
	}

	/* Tests LDA Zero Page,X/Y Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAXYZeroPageX) {
		testZeroPageIndex(INS_LDA_ZPX, CPUState::REGISTER_X, CPUState::REGISTER_A, 4, "LDA_ZPX");
		testZeroPageIndex(INS_LDY_ZPX, CPUState::REGISTER_X, CPUState::REGISTER_Y, 4, "LDY_ZPX");
		testZeroPageIndex(INS_LDX_ZPY, CPUState::REGISTER_Y, CPUState::REGISTER_X, 4, "LDX_ZPY");
	}

	/* Tests LDA Absolute Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAAbsolute) {
		testAbsolute(INS_LDA_ABS, CPUState::REGISTER_A, 4, "LDA_ABS");
//		testAbsolute(INS_LDX_ABS, CPUState::REGISTER_X, 4, "LDX_ABS");
//		testAbsolute(INS_LDY_ABS, CPUState::REGISTER_Y, 4, "LDY_ABS");
	}

	/* Tests LDA Absolute,X Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAAbsoluteX) {
		Byte lsb = 0x84;			//TODO - maybe randomise?
		Byte msb = 0xFF;			//TODO - maybe randomise?
		Byte index = 0x00;
		u8 cyclesUsed = 0;

		index = 0x10;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, CPUState::REGISTER_A, 4, "LDA ABSX (no overflow or page)");

		index = 0xA5;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, CPUState::REGISTER_A, 5, "LDA ABSX (overflow)");

		msb = 0x37;
		index = 0xA1;
		absIndexedHelper(INS_LDA_ABSX, lsb, msb, index, ABS_IDX_X, CPUState::REGISTER_A, 5, "LDA ABSX (page boundry)");
	}

	/* Tests LDA Absolute,Y Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAAbsoluteY) {
		Byte lsb = 0x84;			//TODO - maybe randomise?
		Byte msb = 0xFF;			//TODO - maybe randomise?
		Byte index = 0x00;
		u8 cyclesUsed = 0;

		index = 0x10;
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, CPUState::REGISTER_A, 4, "LDA ABSY (no overflow or page)");

		index = 0xA5;
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, CPUState::REGISTER_A, 5, "LDA ABSY (overflow)");

		msb = 0x37;
		index = 0xA1;
		absIndexedHelper(INS_LDA_ABSY, lsb, msb, index, ABS_IDX_Y, CPUState::REGISTER_A, 5, "LDA ABSY (page boundry)");
	}

	/* Tests LDA Indeirect,X Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAIndirectX) {
		testIndirectIndex(INS_LDA_INDX, CPUState::REGISTER_X, CPUState::REGISTER_A, 6, "LDA_INDX");
	}

	/* Tests LDA Indeirect,Y Instruction */
	TEST_F(TestLDAXYInstruction, TestLDAIndirectY) {
		testIndirectIndex(INS_LDA_INDY, CPUState::REGISTER_Y, CPUState::REGISTER_A, 5, "LDA_INDY");
	}

	/* Tests setFlags when N and Z flags 0 */
	TEST_F(TestLDAXYInstruction, TestLDAXYNoFlags) {
		// No Flags (unset exiting)
		testFlags(CPUState::REGISTER_A, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_A) NO ZN - change");
		testFlags(CPUState::REGISTER_X, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_X) NO ZN - change");
		testFlags(CPUState::REGISTER_Y, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_Y) NO ZN - change");

		// No Flags (Unchange existing)
		testFlags(CPUState::REGISTER_A, 0x00, 0x78, 0x00, "setFlags(REGISTER_A) NO ZN - no change");
		testFlags(CPUState::REGISTER_X, 0x00, 0x78, 0x00, "setFlags(REGISTER_X) NO ZN - no change");
		testFlags(CPUState::REGISTER_Y, 0x00, 0x78, 0x00, "setFlags(REGISTER_Y) NO ZN - no change");
	}

	/* Tests setFlags when Z flag changes */
	TEST_F(TestLDAXYInstruction, TestLDAXYZeroFlags) {
		// Z-Flag should be unset
		testFlags(CPUState::REGISTER_A, 0x02, 0x78, 0x00, "setFlags(REGISTER_A) unset Z");
		testFlags(CPUState::REGISTER_X, 0x02, 0x78, 0x00, "setFlags(REGISTER_X) unset Z");
		testFlags(CPUState::REGISTER_Y, 0x02, 0x78, 0x00, "setFlags(REGISTER_Y) unset Z");
		 
		// Z-Flag sould be set
		testFlags(CPUState::REGISTER_A, 0x00, 0x00, 0x02, "setFlags(REGISTER_A) set Z");
		testFlags(CPUState::REGISTER_X, 0x00, 0x00, 0x02, "setFlags(REGISTER_X) set Z");
		testFlags(CPUState::REGISTER_Y, 0x00, 0x00, 0x02, "setFlags(REGISTER_Y) set Z");
	}

	/* Tests setFlags when N flag changes */
	TEST_F(TestLDAXYInstruction, TestLDAXYNegFlags) {
		// N-Flag should be unset
		testFlags(CPUState::REGISTER_A, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_A) unset N");
		testFlags(CPUState::REGISTER_X, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_X) unset N");
		testFlags(CPUState::REGISTER_Y, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_Y) unset N");

		// N-Flag sould be set
		testFlags(CPUState::REGISTER_A, 0x00, 0x80, 0x80, "setFlags(REGISTER_A) set N");
		testFlags(CPUState::REGISTER_X, 0x00, 0x80, 0x80, "setFlags(REGISTER_X) set N");
		testFlags(CPUState::REGISTER_Y, 0x00, 0x80, 0x80, "setFlags(REGISTER_Y) set N");
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
//		testPropsAndDelete(new LDAXYHandler(INS_LDX_ABS), INS_LDA_ABS, "LDX - Load Accumulator [Absolute]");
//		testPropsAndDelete(new LDAXYHandler(INS_LDY_ABS), INS_LDA_ABS, "LDY - Load Accumulator [Absolute]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYAbsoluteXHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ABSX), INS_LDA_ABSX, "LDA - Load Accumulator [X-Indexed Absolute]");
	}

	TEST_F(TestLDAXYInstruction, TestLDAXYAbsoluteYHandlerProps) {
		testPropsAndDelete(new LDAXYHandler(INS_LDA_ABSY), INS_LDA_ABSY, "LDA - Load Accumulator [Y-Indexed Absolute]");
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

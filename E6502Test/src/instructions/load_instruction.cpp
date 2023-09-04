#include <gmock/gmock.h>
#include "instruction_utils.h"
#include "load_instruction.h"

// TODO - make sure to test saveRegAndTestFlags once this fuctonality is correctly ut in CPU

namespace E6502 {

	using::testing::_;

	class TestLoadInstruction : public testing::Test {
	public:
		Memory* memory;
		CPUState* state;
		CPU* cpu;
		Byte initPS;		// Tests are expected to return state staus flags back to this before completion

		virtual void SetUp() {
			initPS = rand();
			memory = new Memory;
			state = new CPUState;
			cpu = new CPU(state, memory, &InstructionUtils::loader);
			cpu->reset();
			state->PS = initPS;
		}

		virtual void TearDown() {
			EXPECT_EQ(state->PS, initPS);
			delete memory;
			delete state;
			delete cpu;
		}

		/* Helper method checks a status flags match the what would be set by the testvalue in a load instruction 
		 * Resets PS to initPS so the tead down test passes */
		void testAndResetPS(Byte testValue) {
			EXPECT_EQ(testValue == 0, state->Flag.Z);
			EXPECT_EQ(testValue >> 7, state->Flag.N);
			state->PS = initPS;
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
			u8 cyclesUsed = 1;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[0x0000] = testValue;

			// When:
			LoadInstruction::immediateHandler(cpu, cyclesUsed, instruction.opcode);

			// Then:
			testAndResetPS(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, 2);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(InstructionHandler instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Byte insAddress = 0x84;		// TODO - maybe randmomise?
			u8 cyclesUsed = 1;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[0x0000] = insAddress;
			(*memory)[insAddress] = testValue;

			// When:
			LoadInstruction::zeroPageHandler(cpu, cyclesUsed, instruction.opcode);

			// Then:
			testAndResetPS(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte baseAddress[] = { 0x84, 0x12, 0x44 };			// TODO - maybe randmomise?
			Byte testIndex[] = { 0x10, 0xFF, 0x00 };			// TODO - maybe randmomise?
			Word targetAddress[] = { 0x0094, 0x0011, 0x0044 };	// = baseAddress[i] + testX[i] | 0xFF
			Byte testValue[] = { 0x42, 0xF0, 0x01 };			// TODO - maybe randomise?

			for (u8 i = 0; i < 3; i++) {
				// Load fixtures to memory
				state->PC = 0x0000;
				(*memory)[0x000] = baseAddress[i];
				(*memory)[targetAddress[i]] = testValue[i];
				u8 cyclesUsed = 1;

				// Given:
				*indexReg = testIndex[i];
				genTestValAndClearTargetReg(targetReg, testValue[i]);

				// When:
				LoadInstruction::zeroPageIndexedHandler(cpu, cyclesUsed, instruction.opcode);

				// Then:
				testAndResetPS(testValue[i]);
				EXPECT_EQ(*targetReg, testValue[i]);
				EXPECT_EQ(cyclesUsed, expected_cycles);
			}
		}

		/* Helper function to test Absolute instructions */
		void testAbsolute(InstructionHandler instruction, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte lsb = 0x84;			// TODO - maybe randomise?
			Byte msb = 0xBE;			// TODO - maybe randomise?
			Word targetAddress = 0xBE84;
			u8 cyclesUsed = 1;

			// Given:
			state->PC = 0x0000;
			Byte testValue = genTestValAndClearTargetReg(targetReg);
			(*memory)[0x000] = lsb;
			(*memory)[0x001] = msb;
			(*memory)[targetAddress] = testValue;

			// When:
			LoadInstruction::absoluteHandler(cpu, cyclesUsed, instruction.opcode);

			// Then:
			testAndResetPS(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper function to test Absolute Indexed instructions */
		void absIndexedHelper(InstructionHandler instruction, Byte lsb, Byte msb, Byte index, Byte* indexReg, Byte* targetReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Word targetAddress = (msb << 8) + lsb + index;
			u8 cyclesUsed = 1;

			// Load fixtures to memory
			(*memory)[0x000] = lsb;
			(*memory)[0x001] = msb;
			testValue = genTestValAndClearTargetReg(targetReg);

			// Given:
			(*memory)[targetAddress] = testValue;
			state->PC = 0x0000;
			*indexReg = index;

			// When:
			LoadInstruction::absoluteHandler(cpu, cyclesUsed, instruction.opcode);

			// Then:
			testAndResetPS(testValue);
			EXPECT_EQ(*targetReg, testValue);
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper for indrect indexed and indexed indirect instructions */
		void testIndirectXIndex(InstructionHandler instruction, Byte* indexReg, Byte* targetReg, u8 expectedCycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		//TODO Randomise?

			(*memory)[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				// Given:
				u8 cyclesUsed = 1;
				state->PC = 0x0000;
				genTestValAndClearTargetReg(targetReg, testValues[i]);
				(*memory)[dataAddress[i]] = testValues[i];
				*indexReg = testArguments[i];
				Byte zpAddr = zpBaseAddress + testArguments[i];
				(*memory)[zpAddr] = dataAddress[i] & 0x00FF;
				(*memory)[zpAddr + 1] = dataAddress[i] >> 8;

				// When:
				LoadInstruction::indirectHandler(cpu, cyclesUsed, instruction.opcode);

				// Then:
				testAndResetPS(testValues[i]);
				EXPECT_EQ(*targetReg, testValues[i]);
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

			(*memory)[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				u8 testCycles = expectedCycles;
				u8 cyclesUsed = 1;

				// Given:
				state->PC = 0x0000;
				genTestValAndClearTargetReg(targetReg, testValues[i]);
				(*memory)[dataAddress[i]] = testValues[i];
				*indexReg = testArguments[i];
				Word unIndexed = dataAddress[i] - testArguments[i];
				(*memory)[zpBaseAddress] = unIndexed & 0x00FF;
				(*memory)[zpBaseAddress + 1] = unIndexed >> 8;
				testCycles += cyclePageCorrection[i];	// Increase cycles by 1 if crossing page

				// Expected call
				//EXPECT_CALL(state, saveToRegAndFlag(targetReg, testValues[i])).Times(1);

				// When:
				LoadInstruction::indirectHandler(cpu, cyclesUsed, instruction.opcode);

				// Then:
				testAndResetPS(testValues[i]);
				EXPECT_EQ(*targetReg, testValues[i]);
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
		//TODO - clean up this mess!
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
		u8 registerNames[] = { CPU::REGISTER_A, CPU::REGISTER_X, CPU::REGISTER_Y };
		Byte testValues[] = { 0x21, 0x42, 0x84 };
		Word testAddresses[] = { 0x1234, 0xf167, 0x0200 };

		for (int i = 0; i < 3; i++) {
			// Given
			(*memory)[testAddresses[i]] = testValues[i];
			*registers[i] = ~testValues[i];
			u8 cycles = 0;

			// When:
			LoadInstruction::fetchAndSaveToRegister(&cycles, cpu, testAddresses[i], registerNames[i]);

			// Then:
			testAndResetPS(testValues[i]);
			EXPECT_EQ(*registers[i], testValues[i]);
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

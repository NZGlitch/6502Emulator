#include <gmock/gmock.h>
#include "instruction_utils.h"
#include "store_instruction.h"

namespace E6502 {

	class TestStoreInstruction : public testing::Test {
	public:

		CPUState* state;
		Memory* memory;
		CPU* cpu;
		InstructionLoader loader;

		virtual void SetUp() {
			state = new CPUState;
			memory = new Memory;
			cpu = new CPU(state, memory, &loader);
		}

		virtual void TearDown() {
			delete state;
			delete memory;
			delete cpu;
		}

		/* Generates a test value if not provided, and ensures meomory location is clear */
		Byte genTestValAndClearMem(Memory* memory, Word address, Byte testValue = 0x42) {
			memory->data[address] = ~testValue;
			return testValue;
		}

		/* Helper function to test Absolute instructions */
		void testAbsolute(InstructionHandler instruction, Byte* sourceReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte lsb = 0x84;			// TODO - maybe randomise?
			Byte msb = 0xBE;			// TODO - maybe randomise?
			Word targetAddress = 0xBE84;
			Byte testValue = 0x42;		// TODO - maybe randomise?
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			*sourceReg = genTestValAndClearMem(memory, targetAddress);
			memory->data[0x000] = lsb;
			memory->data[0x001] = msb;
			
			// When:
			cyclesUsed = StoreInstruction::absoluteHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(memory->data[targetAddress], testValue);
			EXPECT_EQ(cyclesUsed, expected_cycles);
		}

		/** Helper function to test Absolute Indexed instructions */
		void absIndexedHelper(InstructionHandler instruction, Byte lsb, Byte msb, Byte index, Byte* indexReg, Byte* sourceReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Word targetAddress = (msb << 8) + lsb + index;
			u8 cyclesUsed = 0;

			// Load fixtures to memory
			memory->data[0x000] = lsb;
			memory->data[0x001] = msb;
			testValue = genTestValAndClearMem(memory, targetAddress);

			// Given:
			state->PC = 0x0000;
			*sourceReg = testValue;
			*indexReg = index;

			// When:
			cyclesUsed = StoreInstruction::absoluteHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
			EXPECT_EQ(memory->data[targetAddress], testValue);
		}

		/** Helper function for zero page instructions */
		void testZeroPage(InstructionHandler instruction, Byte* sourceReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte testValue = 0;
			Byte insAddress = 0x84;		// TODO - maybe randmomise?
			u8 cyclesUsed = 0;

			// Given:
			state->PC = 0x0000;
			testValue = genTestValAndClearMem(memory, insAddress);
			memory->data[0x0000] = insAddress;
			*sourceReg = testValue;

			// When:
			cyclesUsed = StoreInstruction::zeroPageHandler(cpu, instruction.opcode);

			// Then:
			EXPECT_EQ(memory->data[insAddress], testValue);
			EXPECT_EQ(cyclesUsed, 3);
		}

		/** Helper method for zero page index instructions */
		void testZeroPageIndex(InstructionHandler instruction, Byte* indexReg, Byte* sourceReg, u8 expected_cycles, char* test_name) {
			// Fixtures
			Byte baseAddress[] = { 0x84, 0x12, 0x44 };			// TODO - maybe randmomise?
			Byte testIndex[] = { 0x10, 0xFF, 0x00 };			// TODO - maybe randmomise?
			Word targetAddress[] = { 0x0094, 0x0011, 0x0044 };	// = baseAddress[i] + testX[i] | 0xFF
			Byte testValue[] = { 0x42, 0xF0, 0x01 };			// TODO - maybe randomise?
			u8 cyclesUsed = 0;

			for (u8 i = 0; i < 3; i++) {
				// Given
				state->PC = 0x0000;
				memory->data[0x000] = baseAddress[i];
				*sourceReg = testValue[i];
				*indexReg = testIndex[i];
				genTestValAndClearMem(memory, targetAddress[i], testValue[i]);

				// When:
				cyclesUsed = StoreInstruction::zeroPageIndexedHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(memory->data[targetAddress[i]], testValue[i]);
				EXPECT_EQ(cyclesUsed, expected_cycles);
			}
		}

		/** Helper for X-Indexed Zero Page Indrect instructions */
		void testIndirectXIndex(InstructionHandler instruction, Byte* sourceReg, u8 expected_cycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		//TODO Randomise?
			u8 cyclesUsed;

			memory->data[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				// Given:
				state->PC = 0x0000;
				genTestValAndClearMem(memory, dataAddress[i], testValues[i]);
				state->X = testArguments[i];
				*sourceReg = testValues[i];
				Byte zpAddr = zpBaseAddress + testArguments[i];
				memory->data[zpAddr] = dataAddress[i] & 0x00FF;
				memory->data[zpAddr + 1] = dataAddress[i] >> 8;

				// When:
				cyclesUsed = StoreInstruction::indirectXHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(memory->data[dataAddress[i]], testValues[i]);
				EXPECT_EQ(cyclesUsed, expected_cycles);
			}
		}

		/** Helper for Zero Page Y-Indexed Indirect instructions */
		void testIndirectYIndex(InstructionHandler instruction, Byte* targetReg, u8 expectedCycles, char* test_name) {
			Byte testArguments[] = { 0x10, 0xF0 };			// Test with and without overflow
			Byte testValues[] = { 0x42, 0xF1 };
			Byte zpBaseAddress = 0xE1;
			Word dataAddress[] = { 0x5A42, 0xCC05 };		// TODO Randomise?
			u8 cyclesUsed;

			memory->data[0x0000] = zpBaseAddress;

			for (u8 i = 0; i < 2; i++) {
				u8 testCycles = expectedCycles;

				// Given:
				state->PC = 0x0000;
				state->Y = testArguments[i];
				state->A = testValues[i];
				Word dataBaseAddress = (dataAddress[i] & 0xFF00) | (dataAddress[i] - testArguments[i]) & 0x00FF;
				memory->data[zpBaseAddress] = dataBaseAddress & 0x00FF;
				memory->data[zpBaseAddress + 1] = dataBaseAddress >> 8;
				genTestValAndClearMem(memory, dataAddress[i], testValues[i]);			// Clear target address

				// When:
				cyclesUsed = StoreInstruction::indirectYHandler(cpu, instruction.opcode);

				// Then:
				EXPECT_EQ(memory->data[dataAddress[i]], testValues[i]);
				EXPECT_EQ(cyclesUsed, testCycles);
			}
		}
	};

	/* Test correct OpCodes */
	TEST_F(TestStoreInstruction, TestInstructionDefs) {
		/* Absolute and Absolute Indexed Instructions */
		EXPECT_EQ(INS_STA_ABS.opcode, 0x8D);
		EXPECT_EQ(INS_STX_ABS.opcode, 0x8E);
		EXPECT_EQ(INS_STY_ABS.opcode, 0x8C);
		EXPECT_EQ(INS_STA_ABSX.opcode, 0x9D);
		EXPECT_EQ(INS_STA_ABSY.opcode, 0x99);

		/* Zero Page Instructions */
		EXPECT_EQ(INS_STA_ZP.opcode, 0x85);
		EXPECT_EQ(INS_STX_ZP.opcode, 0x86);
		EXPECT_EQ(INS_STY_ZP.opcode, 0x84);

		/* Zero Page X,Y Indexed Instructions */
		EXPECT_EQ(INS_STA_ZPX.opcode, 0x95);
		EXPECT_EQ(INS_STX_ZPY.opcode, 0x96);
		EXPECT_EQ(INS_STY_ZPX.opcode, 0x94);

		/* X-Indexed Zero Page Indirect instructions */
		EXPECT_EQ(INS_STA_INDX.opcode, 0x81);

		/* Zero Page Y-Indexed Indirect instructions */
		EXPECT_EQ(INS_STA_INDY.opcode, 0x91);

	}

	/* Test addHandlers func adds ST handlers */
	TEST_F(TestStoreInstruction, TestStoreaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		StoreInstruction::addHandlers(handlers);

		// Then: Expect *handlers[opcode] to point to a handler with the same opcode
		
		/* Absolute and Absolute Indexed Instructions */
		EXPECT_EQ(handlers[INS_STA_ABS.opcode]->opcode, INS_STA_ABS.opcode);
		EXPECT_EQ(handlers[INS_STX_ABS.opcode]->opcode, INS_STX_ABS.opcode);
		EXPECT_EQ(handlers[INS_STY_ABS.opcode]->opcode, INS_STY_ABS.opcode);
		EXPECT_EQ(handlers[INS_STA_ABSX.opcode]->opcode, INS_STA_ABSX.opcode);
		EXPECT_EQ(handlers[INS_STA_ABSY.opcode]->opcode, INS_STA_ABSY.opcode);

		/* Zero Page Instructions */
		EXPECT_EQ(handlers[INS_STA_ZP.opcode]->opcode, INS_STA_ZP.opcode);
		EXPECT_EQ(handlers[INS_STX_ZP.opcode]->opcode, INS_STX_ZP.opcode);
		EXPECT_EQ(handlers[INS_STY_ZP.opcode]->opcode, INS_STY_ZP.opcode);

		/* Zero Page Indexed Instructions */
		EXPECT_EQ(handlers[INS_STA_ZPX.opcode]->opcode, INS_STA_ZPX.opcode);
		EXPECT_EQ(handlers[INS_STX_ZPY.opcode]->opcode, INS_STX_ZPY.opcode);
		EXPECT_EQ(handlers[INS_STY_ZPX.opcode]->opcode, INS_STY_ZPX.opcode);

		/* X-Indexed Zero Page Indirect instructions */
		EXPECT_EQ(handlers[INS_STA_INDX.opcode]->opcode, INS_STA_INDX.opcode);

		/* Zero Page Y-Indexed Indirect instructions */
		EXPECT_EQ(handlers[INS_STA_INDY.opcode]->opcode, INS_STA_INDY.opcode);
	}

	/*******************************
	 ***     Execution tests     ***
	 *******************************/

	 /* Test Absolute Mode execution */
	TEST_F(TestStoreInstruction, TestStoreAbsolute) {
		testAbsolute(INS_STA_ABS, &state->A, 4, "STA_ABS");
		testAbsolute(INS_STX_ABS, &state->X, 4, "STX_ABS");
		testAbsolute(INS_STY_ABS, &state->Y, 4, "STY_ABS");
	}

	/* Tests STA Absolute,X/Y Instructions */
	TEST_F(TestStoreInstruction, TestStoreAbsoluteXY) {
		Byte lsb = 0x84;			// TODO - maybe randomise?
		Byte msb = 0xFF;			// TODO - maybe randomise?
		Byte index = 0x00;
		u8 cyclesUsed = 0;

		index = 0x10;
		absIndexedHelper(INS_STA_ABSX, lsb, msb, index, &state->X, &state->A, 5, "STA ABSX (no overflow or page)");
		absIndexedHelper(INS_STA_ABSY, lsb, msb, index, &state->Y, &state->A, 5, "STA ABSY (no overflow or page)");
		
		index = 0xA5;
		absIndexedHelper(INS_STA_ABSX, lsb, msb, index, &state->X, &state->A, 5, "STA ABSX (overflow)");
		absIndexedHelper(INS_STA_ABSY, lsb, msb, index, &state->Y, &state->A, 5, "STA ABSY (overflow)");
		
		msb = 0x37;
		index = 0xA1;
		absIndexedHelper(INS_STA_ABSX, lsb, msb, index, &state->X, &state->A, 5, "STA ABSX (page boundry)");
		absIndexedHelper(INS_STA_ABSY, lsb, msb, index, &state->Y, &state->A, 5, "STA ABSY (page boundry)");		
	}

	/* Tests LD Zero Page Instruction */
	TEST_F(TestStoreInstruction, TestLStoreZeroPage) {
		testZeroPage(INS_STA_ZP, &state->A, 3, "STA_ZP");
		testZeroPage(INS_STX_ZP, &state->X, 3, "STX_ZP");
		testZeroPage(INS_STY_ZP, &state->Y, 3, "STY_ZP");
	}

	/* Tests Store Zero Page,X/Y Instruction */
	TEST_F(TestStoreInstruction, TestStoreZeroPageX) {
		testZeroPageIndex(INS_STA_ZPX, &state->X, &state->A, 4, "STA_ZPX");
		testZeroPageIndex(INS_STX_ZPY, &state->Y, &state->X, 4, "STX_ZPY");
		testZeroPageIndex(INS_STY_ZPX, &state->X, &state->Y, 4, "STY_ZPX");
	}

	/* Tests Store Indirect,X Instruction */
	TEST_F(TestStoreInstruction, TestStoreIndirectX) {
		testIndirectXIndex(INS_STA_INDX, &state->A, 6, "STA_INDX");
	}

	/* Tests Store Indirect,Y Instruction */
	TEST_F(TestStoreInstruction, TestStoreIndirectY) {
		testIndirectYIndex(INS_STA_INDX, &state->A, 6, "STA_INDY");
	}
}

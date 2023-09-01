#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "store_instruction.h"

namespace E6502 {

	class TestStoreInstruction : public testing::Test {
	public:

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
			cyclesUsed = StoreInstruction::absoluteHandler(memory, state, instruction.opcode);

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
			cyclesUsed = StoreInstruction::absoluteHandler(memory, state, instruction.opcode);

			// Then:
			EXPECT_EQ(cyclesUsed, expected_cycles);
			EXPECT_EQ(memory->data[targetAddress], testValue);
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
}

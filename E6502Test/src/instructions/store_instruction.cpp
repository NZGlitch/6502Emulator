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
	};

	/* Test correct OpCodes */
	TEST_F(TestStoreInstruction, TestInstructionDefs) {
		EXPECT_EQ(INS_STA_ABS.opcode, 0x8D);
		EXPECT_EQ(INS_STX_ABS.opcode, 0x8E);
		EXPECT_EQ(INS_STY_ABS.opcode, 0x8C);
	}

	/* Test addHandlers func adds JSR handler */
	TEST_F(TestStoreInstruction, TestStoreaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		StoreInstruction::addHandlers(handlers);

		// Then: Expect *handlers[opcode] to point to a handler with the same opcode
		EXPECT_EQ(handlers[INS_STA_ABS.opcode]->opcode, INS_STA_ABS.opcode);
		EXPECT_EQ(handlers[INS_STX_ABS.opcode]->opcode, INS_STX_ABS.opcode);
		EXPECT_EQ(handlers[INS_STY_ABS.opcode]->opcode, INS_STY_ABS.opcode);
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
}

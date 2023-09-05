#include <gmock/gmock.h>
#include "instructions/instruction_utils.h"
#include "instructions/jump_instruction.h"
#include "instructions/load_instruction.h"
#include "instructions/store_instruction.h"

namespace E6502 {

	class TestInstructionUtils : public testing::Test {

	public:
		virtual void SetUp() {
		}

		virtual void TearDown() {
		}

		void testGetRegFromInsHelper(InstructionHandler handler) {
			Memory* memory = new Memory;
			CPUState* state = new CPUState;
			InstructionLoader* loader = new InstructionUtils::Loader;
			CPUInternal* cpu = new CPUInternal(state, memory, loader);

			u8 testRegs[] = { CPU::REGISTER_Y, CPU::REGISTER_A, CPU::REGISTER_X };		// Maps opcodes 0x00->Y, 0x01->A, 0x02->X

			// Given:
			Byte instruction = handler.opcode;

			// When:
			u8 result = InstructionUtils::getRegFromInstruction(instruction, cpu);

			// Then:
			EXPECT_EQ(result, testRegs[handler.opcode & 0x03]);

			delete cpu;
			delete loader;
			delete state;
			delete memory;
		}
	};


	/* Test all instructions are correctly added */
	TEST_F(TestInstructionUtils, TestInstructionDefs) {
		// Given:
		InstructionHandler* handlers[0x100];
		for (int i = 0; i < 0x100; i++) handlers[i] = nullptr;

		// When:
		InstructionUtils::loader.load(handlers);

		// Then (JumpInstruction):
		for (const InstructionHandler& handler : JUMP_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (LoadInstruction):
		for (const InstructionHandler& handler : LOAD_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (StoreInstruction):
		for (const InstructionHandler& handler : STORE_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}
	}

	/* Test getRegFromInstruction */
	TEST_F(TestInstructionUtils, TestGetRegFromInstruction) {
		
		for (const InstructionHandler& handler : LOAD_INSTRUCTIONS) {
			testGetRegFromInsHelper(handler);
		}

		for (const InstructionHandler& handler : STORE_INSTRUCTIONS) {
			testGetRegFromInsHelper(handler);
		}	
	}
}

#include <gmock/gmock.h>
#include "instructions/instruction_utils.h"

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

		// Then (BranchInstruction):
		for (const InstructionHandler& handler : BRANCH_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (IncDecInstruction):
		for (const InstructionHandler& handler : INCDEC_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

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

		// Then (LogicInstruction):
		for (const InstructionHandler& handler : LOGIC_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (ShiftInstruction):
		for (const InstructionHandler& handler : SHIFT_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Then (StackInstruction):
		for (const InstructionHandler& handler : STACK_INSTRUCTIONS) {
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

		// Then (TransferInstruction):
		for (const InstructionHandler& handler : TRANS_INSTRUCTIONS) {
			Byte opcode = handler.opcode;
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ((handlers[opcode]->opcode), opcode);
		}

		// Print instruction set
		Byte insCount = 0;
		char sname[4];
		printf("\nInstruction set loaded:\n");
		printf("\n-------------------------------------------------------------------------------------\n");
		printf("%5s", "");
		for (int col = 0; col < 0x10; col++) {
			printf("  x%X ", col);
		}
		printf("\n");
		for (int row = 0; row < 0x10; row++) {
			printf("%Xx:  ", row);
			for (int col = 0; col < 0x10; col++) {
				Byte opcode = (row * 0x10) | col;
				if (handlers[opcode] == nullptr) {
					sname[0] = sname[1] = sname[2] = ' ';
					sname[3] = '\0';
				}
				else {
					insCount++;
					const char* name = (*handlers[(row * 0x10) | col]).name;
					strncpy_s(sname, name, 3);
				}
				printf(" %3s ", sname);
			}
			printf("\n");
		}
		printf("Found %d instructions\n", insCount);
		printf("-------------------------------------------------------------------------------------\n\n");
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

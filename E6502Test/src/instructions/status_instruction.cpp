#include <gmock/gmock.h>
#include "instruction_test.h"
#include "status_instruction.h"

namespace E6502 {

	class TestStatusInstruction : public TestInstruction {
	public:
		void testFlagInstruction(InstructionHandler instruction, u8 flag, bool set) {
			// Given:
			Byte cycles = 0;
			cpu->setFlag(cycles, flag, !set);
			(*memory)[programSpace] = instruction.opcode;
			cycles = 0;

			// When:
			cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ(cycles, 2);
			EXPECT_EQ(cpu->getFlag(cycles, flag), set);
			state->FLAGS = initPS;
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestStatusInstruction, TestStatusHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_CLC_IMP, 0x18}, {INS_CLD_IMP, 0xD8}, {INS_CLI_IMP, 0x58}, {INS_CLV_IMP, 0xB8},
			{INS_SEC_IMP, 0x38}, {INS_SED_IMP, 0xF8}, {INS_SEI_IMP, 0x78},
		};
		testInstructionDef(instructions, StatusInstruction::addHandlers);
	}

	TEST_F(TestStatusInstruction, TestCLC) { testFlagInstruction(INS_CLC_IMP, CPU::FLAG_CARRY, false); }
	TEST_F(TestStatusInstruction, TestCLD) { testFlagInstruction(INS_CLD_IMP, CPU::FLAG_DECIMAL, false); }
	TEST_F(TestStatusInstruction, TestCLI) { testFlagInstruction(INS_CLI_IMP, CPU::FLAG_INTERRUPT_DISABLE, false); }
	TEST_F(TestStatusInstruction, TestCLV) { testFlagInstruction(INS_CLV_IMP, CPU::FLAG_OVERFLOW, false); }
	TEST_F(TestStatusInstruction, TestSEC) { testFlagInstruction(INS_SEC_IMP, CPU::FLAG_CARRY, true); }
	TEST_F(TestStatusInstruction, TestSED) { testFlagInstruction(INS_SED_IMP, CPU::FLAG_DECIMAL, true); }
	TEST_F(TestStatusInstruction, TestSEI) { testFlagInstruction(INS_SEI_IMP, CPU::FLAG_INTERRUPT_DISABLE, true); }
}

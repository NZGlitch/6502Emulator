#include <gmock/gmock.h>
#include "instruction_test.h"
#include "incdec_instruction.h"

namespace E6502 {

	class TestIncDecInstruction : public TestInstruction {
	
	public:

		//absolute, absx, zp, zpx, implied

		/* Helper method for testing operations using implied addressing mode */
		void testImplied(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) { }

		/* Helper method for testing operations using absolute addressing mode */
		void testAbsOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) { }

		/* Helper method for testing operations using absolute addressing-x mode */
		void testAbsXOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), Byte expectedCycles) {}

		/* Helper method for testing operations using zero page mode */
		void testZPOp(InstructionHandler instruction, Byte(*op)(Byte a, Byte b), u8 expectedCycles, bool useXIndex) { }
			
	};

	/* Test defs & addHandlers func */
	TEST_F(TestIncDecInstruction, TestLogicHandlers) {

		std::vector<InstructionMap> instructions = {
			// DEC Instructions
			
			// DEX Instructions
			
			// DEY Instructions
			
			// INC Instructions

			// INX Instructions

			// INY Instructions
			
		};
		testInstructionDef(instructions, IncDecInstruction::addHandlers);
	}

	/* Test execution */
	//TEST_F(TestLogicInstruction, TestEORImmediate) { testImmOp(INS_EOR_IMM, LogicInstruction::EOR, 2); }
}

#include <gmock/gmock.h>
#include "instruction_test.h"
#include "arithmetic_instruction.h"

namespace E6502 {
	using ::testing::_;
	using ::testing::Return;
	using ::testing::FieldsAre;

	// Matcher needed to check references for some mocked methods
	MATCHER_P2(referencesAreEqual, type, address, "") { return arg.referenceType == CPU::REFERENCE_MEM && arg.memoryAddress == address; }

	class TestArithmeticInstruction : public TestInstruction {
	public:
		void testAbsolute(InstructionHandler instruction, Byte* indexReg = nullptr) {
			// Given:
			Byte testValue = rand();
			Word targetAddress = dataSpace;
			Byte index = rand();
			if (indexReg != nullptr) {
				*indexReg = index;
				targetAddress += index;
			}

			(*memory)[programSpace] = instruction.opcode;

			// Then:
			if (indexReg != nullptr) EXPECT_CALL(*mockCPU, regValue(_, _)).Times(1).WillOnce(Return(index));
			EXPECT_CALL(*mockCPU, readPCWord(_)).Times(1).WillOnce(Return(dataSpace));
			EXPECT_CALL(*mockCPU, readReferenceByte(_, referencesAreEqual(CPU::REFERENCE_MEM, targetAddress))).Times(1).WillOnce(Return(testValue));
			EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

			// When
			u8 cycles = cpu->testExecute(1, mockCPU);
		}

		void testAbsoluteReal(InstructionHandler instruction, Byte* targetReg = nullptr) {
			Byte index = 0;
			Word targetAddress = dataSpace;
			if (targetReg != nullptr) {
				*targetReg = index;
				targetAddress = dataSpace + index;
			}

			// Given:
			Byte opA = rand(), opB = rand();
			Byte flag = (state->FLAGS.bit.C ? 1 : 0);
			state->A = opA;


			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[targetAddress] = opB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ(state->A, (Byte)(opA + opB + flag));
			EXPECT_EQ(cycles, 4);
			state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestArithmeticInstruction, TestArithmeticHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_ADC_IMM, 0x69}, {INS_ADC_ABS, 0x6D}, {INS_ADC_ABX, 0x7D}, {INS_ADC_ABY, 0x79},
		};
		testInstructionDef(instructions, ArithmeticInstruction::addHandlers);
	}	
	
	/** Tests correct flow of ADC Immediate */
	TEST_F(TestArithmeticInstruction, TestADCImmediate) { 
		// Given:
		Byte testValue = rand();
		(*memory)[programSpace] = INS_ADC_IMM.opcode;

		// Then:
		EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(testValue));
		EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

		// When:
		cpu->testExecute(1, mockCPU);
	}
	/* Tests that cant work with mocks (e.g. cycles over functional) */
	TEST_F(TestArithmeticInstruction, TestADCImmediateReal) {
		// Given:
		Byte opA = rand(), opB = rand();
		Byte flag = (state->FLAGS.bit.C ? 1 : 0);
		state->A = opA;

		(*memory)[programSpace] = INS_ADC_IMM.opcode;
		(*memory)[programSpace + 1] = opB;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->A, (Byte)(opA + opB + flag));
		EXPECT_EQ(cycles, 2);
		state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
	}

	// Absolute flow tests
	TEST_F(TestArithmeticInstruction, TestADCAbsolute) { testAbsolute(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteX) { testAbsolute(INS_ADC_ABX, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteY) { testAbsolute(INS_ADC_ABY, &state->Y); }

	// Absolute 'real' tests
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteReal) { testAbsoluteReal(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXReal) { testAbsoluteReal(INS_ADC_ABX, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYReal) { testAbsoluteReal(INS_ADC_ABY, &state->Y); }



	
}

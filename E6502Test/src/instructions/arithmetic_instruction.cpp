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
		void testAbsZP(InstructionHandler instruction, bool isZeroPage = false, Byte* indexReg = nullptr) {
			if (isZeroPage) dataSpace &= 0x00FF;

			// Given:
			Byte testValue = rand();
			Word targetAddress = dataSpace;
			Byte index = rand();
			if (indexReg != nullptr) {
				*indexReg = index;
				targetAddress += index;
			}

			if (isZeroPage) targetAddress &= 0x00FF;

			(*memory)[programSpace] = instruction.opcode;

			// Then:
			if (indexReg != nullptr) EXPECT_CALL(*mockCPU, regValue(_, _)).Times(1).WillOnce(Return(index));

			if (isZeroPage)
				EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(dataSpace & 0x00FF));
			else
				EXPECT_CALL(*mockCPU, readPCWord(_)).Times(1).WillOnce(Return(dataSpace));

			EXPECT_CALL(*mockCPU, readReferenceByte(_, referencesAreEqual(CPU::REFERENCE_MEM, targetAddress))).Times(1).WillOnce(Return(testValue));
			EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

			// When
			u8 cycles = cpu->testExecute(1, mockCPU);
		}

		void testAbsZPRealBinary(InstructionHandler instruction, bool isZeroPage = false, bool crossBoundry = false, Byte* targetReg = nullptr) {
			state->FLAGS.bit.D = false;	// ensure binary mode
			Byte expectedCycles = 4;
			Byte index = rand();

			if (isZeroPage) dataSpace &= 0x00FF;

			if (crossBoundry) {
				expectedCycles = 5;
				dataSpace |= 0x0080;
				index |= 0x80;
			}
			else {
				dataSpace &= 0xFF7F;
			}
			
			Word targetAddress = dataSpace;

			if (targetReg != nullptr) {
				*targetReg = index;
				targetAddress = dataSpace + index;
			}

			if (isZeroPage) {
				targetAddress &= 0x00FF;
				expectedCycles = (targetReg == nullptr ? 3 : 4);
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
			EXPECT_EQ(cycles, expectedCycles);
			state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
		}

		void testAbsZPRealDecimal(InstructionHandler instruction, bool isZeroPage = false, bool crossBoundry = false, Byte* targetReg = nullptr) {
			state->FLAGS.bit.D = true;	// ensure decimal mode
			Byte expectedCycles = 4;
			Byte index = rand();

			if (isZeroPage) dataSpace &= 0x00FF;

			if (crossBoundry) {
				expectedCycles = 5;
				dataSpace |= 0x0080;
				index |= 0x80;
			}
			else {
				dataSpace &= 0xFF7F;
			}

			Word targetAddress = dataSpace;

			if (targetReg != nullptr) {
				*targetReg = index;
				targetAddress = dataSpace + index;
			}

			if (isZeroPage) {
				targetAddress &= 0x00FF;
				expectedCycles = (targetReg == nullptr ? 3 : 4);
			}

			// Given:
			Byte opA = 0x07, opB = 0x25;	// would be good to test all valid values just to make sure its correct
			Byte flag = (state->FLAGS.bit.C ? 1 : 0);
			state->A = opA;


			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = dataSpace & 0x00FF;
			(*memory)[programSpace + 2] = dataSpace >> 8;
			(*memory)[targetAddress] = opB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			EXPECT_EQ(state->A, (Byte)(0x32 + flag)); // 7 = 25 = 32 + carry
			EXPECT_EQ(cycles, expectedCycles);
			EXPECT_EQ(state->FLAGS.bit.C, false);
			state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
		}
	};



	/* Test defs & addHandlers func */
	TEST_F(TestArithmeticInstruction, TestArithmeticHandlers) {

		std::vector<InstructionMap> instructions = {
			{INS_ADC_IMM, 0x69}, {INS_ADC_ABS, 0x6D}, {INS_ADC_ABX, 0x7D}, {INS_ADC_ABY, 0x79},
			{INS_ADC_ZP0, 0x65}, {INS_ADC_ZPX, 0x75}, {INS_ADC_INX, 0x61},
		};
		testInstructionDef(instructions, ArithmeticInstruction::addHandlers);
	}	
	
	/** Immediate tests */
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
	TEST_F(TestArithmeticInstruction, TestADCImmediateRealBinary) {
		// Given:
		Byte opA = rand(), opB = rand();
		Byte flag = (state->FLAGS.bit.C ? 1 : 0);
		state->A = opA;
		state->FLAGS.bit.D = false;	//Binary mode

		(*memory)[programSpace] = INS_ADC_IMM.opcode;
		(*memory)[programSpace + 1] = opB;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->A, (Byte)(opA + opB + flag));
		EXPECT_EQ(cycles, 2);
		state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
	}
	TEST_F(TestArithmeticInstruction, TestADCImmediateRealDecimal) {
		// Given:
		Byte opA = 0x72, opB = 0x59;
		Byte flag = (state->FLAGS.bit.C ? 1 : 0);
		state->A = opA;
		state->FLAGS.bit.D = true;	//Decimal mode

		(*memory)[programSpace] = INS_ADC_IMM.opcode;
		(*memory)[programSpace + 1] = opB;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->A, (Byte)(0x31 + flag));	// 72 + 59 + carry = 131+c = 0x31 + c
		EXPECT_EQ(state->FLAGS.bit.C, true);
		EXPECT_EQ(cycles, 2);
		state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
	}

	// Absolute flow tests
	TEST_F(TestArithmeticInstruction, TestADCAbsolute) { testAbsZP(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteX) { testAbsZP(INS_ADC_ABX, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteY) { testAbsZP(INS_ADC_ABY, false, &state->Y); }

	// Absolute 'real' tests (Binary)
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteRealBinary) { testAbsZPRealBinary(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealBinary) { testAbsZPRealBinary(INS_ADC_ABX, false, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealBinary) { testAbsZPRealBinary(INS_ADC_ABY, false, false, &state->Y); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealCrossBinary) { testAbsZPRealBinary(INS_ADC_ABX, false, true, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealCrossBinary) { testAbsZPRealBinary(INS_ADC_ABY, false, true, &state->Y); }

	// Absolute 'real' tests (Decimal)
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteRealDecimal) { testAbsZPRealDecimal(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealDecimal) { testAbsZPRealDecimal(INS_ADC_ABX, false, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealDecimal) { testAbsZPRealDecimal(INS_ADC_ABY, false, false, &state->Y); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealCrossDecimal) { testAbsZPRealDecimal(INS_ADC_ABX, false, true, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealCrossDecimal) { testAbsZPRealDecimal(INS_ADC_ABY, false, true, &state->Y); }

	// Zero Page flow tests
	TEST_F(TestArithmeticInstruction, TestADCZeroPage) { testAbsZP(INS_ADC_ZP0, true); }
	TEST_F(TestArithmeticInstruction, TestADCZeroPageX) { testAbsZP(INS_ADC_ZPX, true, &state->X); }

	// ZeroPAge 'real' tests
	TEST_F(TestArithmeticInstruction, TestADCZeroPageRealBinary) { testAbsZPRealBinary(INS_ADC_ZP0, true); }
	TEST_F(TestArithmeticInstruction, TestADCZeroPageXRealBinary) { testAbsZPRealBinary(INS_ADC_ZPX, true, false, &state->X); }

	// X-Indexed Zero Page Indirect tests
	TEST_F(TestArithmeticInstruction, TestADCIndirectX) {
		// Given:
		Byte testValue = rand();
		Byte zpBase = rand();
		Byte index = rand();
		Byte zpTarget = zpBase + index;

		state->X = index;
		(*memory)[programSpace] = INS_ADC_INX.opcode;

		// Then:
		EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(zpBase));
		EXPECT_CALL(*mockCPU, regValue(_, CPU::REGISTER_X)).Times(1).WillOnce(Return(index));
		EXPECT_CALL(*mockCPU, readWord(_, zpTarget)).Times(1).WillOnce(Return(dataSpace));
		EXPECT_CALL(*mockCPU, readReferenceByte(_, referencesAreEqual(CPU::REFERENCE_MEM, dataSpace))).Times(1).WillOnce(Return(testValue));
		EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

		// When:
		cpu->testExecute(1, mockCPU);
	}

	TEST_F(TestArithmeticInstruction, TestADCIndirectXRealBinary) {
		// Given:
		Byte opA = rand(), opB = rand();
		Byte flag = (state->FLAGS.bit.C ? 1 : 0);
		state->A = opA;
		state->FLAGS.bit.D = false;	//Binary mode
		Byte zpBase = rand();
		Byte index = rand();
		Byte zpTarget = zpBase + index;

		state->X = index;
		(*memory)[programSpace] = INS_ADC_INX.opcode;
		(*memory)[programSpace + 1] = zpBase;
		(*memory)[zpTarget] = dataSpace & 0xFF;
		(*memory)[zpTarget + 1] = dataSpace >> 8;
		(*memory)[dataSpace] = opB;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->A, (Byte)(opA + opB + flag));
		EXPECT_EQ(cycles, 6);
		state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
	}

	TEST_F(TestArithmeticInstruction, TestADCIndirectXRealDecimal) {
		// Given:
		Byte opA = 0x72, opB = 0x59;
		Byte flag = (state->FLAGS.bit.C ? 1 : 0);
		state->A = opA;
		state->FLAGS.bit.D = true;	//Binary mode
		Byte zpBase = rand();
		Byte index = rand();
		Byte zpTarget = zpBase + index;

		state->A = opA;
		state->X = index;
		(*memory)[programSpace] = INS_ADC_INX.opcode;
		(*memory)[programSpace + 1] = zpBase;
		(*memory)[zpTarget] = dataSpace & 0xFF;
		(*memory)[zpTarget + 1] = dataSpace >> 8;
		(*memory)[dataSpace] = opB;

		// When:
		u8 cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state->A, (Byte)(0x31 + flag));	// 72 + 59 + carry = 131+c = 0x31 + c
		EXPECT_EQ(cycles, 6);
		state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
	}
}

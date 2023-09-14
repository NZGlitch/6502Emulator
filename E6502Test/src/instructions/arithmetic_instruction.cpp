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

		// Helper for calculating the result of a decimal add
		Byte decimalAdd(Byte operandA, Byte operandB, bool carry) {
			Word al = (operandA & 0x0F) + (operandB & 0x0F) + (carry ? 1 : 0);	// Add LSD
			if (al > 0x09) al = ((al + 0x06) & 0x0F) + 0x10;		// if lsd between A and F, add 6 to LSD to get back in range (+6&$F), add carry to next digit (+0x10)
			al = (operandA & 0xF0) + (operandB & 0xF0) + al;		// Add MSD
			if (al > 0x99) al = al + 0x60;							// if msd between A and F, add 6 to MSD to get back in range (+$60)
			return (al & 0x00FF);									// Answer is lowets byte of AL
		}

		// Uses mocks to test correct function calls
		void testImmFlow(InstructionHandler instruction, bool addition = true) {
			// Given:
			Byte testValue = rand();
			(*memory)[programSpace] = instruction.opcode;

			// Then:
			EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(testValue));
			if (addition)
				EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);
			else
				EXPECT_CALL(*mockCPU, subAccumulator(_, testValue)).Times(1);
			// When:
			cpu->testExecute(1, mockCPU);
		}

		// Tests instruction actually produces correct result (Does not test flags, as this is handled in CPU tests)
		void testImmReal(InstructionHandler instruction, bool decimal, bool addition = true) {
			// Given:
			Byte opA = rand(), opB = rand();
			Byte flag = (state->FLAGS.bit.C ? 1 : 0);
			state->A = opA;
			state->FLAGS.bit.D = decimal;

			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = opB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			Byte expectResult = 0;
			if (addition)
				expectResult = decimal ? decimalAdd(opA, opB, flag) : (Byte)(opA + opB + flag);
			else
				expectResult = decimal ? decimalAdd(opA, (~opB), flag) : (Byte)(opA + (~opB) + flag);
			EXPECT_EQ(state->A, expectResult);
			EXPECT_EQ(cycles, 2);
			state->FLAGS = initPS;	// Not interested in testing flags here as this is covered in CPU tests for addAccumulator
		}

		// Uses mocks to test correct function calls
		void testAbsZPFlow(InstructionHandler instruction, bool isZeroPage = false, Byte* indexReg = nullptr) {
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

		// Tests instruction actually produces correct result (Does not test flags, as this is handled in CPU tests)
		void testAbsZPReal(InstructionHandler instruction, bool decimal, bool isZeroPage = false, bool crossBoundry = false, Byte* targetReg = nullptr) {
			state->FLAGS.bit.D = decimal;
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
			Byte expectResult = decimal ? decimalAdd(opA, opB, flag) : (Byte)(opA + opB + flag);
			EXPECT_EQ(state->A, expectResult);
			EXPECT_EQ(cycles, expectedCycles);
			state->FLAGS = initPS;		// Not interested in testing flags here as this is covered in CPU tests
		}

		// Uses mocks to test correct function calls
		void testIndirectXFlow(InstructionHandler instruction) {
			// Given:
			Byte testValue = rand();
			Byte zpBase = rand();
			Byte index = rand();
			Byte zpTarget = zpBase + index;

			state->X = index;
			(*memory)[programSpace] = instruction.opcode;

			// Then:
			EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(zpBase));
			EXPECT_CALL(*mockCPU, regValue(_, CPU::REGISTER_X)).Times(1).WillOnce(Return(index));
			EXPECT_CALL(*mockCPU, readWord(_, zpTarget)).Times(1).WillOnce(Return(dataSpace));
			EXPECT_CALL(*mockCPU, readReferenceByte(_, referencesAreEqual(CPU::REFERENCE_MEM, dataSpace))).Times(1).WillOnce(Return(testValue));
			EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

			// When:
			cpu->testExecute(1, mockCPU);
		}

		// Tests instruction actually produces correct result (Does not test flags, as this is handled in CPU tests)
		void testIndirectXReal(InstructionHandler instruction, bool decimal) {
			// Given:
			Byte opA = rand(), opB = rand();
			Byte flag = (state->FLAGS.bit.C ? 1 : 0);
			state->FLAGS.bit.D = decimal;
			Byte zpBase = rand();
			Byte index = rand();
			Byte zpTarget = zpBase + index;

			state->A = opA;
			state->X = index;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpBase;
			(*memory)[zpTarget] = dataSpace & 0xFF;
			(*memory)[zpTarget + 1] = dataSpace >> 8;
			(*memory)[dataSpace] = opB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			Byte expectResult = decimal ? decimalAdd(opA, opB, flag) : (Byte)(opA + opB + flag);
			EXPECT_EQ(state->A, expectResult);
			EXPECT_EQ(cycles, 6);
			state->FLAGS = initPS;
		}

		// Uses mocks to test correct function calls
		void testIndirectYFlow(InstructionHandler instruction) {
			// Given:
			Byte testValue = rand();
			Byte zpTarget = rand();
			Byte index = rand();
			Word opTarget = dataSpace + index;

			(*memory)[programSpace] = instruction.opcode;

			// Then:
			EXPECT_CALL(*mockCPU, readPCByte(_)).Times(1).WillOnce(Return(zpTarget));
			EXPECT_CALL(*mockCPU, readWord(_, zpTarget)).Times(1).WillOnce(Return(dataSpace));
			EXPECT_CALL(*mockCPU, regValue(_, CPU::REGISTER_Y)).Times(1).WillOnce(Return(index));
			EXPECT_CALL(*mockCPU, readReferenceByte(_, referencesAreEqual(CPU::REFERENCE_MEM, opTarget))).Times(1).WillOnce(Return(testValue));
			EXPECT_CALL(*mockCPU, addAccumulator(_, testValue)).Times(1);

			// When:
			cpu->testExecute(1, mockCPU);
		}

		// Tests instruction actually produces correct result (Does not test flags, as this is handled in CPU tests)
		void testIndirectYReal(InstructionHandler instruction, bool decimal, bool crossPage = false) {
			// Given:
			Byte index = rand();
			if (crossPage) {
				dataSpace |= 0x80;
				index = (rand() | 0x80);
			}
			else {
				dataSpace &= 0xFF00;
			}

			Byte opA = rand(), opB = rand();
			Byte flag = (state->FLAGS.bit.C ? 1 : 0);
			state->FLAGS.bit.D = decimal;
			Byte zpTarget = rand();


			state->A = opA;
			state->Y = index;
			(*memory)[programSpace] = instruction.opcode;
			(*memory)[programSpace + 1] = zpTarget;
			(*memory)[zpTarget] = dataSpace & 0xFF;
			(*memory)[zpTarget + 1] = dataSpace >> 8;
			(*memory)[dataSpace + index] = opB;

			// When:
			u8 cycles = cpu->execute(1);

			// Then:
			Byte expectResult = decimal ? decimalAdd(opA, opB, flag) : (Byte)(opA + opB + flag);
			EXPECT_EQ(state->A, expectResult);
			EXPECT_EQ(cycles, crossPage ? 6 : 5);
			state->FLAGS = initPS;
		}
	};

	/* Test defs & addHandlers func */
	TEST_F(TestArithmeticInstruction, TestArithmeticHandlers) {

		std::vector<InstructionMap> instructions = {
			// ADC Instructions
			{INS_ADC_IMM, 0x69}, {INS_ADC_ABS, 0x6D}, {INS_ADC_ABX, 0x7D}, {INS_ADC_ABY, 0x79},
			{INS_ADC_ZP0, 0x65}, {INS_ADC_ZPX, 0x75}, {INS_ADC_INX, 0x61}, {INS_ADC_INY, 0x71},

			// SBC Instructions
			{INS_SBC_IMM, 0xE9},

		};
		testInstructionDef(instructions, ArithmeticInstruction::addHandlers);
	}
}

	/***************** ADC TESTS *****************/
namespace E6502 {				// new scope allows for better readibility
	/** Immediate tests */
	TEST_F(TestArithmeticInstruction, TestADCImmediate) { testImmFlow(INS_ADC_IMM); }
	TEST_F(TestArithmeticInstruction, TestADCImmediateRealBinary) { testImmReal(INS_ADC_IMM, false); }
	TEST_F(TestArithmeticInstruction, TestADCImmediateRealDecimal) { testImmReal(INS_ADC_IMM, true); }

	// Absolute flow tests
	TEST_F(TestArithmeticInstruction, TestADCAbsolute) { testAbsZPFlow(INS_ADC_ABS); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteX) { testAbsZPFlow(INS_ADC_ABX, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteY) { testAbsZPFlow(INS_ADC_ABY, false, &state->Y); }

	// Absolute 'real' tests (Binary)
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteRealBinary) { testAbsZPReal(INS_ADC_ABS, false); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealBinary) { testAbsZPReal(INS_ADC_ABX, false, false, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealBinary) { testAbsZPReal(INS_ADC_ABY, false, false, false, &state->Y); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealCrossBinary) { testAbsZPReal(INS_ADC_ABX, false, false, true, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealCrossBinary) { testAbsZPReal(INS_ADC_ABY, false, false, true, &state->Y); }

	// Absolute 'real' tests (Decimal)
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteRealDecimal) { testAbsZPReal(INS_ADC_ABS, true); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealDecimal) { testAbsZPReal(INS_ADC_ABX, true, false, false, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealDecimal) { testAbsZPReal(INS_ADC_ABY, true, false, false, &state->Y); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteXRealCrossDecimal) { testAbsZPReal(INS_ADC_ABX, true, false, true, &state->X); }
	TEST_F(TestArithmeticInstruction, TestADCAbsoluteYRealCrossDecimal) { testAbsZPReal(INS_ADC_ABY, true, false, true, &state->Y); }

	// Zero Page flow tests
	TEST_F(TestArithmeticInstruction, TestADCZeroPage) { testAbsZPFlow(INS_ADC_ZP0, true); }
	TEST_F(TestArithmeticInstruction, TestADCZeroPageX) { testAbsZPFlow(INS_ADC_ZPX, true, &state->X); }

	// ZeroPAge 'real' tests (Binary)
	TEST_F(TestArithmeticInstruction, TestADCZeroPageRealBinary) { testAbsZPReal(INS_ADC_ZP0, false, true); }
	TEST_F(TestArithmeticInstruction, TestADCZeroPageXRealBinary) { testAbsZPReal(INS_ADC_ZPX, false, true, false, &state->X); }

	// ZeroPAge 'real' tests (Decimal)
	TEST_F(TestArithmeticInstruction, TestADCZeroPageRealDecimal) { testAbsZPReal(INS_ADC_ZP0, true, true); }
	TEST_F(TestArithmeticInstruction, TestADCZeroPageXRealDecimal) { testAbsZPReal(INS_ADC_ZPX, true, true, false, &state->X); }

	// X-Indexed Zero Page Indirect flow
	TEST_F(TestArithmeticInstruction, TestADCIndirectX) { testIndirectXFlow(INS_ADC_INX); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectXRealBinary) { testIndirectXReal(INS_ADC_INX, false); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectXRealDecimal) { testIndirectXReal(INS_ADC_INX, true); }

	// Zero Page Indirect Y-Indexed tests
	TEST_F(TestArithmeticInstruction, TestADCIndirectY) { testIndirectYFlow(INS_ADC_INY); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectYRealBinaryNoPage) { testIndirectYReal(INS_ADC_INY, false); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectYRealDecimalNoPage) { testIndirectYReal(INS_ADC_INY, true); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectYRealBinaryPage) { testIndirectYReal(INS_ADC_INY, false, true); }
	TEST_F(TestArithmeticInstruction, TestADCIndirectYRealDecimalPage) { testIndirectYReal(INS_ADC_INY, true, true); }
}

/***************** SBC TESTS *****************/
namespace E6502 {				// new scope allows for better readibility
	/** Immediate tests 
	TEST_F(TestArithmeticInstruction, TestSBCImmediate) { testImmFlow(INS_SBC_IMM, false); }
	TEST_F(TestArithmeticInstruction, TestSBCImmediateRealBinary) { testImmReal(INS_SBC_IMM, false, false); }
	TEST_F(TestArithmeticInstruction, TestSBCImmediateRealDecimal) { testImmReal(INS_SBC_IMM, true, false); }
	*/
}
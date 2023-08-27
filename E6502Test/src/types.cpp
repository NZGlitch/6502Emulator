#include <gmock/gmock.h>
#include "types.h"

/**
* Tests data types in types.h are correct - note this is is system dependent, so
* even if these test pass, compiling the code on a different system may lead
* to failure
*/
class TestTypes : public testing::Test {
public:
	virtual void SetUp() {
	}

	virtual void TearDown() {
	}
};

/* Test types are the correct size */
TEST_F(TestTypes, TestTypesCorrectLen) {
	EXPECT_EQ(sizeof(Byte), 1);
	EXPECT_EQ(sizeof(Word), 2);
	EXPECT_EQ(sizeof(u8), 1);
	EXPECT_EQ(sizeof(s8), 1);
	EXPECT_EQ(sizeof(u16), 2);
	EXPECT_EQ(sizeof(s16), 2);
}

/** Instruction Code from Byte */
TEST_F(TestTypes, TestInstructionCodeConsruct) {
	// Given:
	Byte testByte = 0b10101010;

	// When:
	InstructionCode code(testByte);

	// Then
	EXPECT_EQ(code.code, testByte);
	EXPECT_EQ(code.A, 0b101);
	EXPECT_EQ(code.B, 0b010);
	EXPECT_EQ(code.C, 0b10);
}

/** Instruction Code = Byte */
TEST_F(TestTypes, TestInstructionCodeToByte) {
	// Given:
	Byte initByte = 0b01010101;
	Byte testByte = 0b10101010;

	// When:
	InstructionCode code(initByte);
	code = testByte;

	// Then:
	EXPECT_EQ(code.A, 0b101);
	EXPECT_EQ(code.B, 0b010);
	EXPECT_EQ(code.C, 0b10);
}

/** Byte = InstructionCode */
TEST_F(TestTypes, TestByteToInstructionCode) {
	// Given:
	Byte testByte = 0b10101010;
	InstructionCode code(testByte);

	// When:
	Byte result = code;

	// Then:
	EXPECT_EQ(result, testByte);
}

/* Test unsigned types are indeed unsigned */
TEST_F(TestTypes, TestTypesUnsigned) {
	Byte testByte = -1;
	EXPECT_TRUE(testByte >= 0);

	Word testWord = -1;
	EXPECT_TRUE(testWord >= 0);

	u8 test8 = -1;
	EXPECT_TRUE(test8 >= 0);

	u16 test16 = -1;
	EXPECT_TRUE(test16 >= 0);
}

/* Test signed types are indeed signed */
TEST_F(TestTypes, TestTypesSigned) {
	s8 test8 = -1;
	EXPECT_TRUE(test8 < 0);

	s16 test16 = -1;
	EXPECT_TRUE(test16 < 0);
}

/* Test CPUState incPC gets and increments the PC */
TEST_F(TestTypes, TestCPUincPC) {
	// Given:
	CPUState state;
	Word testAddress = 0xABCD;	// TODO - randomise?
	state.PC = testAddress;

	// When:
	Word PC = state.incPC();

	// Then:
	EXPECT_EQ(PC, testAddress);				// Expect the return value to be what PC WAS)
	EXPECT_EQ(state.PC, testAddress+1);		// Expect PC to be incremented
}

/* Test CPUState pushSP gets and decrements the SP */
TEST_F(TestTypes, TestCPUpushSP) {
	// Given:
	CPUState state;
	Byte testAddress = 0x01;	// TODO - randomise?
	state.setSP(testAddress);
	ASSERT_EQ(state.getSP(), 0x0101);

	// When:
	Word sp01 = state.pushSP();
	Word sp00 = state.pushSP();
	Word spff = state.pushSP();

	// Then:
	EXPECT_EQ(sp01, 0x0101);				// First call should return 0x0101
	EXPECT_EQ(sp00, 0x0100);				// Next call should return 0x0100 (prev - 1)
	EXPECT_EQ(spff, 0x01FF);				// Last all should return 0x01FF (prev - 1, underflow wrap)
	EXPECT_EQ(state.getSP(), 0x01FE);		// Stack pointer should now be at 0x01F8
}

/* Test CPUState popSP increments then gets the SP */
TEST_F(TestTypes, TestCPUpopSP) {
	// Given:
	CPUState state;
	Byte testAddress = 0xFE;	// TODO - randomise?
	state.setSP(testAddress);
	ASSERT_EQ(state.getSP(), 0x01FE);

	// When:
	Word spff = state.popSP();
	Word sp00 = state.popSP();
	Word sp01 = state.popSP();

	// Then:
	
	EXPECT_EQ(spff, 0x01FF);				// First all should return 0x01FF (prev + 1)
	EXPECT_EQ(sp00, 0x0100);				// Next call should return 0x0100 (prev + 1 overflow/wrap)
	EXPECT_EQ(sp01, 0x0101);				// Last call should return 0x0101
	EXPECT_EQ(state.getSP(), 0x0101);		// Stack pointer should now be at 0x0101
}

/* Test CPUState setFlags/getFlags */
TEST_F(TestTypes, TestCPUGetSetFlags) {
	//Given:
	CPUState state;
	EXPECT_EQ(state.getFlags(), 0x00);
	Byte testFlags = 0xCF;

	//When:
	state.setFlags(testFlags);

	//Then:
	EXPECT_EQ(state.getFlags(), testFlags);
}

/* Test CPUState setSP/getSP */
TEST_F(TestTypes, TestCPUGetSP) {
	CPUState state;
	state.setSP(0x12);		//TODO - randomise

	EXPECT_EQ(state.getSP(), 0x0112);
}

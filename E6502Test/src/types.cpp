#include <gmock/gmock.h>
#include "types.h"

namespace E6502 {

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
	

	/******** Data types Tests ********/
	/* Test types are the correct size */
	TEST_F(TestTypes, TestTypesCorrectLen) {
		EXPECT_EQ(sizeof(Byte), 1);
		EXPECT_EQ(sizeof(Word), 2);
		EXPECT_EQ(sizeof(u8), 1);
		EXPECT_EQ(sizeof(s8), 1);
		EXPECT_EQ(sizeof(u16), 2);
		EXPECT_EQ(sizeof(s16), 2);
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
	
	/******** CPUState Tests ********
	/* Test CPUState incPC gets and increments the PC 
	TEST_F(TestTypes, TestCPUincPC) {
		// Given:
		CPUState state;
		Word testAddress = 0xABCD;	// TODO - randomise?
		state.PC = testAddress;

		// When:
		Word PC = state.incPC();

		// Then:
		EXPECT_EQ(PC, testAddress);				// Expect the return value to be what PC WAS)
		EXPECT_EQ(state.PC, testAddress + 1);		// Expect PC to be incremented
	}

	/* Test CPUState pushSP gets and decrements the SP 
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

	/* Test CPUState popSP increments then gets the SP 
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

	/* Test CPUState reset */
	TEST_F(TestTypes, TestCPUStateReset) {
		// Given:
		CPUState state;
		state.PC = 0x1142;
		state.SP = 0x42;

		state.A = 0x42;
		state.X = 0x42;
		state.Y = 0x42;

		state.setFlags(0xFF);

		// When:
		state.reset();

		// Then:
		EXPECT_EQ(state.PC, CPUState::DEFAULT_RESET_VECTOR);
		EXPECT_EQ(state.SP, CPUState::DEFAULT_SP);

		EXPECT_EQ(state.A, 0);
		EXPECT_EQ(state.X, 0);
		EXPECT_EQ(state.Y, 0);

		EXPECT_EQ(state.getFlags(), 0);
	}
}

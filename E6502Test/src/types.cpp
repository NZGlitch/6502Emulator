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
	
	/* Test CPUState reset */
	TEST_F(TestTypes, TestCPUStateReset) {
		// Given:
		CPUState state;
		state.PC = 0x1142;
		state.SP = 0x42;

		state.A = 0x42;
		state.X = 0x42;
		state.Y = 0x42;

		state.FLAGS.byte = 0xFF;

		// When:
		state.reset();

		// Then:
		EXPECT_EQ(state.PC, CPUState::DEFAULT_RESET_VECTOR);
		EXPECT_EQ(state.SP, CPUState::DEFAULT_SP);

		EXPECT_EQ(state.A, 0);
		EXPECT_EQ(state.X, 0);
		EXPECT_EQ(state.Y, 0);

		EXPECT_EQ(state.FLAGS.byte, 0x32);
	}
}

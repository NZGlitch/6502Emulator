#include <gmock/gmock.h>
#include "types.h"

/**
* Tests data types in types.h are correct - note this is isystem dependent, so
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
	ASSERT_EQ(sizeof(Byte), 1);
	ASSERT_EQ(sizeof(Word), 2);
	ASSERT_EQ(sizeof(u8), 1);
	ASSERT_EQ(sizeof(s8), 1);
	ASSERT_EQ(sizeof(u16), 2);
	ASSERT_EQ(sizeof(s16), 2);
}

/* Test unsigned types are indeed unsigned */
TEST_F(TestTypes, TestTypesUnsigned) {
	Byte testByte = -1;
	ASSERT_TRUE(testByte >= 0);

	Word testWord = -1;
	ASSERT_TRUE(testWord >= 0);

	u8 test8 = -1;
	ASSERT_TRUE(test8 >= 0);

	u16 test16 = -1;
	ASSERT_TRUE(test16 >= 0);
}

/* Test signed types are indeed signed */
TEST_F(TestTypes, TestTypesSigned) {
	s8 test8 = -1;
	ASSERT_TRUE(test8 < 0);

	s16 test16 = -1;
	ASSERT_TRUE(test16 < 0);
}

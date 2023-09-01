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

		/** Helper for setFlags test - note given state is reset before and after the test */
		void testFlags(Byte* targetReg, CPUState* state, Byte initFlags, Byte testValue, Byte expectFlags, char* test_name) {
			state->reset();

			// Given:
			state->setFlags(initFlags);
			
			// When:
			state->saveToRegAndFlag(targetReg, testValue);

			// Then:
			EXPECT_EQ(state->getFlags(), expectFlags);
			state->reset();
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
	
	/******** CPUState Tests ********/
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
		EXPECT_EQ(state.PC, testAddress + 1);		// Expect PC to be incremented
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

	/* Test CPUState reset */
	TEST_F(TestTypes, TestCPUStateReset) {
		// Given:
		CPUState state;
		state.PC = 0x1142;
		state.setSP(0x42);

		state.A = 0x42;
		state.X = 0x42;
		state.Y = 0x42;

		state.setFlags(0xFF);

		// When:
		state.reset();

		// Then:

		EXPECT_EQ(state.PC, 0);
		EXPECT_EQ(state.getSP(), 0x01FF);

		EXPECT_EQ(state.A, 0);
		EXPECT_EQ(state.X, 0);
		EXPECT_EQ(state.Y, 0);

		EXPECT_EQ(state.getFlags(), 0);
	}

	/* Test values are saved to the correct register */
	TEST_F(TestTypes, TestCPUSaveToReg) {
		CPUState *state = new CPUState;

		// Given:
		Byte regA = 0x21;
		Byte regX = 0x42;
		Byte regY = 0x84;

		// When:
		state->saveToRegAndFlag(&state->A, regA);
		state->saveToRegAndFlag(&state->X, regX);
		state->saveToRegAndFlag(&state->Y, regY);

		// Then:
		EXPECT_EQ(state->A, regA);
		EXPECT_EQ(state->X, regX);
		EXPECT_EQ(state->Y, regY);

		delete state;
	}
	
	/* Tests setFlags when N and Z flags 0 */
	TEST_F(TestTypes, TestRegisterSaveAndSetFlags00) {
		CPUState* state = new CPUState;
		// No Flags (unset exiting)
		testFlags(&state->A, state, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_A) NO ZN - change");
		testFlags(&state->X, state, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_X) NO ZN - change");
		testFlags(&state->Y, state, 0xFF, 0x78, 0x5D, "setFlags(REGISTER_Y) NO ZN - change");

		// No Flags (Unchange existing)
		testFlags(&state->A, state, 0x00, 0x78, 0x00, "setFlags(REGISTER_A) NO ZN - no change");
		testFlags(&state->X, state, 0x00, 0x78, 0x00, "setFlags(REGISTER_X) NO ZN - no change");
		testFlags(&state->Y, state, 0x00, 0x78, 0x00, "setFlags(REGISTER_Y) NO ZN - no change");
	}

	/* Tests setFlags when Z flag changes */
	TEST_F(TestTypes, TestRegisterSaveAndSetFlagsZ) {
		CPUState* state = new CPUState;
		// Z-Flag should be unset
		testFlags(&state->A, state, 0x02, 0x78, 0x00, "setFlags(REGISTER_A) unset Z");
		testFlags(&state->X, state, 0x02, 0x78, 0x00, "setFlags(REGISTER_X) unset Z");
		testFlags(&state->Y, state, 0x02, 0x78, 0x00, "setFlags(REGISTER_Y) unset Z");

		// Z-Flag sould be set
		testFlags(&state->A, state, 0x00, 0x00, 0x02, "setFlags(REGISTER_A) set Z");
		testFlags(&state->X, state, 0x00, 0x00, 0x02, "setFlags(REGISTER_X) set Z");
		testFlags(&state->Y, state, 0x00, 0x00, 0x02, "setFlags(REGISTER_Y) set Z");
	}

	/* Tests setFlags when N flag changes */
	TEST_F(TestTypes, TestRegisterSaveAndSetFlagsN) {
		CPUState* state = new CPUState;
		// N-Flag should be unset
		testFlags(&state->A, state, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_A) unset N");
		testFlags(&state->X, state, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_X) unset N");
		testFlags(&state->Y, state, 0xDD, 0x78, 0x5d, "setFlags(REGISTER_Y) unset N");

		// N-Flag sould be set
		testFlags(&state->A, state, 0x00, 0x80, 0x80, "setFlags(REGISTER_A) set N");
		testFlags(&state->X, state, 0x00, 0x80, 0x80, "setFlags(REGISTER_X) set N");
		testFlags(&state->Y, state, 0x00, 0x80, 0x80, "setFlags(REGISTER_Y) set N");
	}
}

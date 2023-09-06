#include "types.h"
#include "instruction_utils.h"

namespace E6502 {
	
	/**
	* Parent class for all instruction test classes
	*	- enforces flag handling
	*   - Configures CPU on SetUp and TearDown
	*	- Provides common helper functions
	*   - Provides a randomised program space in memory
	*   - Provides 2 other randomoised memory spaces
	* 
	* Memory Map:
	* 0x0000 -> 0x00FF		Zero Page
	* 0x0100 -> 0x01DD		Stack
	* 0x0200 -> 0x03FF		Unused
	* 0x0400 -> 0x0FFF		Program Space - PC and programSpace will be set to a random page here
	* 0x1000 -> 0x1FFF		Test Space 1 - addressSpace will be set to a random page here
	* 0x2000 -> 0x7FFF		Unused
	* 0x8000 -> 0xEFFF		Test Space 2 - dataSpace will be set to a random page here
	* 0xFF00 -> 0xFFFF		Reserved (0xFFFC is typically the reset vector)
	* 
	*/
	class TestInstruction : public testing::Test {
	public:
		
		// Access to CPU and internals
		CPUState* state;
		Memory* memory;
		CPUInternal* cpu;

		// Used to enforce flag checking on all tests
		FlagUnion initPS;

		// Memory spaces that can be used for testing - these are randomised every test
		Word programSpace = 0x0000;
		Word addressSpace = 0x0000;
		Word dataSpace = 0x0000;

		virtual void SetUp() {
			// Setup objects
			memory = new Memory();
			state = new CPUState;
			cpu = new CPUInternal(state, memory, &InstructionUtils::loader);
			cpu->reset();

			// Initialise flags
			state->FLAGS.byte = rand(); 
			initPS.byte = state->FLAGS.byte;

			// Initialise Memory
			programSpace = ((0x04 | rand()) & 0x0F) << 8;
			addressSpace = (0x10 | rand() & 0xF) << 8;
			dataSpace = (0x80 | (rand() & 0xFF)) << 8;

			// Set PC to programSpace
			state->PC = programSpace;

			// Set the stack pointer between 0x80 and 0xFF
			state->SP = 0x80 | rand();
		}

		virtual void TearDown() {
			EXPECT_EQ(state->FLAGS.byte, initPS.byte);
			delete memory;
			delete state;
			delete cpu;

			// Reset memory spaces
			programSpace = 0x0000;
			addressSpace = 0x0000;
			dataSpace = 0x0000;
		}

		/* Checks a status flags match testalue and Resets PS to initPS so the tear down test passes */
		void testAndResetStatusFlags(Byte testValue) {
			EXPECT_EQ(testValue == 0, state->FLAGS.bit.Z);
			EXPECT_EQ(testValue >> 7, state->FLAGS.bit.N);
			state->FLAGS.byte = initPS.byte;
		}

		/* Creates a test value (if not provided), ensures the target reg doesnt contain it and returns the testvalue */
		Byte genTestValAndClearTargetReg(Byte* targetReg) {
			Byte testValue = rand() & 0xFF;
			(*targetReg) = ~testValue;
			return testValue;
		}

		/* Generates a test value and ensures meomory location is clear */
		Byte genTestValAndTargetClearMem(Word address) {
			Byte testValue = rand();
			(*memory)[address] = ~testValue;
			return testValue;
		}
	};
}

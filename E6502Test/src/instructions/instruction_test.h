#include "types.h"
#include "instruction_utils.h"
#include <vector>

namespace E6502 {
	
	/* Used for checking instructionDefs */
	struct InstructionMap {
		InstructionHandler handler;
		Byte opcode;
	};

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

	/* Mock CPU is sometime useful */
	struct MockCPU : public CPUInternal {
		MockCPU() : CPUInternal(nullptr, nullptr, &InstructionUtils::loader) {}
		MOCK_METHOD(void, addAccumulator, (u8& cycles, Byte value));
		MOCK_METHOD(bool, getFlag, (u8& cycles, u8 flag));
		MOCK_METHOD(void, branch, (u8& cycles, s8 offset));
		MOCK_METHOD(Byte, readPCByte, (u8& cycles));
		MOCK_METHOD(Word, readPCWord, (u8& cycles));
		MOCK_METHOD(Byte, readReferenceByte, (u8& cycles, Reference& ref));
	};

	class TestInstruction : public testing::Test {
	public:
		
		// Access to CPU and internals
		CPUState* state = nullptr;
		Memory* memory = nullptr;
		CPUInternal* cpu = nullptr;
		
		// Mock CPU needed for various tests
		MockCPU* mockCPU = nullptr;

		// Used to enforce flag checking on all tests
		FlagUnion initPS = FlagUnion{};

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

			// Mock CPU if needed
			mockCPU = new MockCPU();

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
			delete mockCPU;

			// Reset memory spaces
			programSpace = 0x0000;
			addressSpace = 0x0000;
			dataSpace = 0x0000;
		}

		/* Instruction Defs & Handler helper - very repeated task so can justify a short helper method */
		void testInstructionDef(std::vector<InstructionMap> instructions, void(*addHandlers)(InstructionHandler* handlers[])) {
			// Given:
			InstructionHandler* handlers[0x100] = { nullptr };

			// When:
			addHandlers(handlers);

			// Then: for the each supplied instruction
			for (InstructionMap ins : instructions) {
				// Check that the handler opcode is correct
				EXPECT_EQ(ins.handler.opcode, ins.opcode) << ins.handler.name << " Opcode mismatch\n";

				// This assert will prevent program carsh if the memory is not correctly initialised
				ASSERT_NE(nullptr, handlers[ins.opcode]) << ins.handler.name << " Unable to find instruction at correct location in handler list";
				
				// Check the handler is put in the correct place
				EXPECT_EQ(*handlers[ins.opcode], ins.handler) << ins.handler.name << " Not added to handler list correclty\n";
			}
		}

		/* Checks a status flags match testValue and Resets PS to initPS so the tear down test passes
		* TODO - As we get more flag ops it seems likely this method needs to be removed or altered
		* ONLY checks N and Z flags.
		*/
		void testAndResetStatusFlags(Byte testValue) {
			//EXPECT_TRUE(false) << " Seed 68751 fails here";
			EXPECT_EQ(testValue == 0, state->FLAGS.bit.Z);
			EXPECT_EQ(testValue >> 7, state->FLAGS.bit.N);
			state->FLAGS.byte = initPS.byte;
		}

		/* Creates a test value, ensures the target reg does not already contain it and returns the testvalue */
		Byte genTestValAndClearTargetReg(Byte* targetReg) {
			Byte testValue = rand();
			(*targetReg) = ~testValue;
			return testValue;
		}

		/* Creates a test value, ensures the target reg contains it and returns the testvalue */
		Byte genTestValAndSetTargetReg(Byte* targetReg) {
			Byte testValue = rand();
			(*targetReg) = testValue;
			return testValue;
		}

		/* Generates a test value and ensures meomory location does not already contain it */
		Byte genTestValAndTargetClearMem(Word address) {
			Byte testValue = rand();
			(*memory)[address] = ~testValue;
			return testValue;
		}

		/* Generates a test value and places it in the given memory location */
		Byte genTestValAndSetMem(Word address) {
			Byte testValue = rand();
			(*memory)[address] = testValue;
			return testValue;
		}
	};
}

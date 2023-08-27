#include <gmock/gmock.h>
#include "types.h"
#include "cpu.h"
#include "jsr.h"
namespace E6502 {

	class TestJSRInstruction : public testing::Test {
	public:

		CPUState* state;
		Memory* memory;

		virtual void SetUp() {
			state = new CPUState;
			memory = new Memory;
		}

		virtual void TearDown() {
			delete state;
			delete memory;
		}
	};

	/************************************************
	*                Execution tests                *
	* Tests the execute function operates correctly *
	*************************************************/

	/* Test JSR execution */
	TEST_F(TestJSRInstruction, TestJSRAbsolute) {
		// Fixtures
		Byte lsb = 0x21;								//TODO - maybe randomise?
		Byte msb = 0x43;								//TODO - maybe randomise?
		Word startPC = 0x1234;							// PC Value we would before execution on (ignores actual JSR instruction)
		Word expectedPCafeterJump = (msb << 8) | lsb;	// PC Value after execution
		Byte initialSP = 0x42;							//TODO - maybe randomise?
		u8 cyclesUsed = 0;

		/**
		Before and after state of mem/cpu:

							   Memory							|	    CPU
			33		34		35		...		0x140	0x141	0x142	|	SP		PC
			JSR		0x21	0x43				?		?		?	|	0x42	0x1234

			Algorithm:
			Mem[SP--] = PC>>8
			MEM[SP--] = PC & 0xFF
			PC=[arg2<<8 | arg1] --> PC=[35<<8 | 34] --> PC=0x4321

								   Memory							|	    CPU
			33		34		35				0x140	0x141	0x142	|	SP		PC
			JSR		0x21	0x43			?		0x35	0x12	|	0x40	0x4321

		*/

		// Given: 
		state->setSP(initialSP);
		state->PC = startPC;
		memory->data[startPC] = lsb;
		memory->data[startPC + 1] = msb;

		// When:
		cyclesUsed = JSR::executeHandler(memory, state, &InstructionCode(INS_JSR));

		// Then:
		EXPECT_EQ(state->PC, (msb << 8) | lsb);						//The PC should be pointed at the target address
		EXPECT_EQ(memory->data[0x0100 | initialSP], 0x12);		// mem[0x0100 + stackInit] == msb(msbPC)		High order bits of original PC+2
		EXPECT_EQ(memory->data[0x0100 | initialSP - 1], 0x35);	// mem[0x0100 + stackInit - 1] == lsb(msbPC)	Low order bits of original PC+2
		EXPECT_EQ(state->getSP(), 0x0100 | (initialSP - 2));	// SP should decrement by 2
		EXPECT_EQ(cyclesUsed, 6);
	}

	/************************************************
	*              End Execution tests              *
	*************************************************

	/* Test correct OpCodes */
	TEST_F(TestJSRInstruction, TestInstructionDefs) {
		EXPECT_EQ(INS_JSR, 0x20);
	}

	/* Test addHandlers func adds JSR handler */
	TEST_F(TestJSRInstruction, TestJSRaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		JSR::addHandlers(handlers);

		// Then: For the JSR instruction, Expect *handlers[opcode] to point to a handler with the same opcode
		for (const Byte& opcode : JSR::instructions) {
			ASSERT_FALSE(handlers[opcode] == nullptr);
			EXPECT_EQ(((*handlers[opcode]).opcode), opcode);
		}
	}

	/*********************************************
	*				Handler Tests				 *
	* Check each handler is configured correctly *
	**********************************************/

	TEST_F(TestJSRInstruction, TestJSRAbsoluteHandlerProps) {
		InstructionHandler handler = JSR_ABS();
		EXPECT_EQ(handler.opcode, INS_JSR);
		EXPECT_TRUE(handler.isLegal);
		EXPECT_STREQ(handler.name, "JSR - Jump to Subroutine [Absolute]");
	}
	/*********************************************
	*			End of Handler Tests			 *
	**********************************************/
}

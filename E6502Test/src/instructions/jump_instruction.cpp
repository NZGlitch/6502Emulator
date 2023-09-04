#include <gmock/gmock.h>
#include "instruction_utils.h"
#include "jump_instruction.h"

namespace E6502 {

	class TestJSRInstruction : public testing::Test {
	public:

		CPUState state;
		Memory memory;
		InstructionUtils::Loader loader = InstructionUtils::loader;
		CPU* cpu;

		virtual void SetUp() {
			cpu = new CPU(&state, &memory, &loader);
		}

		virtual void TearDown() {
			delete cpu;
		}
	};

	/* Test correct OpCodes */
	TEST_F(TestJSRInstruction, TestInstructionDefs) {
		// JSR Opcode
		EXPECT_EQ(INS_JSR.opcode, 0x20);

		// JMP opcodes
		EXPECT_EQ(INS_JMP_ABS.opcode, 0x4C);
		EXPECT_EQ(INS_JMP_ABIN.opcode, 0x6C);

		// RTS opode
		EXPECT_EQ(INS_RTS.opcode, 0x60);
	}

	/* Test addHandlers func adds JSR handler */
	TEST_F(TestJSRInstruction, TestJSRaddHandlers) {
		// Given:
		InstructionHandler* handlers[0x100] = { nullptr };

		// When:
		Jump::addHandlers(handlers);

		// Then: For the JSR instruction, Expect *handlers[opcode] to point to a handler with the same opcode
		EXPECT_EQ(handlers[INS_JSR.opcode]->opcode, INS_JSR.opcode);

		// Then: For the JMP instruction, Expect *handlers[opcode] to point to a handler with the same opcode
		EXPECT_EQ(handlers[INS_JMP_ABS.opcode]->opcode, INS_JMP_ABS.opcode);
		EXPECT_EQ(handlers[INS_JMP_ABIN.opcode]->opcode, INS_JMP_ABIN.opcode);

		// Then: For the JSR instruction, Expect *handlers[opcode] to point to a handler with the same opcode
		EXPECT_EQ(handlers[INS_RTS.opcode]->opcode, INS_RTS.opcode);
	}

	/*******************************
	 ***     Execution tests     ***
	 *******************************/

	/* Test JSR execution */
	TEST_F(TestJSRInstruction, TestJSRAbsolute) {
		// Fixtures
		Byte lsb = 0x21;								//TODO - maybe randomise?
		Byte msb = 0x43;								//TODO - maybe randomise?
		Word startPC = 0x1234;							// PC Value we would before execution on (ignores actual JSR instruction)
		Word expectedPCafeterJump = (msb << 8) | lsb;	// PC Value after execution
		Byte initialSP = 0x42;							//TODO - maybe randomise?
		u8 cyclesUsed = 1;

		/**
		Before and after state of mem/cpu:

							   Memory							|	    CPU
			33		34		35		...		0x140	0x141	0x142	|	SP		PC
			JSR		0x21	0x43				?		?		?	|	0x42	0x1234

			Algorithm:
			Mem[SP--] = PC>>8
			MEM[SP--] = PC & 0xFF
			PC=[arg2<<8 | arg1] --> PC=[12<<8 | 34] --> PC=0x4321

								   Memory							|	    CPU
			33		34		35				0x140	0x141	0x142	|	SP		PC
			JSR		0x21	0x43			?		0x35	0x12	|	0x40	0x4321

		*/

		// Given: 
		state.SP = initialSP;
		state.PC = startPC;
		memory[startPC] = lsb;
		memory[startPC + 1] = msb;

		// When:
		Jump::jsrHandler(cpu, cyclesUsed, INS_JSR.opcode);

		// Then:
		EXPECT_EQ(state.PC, (msb << 8) | lsb);				//The PC should be pointed at the target address
		EXPECT_EQ(memory[0x0100 | initialSP], 0x35);		// mem[0x0100 + stackInit] == lsbPC		Low order bits of original PC+2
		EXPECT_EQ(memory[0x0100 | initialSP - 1], 0x12);	// mem[0x0100 + stackInit - 1] == msbPC	High order bits of original PC+2
		EXPECT_EQ(state.SP, (initialSP - 2));				// SP should decrement by 2
		EXPECT_EQ(cyclesUsed, 6);
	}

	/* Test JMP Absolute execution */
	TEST_F(TestJSRInstruction, TestJMPPAbsolute) {
		// Given:
		state.PC = 0x9A1C;
		memory[0x9A1C] = INS_JMP_ABS.opcode;
		memory[0x9A1D] = 0x42;
		memory[0x9A1E] = 0x81;
		Byte expectedCycles = 3;
		
		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state.PC, 0x8142);
		EXPECT_EQ(cycles, expectedCycles);
	}

	/* Test JMP Absolute Indirect execution */
	TEST_F(TestJSRInstruction, TestJMPAbsIndirect) {
		// Given:
		state.PC = 0x9A1C;
		memory[0x9A1C] = INS_JMP_ABIN.opcode;
		memory[0x9A1D] = 0x42;
		memory[0x9A1E] = 0x81;	// -> address 0x8142 points to the target address
		memory[0x8142] = 0x13;
		memory[0x8143] = 0xBF;	// -> target address of 0xBF13
		Byte expectedCycles = 5;

		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state.PC, 0xBF13);
		EXPECT_EQ(cycles, expectedCycles);
	}

	/* Test RTS Implied execution */
	TEST_F(TestJSRInstruction, TestRTSImplied) {
		// Given:
		state.PC = 0x5241;
		memory[0x5241] = INS_RTS.opcode;
		memory[0x1FF] = 0xAB;
		memory[0x1FE] = 0xCD;
		state.SP = 0x1FD;
		Byte expectedCycles = 6;

		// When:
		Byte cycles = cpu->execute(1);

		// Then:
		EXPECT_EQ(state.PC, 0xCDAC);		// Address on stack + 1
		EXPECT_EQ(state.SP, 0xFF);			// Stack incremented 2
		EXPECT_EQ(cycles, expectedCycles);
	}
}

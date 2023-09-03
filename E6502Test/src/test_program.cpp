#include <gmock/gmock.h>
#include "types.h"
#include "instructions/jsr.h"
#include "instructions/load_instruction.h"
#include "instructions/store_instruction.h"
#include "instructions/instruction_utils.h"

namespace E6502 {

	/** Tests the CPU by running an actual program on it */
	class TestProgram : public testing::Test {
	public:

		/** Address to load program to */
		Byte pamsb = 0x12;
		Byte palsb = 0x34;
		Word programAddress = (pamsb << 8) | palsb;

		// Where to write the data section
		Word dataSectionAddress = 0x4284;

		/** Loaded at #FFFC jumps to 'program' - MUST be 4 bytes long*/
		Byte initProgram[4] = { INS_JSR.opcode, palsb, pamsb, 0x00 };

		/* Size of program */
		const static Word programSize = 0x16;

		/* Sample program to test */
		Byte program[programSize] = {
			// Test out LDA instructions by loading data into the A register
			INS_LDX_IMM.opcode,		0x01,				// Set the X-Index register to 01
			INS_LDY_IMM.opcode,		0x02,				// Set the y-Index register to 02
			INS_LDA_ABS.opcode,		0x84,	0x42,		// Load the first byte from the data section (0x12)
			INS_LDA_ABSX.opcode,	0x84,	0x42,		// Load data[x] - A = 0x34
			INS_LDA_ABSY.opcode,	0x84,	0x42,		// Load data[y] - A = 0x56
			INS_LDA_IMM.opcode,		0x42,				// Load 0x42 into A
			INS_LDA_ZP.opcode,		0x02,				// load zp[02] = 0xDD into A
			INS_LDA_ZPX.opcode,		0x08,				// load (zp+0x08)[x] - A = 0x66
			//TODO - indirect instructions
			INS_JSR.opcode,			palsb,	pamsb,		// GOTO 10 - infinite loop
		};

		/* Size of data section */
		const static Word dataSize = 0x08;

		// Heap the progam can use for stuff
		Byte dataSection[dataSize] = {
			0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFF		//0x0000 -> 0x0007
		};

		/* Size of ZP section */
		const static Word zpSize = 0x10;

		// Heap the progam can use for stuff
		Byte zpSection[zpSize] = {
			0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA, 0x99, 0x88,		//0x00 -> 0x07
			0x77, 0x66, 0x55, 0x44, 0x33, 0x22, 0x11, 0x00,		//0x08 -> 0x0F
		};

		virtual void SetUp() {
		}

		virtual void TearDown() {
		}

	};

	/* Test the example program */
	TEST_F(TestProgram, TestProgram1) {
		// Initialise objects
		Memory* mem = new Memory();
		CPUState* state = new CPUState;
		InstructionLoader* loader = new InstructionUtils::Loader;
		CPU* cpu = new CPU(state, mem, loader);
		cpu->reset();
		u8 cyclesExecuted = 0;

		// Clock stuff
		s8 clockSpeedMhz = 1;		// 1 instruction per microsecond


		u8 insToExecute = 10;		// initProgram(1) + program(9) = 10

		// boot routine @ 0xFFFC
		mem->loadProgram(0xFFFC, initProgram, 4);

		// zero page @ 0x0000
		mem->loadProgram(0x0000, zpSection, zpSize);

		// program section 
		mem->loadProgram(programAddress, program, programSize);

		// data section
		mem->loadProgram(dataSectionAddress, dataSection, dataSize);

		auto start_time = std::chrono::high_resolution_clock::now();
		auto end_time = std::chrono::high_resolution_clock::now();
		auto time = end_time - start_time;

		while (insToExecute > 0) {
			// Execute One instruction
			u8 numExec = (insToExecute < 20 ? insToExecute : 50);	//20 at a time?
			cyclesExecuted = cpu->execute(numExec);

			// Calculate delay if required
			s8 timeToTake = cyclesExecuted * clockSpeedMhz;
			end_time = std::chrono::high_resolution_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();

			if (elapsed > timeToTake) {
				// Took too long
				// printf("%d cycles took %d microseconds to elapse - too long!\n", cyclesExecuted, elapsed);
			}
			else {
				// printf("%d cycles took %d microseconds to elapse - sleeping...\n", cyclesExecuted, elapsed);
				// Sleep 1us at a time until required time is taken
				while (elapsed < timeToTake) {
					std::this_thread::sleep_for(std::chrono::microseconds(1));
					end_time = std::chrono::high_resolution_clock::now();
					elapsed = (std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time)).count();
				}
			}

			// Reset clock & cycles
			insToExecute -= numExec;
			cyclesExecuted = 0;
			start_time = end_time;
		}

		EXPECT_EQ(state->PC, 0x1234);	// Last instruction (JSR) sets this
		EXPECT_EQ(state->A, 0x66);	// Last LDA instruction sets this
		EXPECT_EQ(state->X, 0x01);
		EXPECT_EQ(state->Y, 0x02);

		// Return address of stack should be programAddr + 0x11 (i.e. the last byte of the program)

		//TODO reinstate once popSP() has been reimplemented
		//Byte slsb = cpu->readByte(cyclesExecuted, state->popSP());
		//Byte smsb = cpu->readByte(cyclesExecuted, state->popSP());
		//Word stackAddr = (smsb << 8) | slsb;
		//EXPECT_EQ(stackAddr, programAddress + 0x15);

		delete cpu;
		delete loader;
		delete mem;
		delete state;
	}
}

#include <gmock/gmock.h>
#include "system.h"

namespace E6502 {

	/** Tests the CPU by running an actual program on it */
	class TestSystem : public testing::Test {
	public:

		virtual void SetUp() {
		}

		virtual void TearDown() {
		}

	};

	/* Test WIP Need at least Branch and inc sections before we can run the functional tests 
	TEST_F(TestSystem, TestSystem) {
		char* filename = "C:\\Users\\Chris\\source\\repos\\6502Emulator\\6502Emulator\\Assembly\\func_test.bin";
		//char* filename = "C:\\Users\\Chris\\source\\repos\\6502Emulator\\6502Emulator\\Assembly\\helloworld.bin";
		System test = System(filename);
		//printf("Breakpoint!");
		while (true)
			test.cpu->execute(1);
		//printf("Breakpoint!");

		// Print page until we hit a 0
		printf("\n");
		printf("Test Program Output:\n");
		printf("\n");
		Word idx = 0x1100;
		while ((*test.memory)[idx] != 0x00 && idx < 0x1200) {
			char c = (*test.memory)[idx++];
			printf("%c", c);
		}
		printf("\n");
		printf("\n");
	}
	*/
}


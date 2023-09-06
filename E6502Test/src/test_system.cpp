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

	/* Test WIP
	TEST_F(TestSystem, TestSystem) {
		char* filename = "C:\\Users\\Chris\\source\\repos\\6502Emulator\\6502Emulator\\Assembly\\test.prg";
		System test = System(filename);
		//printf("Breakpoint!");
		test.cpu->execute(test.program->size);
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


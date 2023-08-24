// Set to 1 to 'test' set to 0 to 'emulate'
#define TEST_OR_EMULATE 1
#if TEST_OR_EMULATE

//#include "gtest/gtest.h"
#include "gmock/gmock.h"
#if GTEST_OS_ESP8266 || GTEST_OS_ESP32
#if GTEST_OS_ESP8266
extern "C" {
#endif
	void setup() {
		testing::InitGoogleTest();
	}

	void loop() { RUN_ALL_TESTS(); }

#if GTEST_OS_ESP8266
}
#endif

#else

GTEST_API_ int main(int argc, char** argv) {
	printf("Running main() from %s\n", __FILE__);
	testing::InitGoogleTest(&argc, argv);
	testing::InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();
}
#endif

#else

#include "cpu.h"
int main() {
	Memory* mem = new Memory();
	CPU* cpu = new CPU();
	cpu->reset(mem);
	u8 cyclesExecuted = cpu->execute(1, mem);
	return 0;
}

#endif

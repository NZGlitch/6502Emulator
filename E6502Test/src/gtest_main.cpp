#include "gmock/gmock.h"
namespace E6502 {

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
		// If you need to set the seed for the test use --gtest_random_seed=SEED
		printf("Running main() from %s\n", __FILE__);
		testing::InitGoogleTest(&argc, argv);
		testing::InitGoogleMock(&argc, argv);
		return RUN_ALL_TESTS();
	}
#endif
}

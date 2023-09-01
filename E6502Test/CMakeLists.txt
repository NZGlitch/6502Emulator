cmake_minimum_required (VERSION 3.8)
project ( E6502Emu)

### GOOGLE TEST BEGIN ###
# Google Test Setup                                         #
# See https://google.github.io/googletest/quickstart-cmake.html #

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/9fce5480448488e17a50bcbf88d2f3bdb637ad6c.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
### GOOGLE TEST END ###

set (E6502_SOURCES
	"src/gtest_main.cpp"
	"src/types.cpp"
	"src/memory.cpp"
	"src/instruction_manager.cpp"
	"src/instruction_handler.cpp"
	"src/cpu.cpp"

	"src/instructions/instruction_utils.cpp"
	"src/instructions/jsr.cpp"
	"src/instructions/ldaxy.cpp"
	"src/instructions/store_instructions.cpp"

	"src/test_program.cpp"
	
	)

source_group("src" FILES ${E6502_SOURCES})

add_executable( E6502Test ${E6502_SOURCES})
add_dependencies( E6502Test E6502Lib)
add_dependencies( E6502Test E6502Instruction)

target_link_libraries(E6502Test E6502Lib)
target_link_libraries(E6502Test E6502Instruction)

enable_testing()
target_link_libraries( E6502Test gmock GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(E6502Test)

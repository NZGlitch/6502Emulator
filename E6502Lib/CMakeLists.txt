# CMakeList.txt : CMake project for 6502Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project( E6502Lib)
# Add source to this project's executable.
#add_executable (6502Emulator "6502Emulator.cpp" "6502Emulator.h")

# TODO: Add tests and install targets if needed.

set (E6502LIB_SOURCES
	"src/types.h"
	"src/instruction_handler.h"
	"src/memory.h"
	"src/cpu.h"
	"src/cpu.cpp"
	"src/instruction_manager.h"
	"src/instruction_manager.cpp"
	"src/system.h"
	"src/system.cpp"
	)

set (E6502LIB_INSTRUCTIONS
	"src/instructions/base.h"	
	"src/instructions/base.cpp"	
	"src/instructions/instruction_utils.h"	
	"src/instructions/load_instruction.h"
	"src/instructions/load_instruction.cpp"
	"src/instructions/jump_instruction.h"
	"src/instructions/jump_instruction.cpp"
	"src/instructions/stack_instruction.h"
	"src/instructions/stack_instruction.cpp"
	"src/instructions/store_instruction.h"
	"src/instructions/store_instruction.cpp"
	"src/instructions/transfer_instruction.h"
	"src/instructions/transfer_instruction.cpp"
)

source_group("src" FILES ${E6502LIB_SOURCES})
source_group("instructions" FILES ${E6502LIB_INSTRUCTIONS})

add_library( E6502Lib ${E6502LIB_SOURCES})
add_library( E6502Instruction ${E6502LIB_INSTRUCTIONS})

target_include_directories ( E6502Lib PUBLIC "${PROJECT_SOURCE_DIR}/src")
target_include_directories ( E6502Instruction PUBLIC "${PROJECT_SOURCE_DIR}/src/instructions")

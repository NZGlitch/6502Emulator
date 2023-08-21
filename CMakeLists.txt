# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("6502Emulator")

# Include sub-projects.
add_subdirectory ("E6502Lib")
add_subdirectory ("E6502Test")

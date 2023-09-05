#include "types.h"
#include "instructions/instruction_utils.h"

// Represents a computer system (e.g. C64), currently minimal needed to test instructions
namespace E6502 {
	class System {
	public:
		CPUInternal* cpu;
		CPUState* state;
		Memory* memory;
		InstructionLoader* loader;
		Program* program;

		//Read & Load a program into Memory, Update reset Vector, reset CPU
		System(char* executableFile);
		~System();
	};
}
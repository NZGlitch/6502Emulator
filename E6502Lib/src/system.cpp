#include "system.h"

// Represents a computer system (e.g. C64), currently minimal needed to test instructions
namespace E6502 {
	System::System(char* executableFile) {
		memory = new Memory;
		state = new CPUState;
		loader = &InstructionUtils::loader;
		cpu = new CPUInternal(state, memory, loader);
		cpu->reset();

		// Read and load program
		program = new Program;
		Byte buffer = -1;
		FILE* fp;
		fp = fopen(executableFile, "rb");  // r for read, b for binary
		fseek(fp, 0, SEEK_SET);

		// The first 2 Bytes give us the load address
		fread(&buffer, 1, 1, fp); program->loadAddress = buffer;
		fread(&buffer, 1, 1, fp); program->loadAddress |= buffer << 8;

		// Read the rest of the file into memory from loadAddress, keeping a count of the program size
		program->size = 0;
		Word currentAddress = program->loadAddress;
		while (fread(&buffer, 1, 1, fp) && currentAddress < CPUState::DEFAULT_RESET_VECTOR) {
			(*memory)[currentAddress++] = buffer;
			program->size++;
		}
		fclose(fp);

		// Copy the loaded bytes into program.Bytes
		for (Word i = program->loadAddress; i <= program->size; i++)
			program->bytes.push_back((*memory)[i]);
		
		// Update the reset vector to jump to the program (Must be 4 Bytes)
		(*memory)[CPUState::DEFAULT_RESET_VECTOR] = INS_JSR.opcode;
		(*memory)[CPUState::DEFAULT_RESET_VECTOR + 1] = program->loadAddress & 0xFF;
		(*memory)[CPUState::DEFAULT_RESET_VECTOR + 2] = program->loadAddress >> 8;
		(*memory)[CPUState::DEFAULT_RESET_VECTOR + 3] = INS_NOP.opcode;
	}

	System:: ~System() {
		delete program;
		delete state;
		delete memory;
	}
}

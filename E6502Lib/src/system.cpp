#include "system.h"

// uncomment to use TMPx style loading, otherwise assumes functests which loads at $400
//#define LOAD_MODE

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
		char buffer = -1;
		FILE* fp = NULL;
		if (fopen_s(&fp, executableFile, "rb")) {
			fprintf(stderr, "Unable to read file, abort!");
			return;
		}

#ifdef LOAD_MODE
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

#else
		program->loadAddress = 0x400;
		// Read the rest of the file into memory from loadAddress, keeping a count of the program size
		program->size = 0;

		Word currentAddress = 0x0000;
		while (fread(&buffer, 1, 1, fp)) {
			(*memory)[currentAddress++] = buffer;
			program->size++;
		}
		
		fclose(fp);

		// Copy the loaded bytes into program.Bytes
		for (Word i = 0; i <= program->size; i++)
			program->bytes.push_back((*memory)[i]);

#endif
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

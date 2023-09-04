#pragma once
#include "types.h"
#include <string>

namespace E6502 {

	// Forward declaration of CPU
	class CPU;
	
	/* A function that can handle execution of a single instruction */
	typedef void (*insHandlerFn)(CPU* cpu, u8& cycles, Byte opCode);

	struct InstructionHandler {
		Byte opcode;
		bool isLegal;
		const char* name;
		insHandlerFn execute;
	};

	inline bool operator==(const Byte& lhs, const InstructionHandler& rhs) {
		return lhs == rhs.opcode;
	}

	inline bool operator==(const InstructionHandler& lhs, const InstructionHandler& rhs) {
		return lhs.opcode == rhs.opcode && lhs.isLegal == rhs.isLegal && strcmp(lhs.name, rhs.name) == 0 && lhs.execute == rhs.execute;
	}

	struct InstructionLoader {
		virtual void load(InstructionHandler* handlers[]) {}
	};
}

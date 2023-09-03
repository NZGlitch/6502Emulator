#pragma once
#include "types.h"

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

	struct InstructionLoader {
		virtual void load(InstructionHandler* handlers[]) {}
	};
}

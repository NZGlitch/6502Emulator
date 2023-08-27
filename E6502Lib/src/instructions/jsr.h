#pragma once
#include "../types.h"
#include "base.h"

/** JSR Instruction Codes -> struct that handles it */
const static Byte INS_JSR = 0x20;

class JSR : public BaseInstruction {
public:
	static constexpr Byte instructions[] = { INS_JSR };

	/** One function will handle the 'execute' method for all variants */
	static const insHandlerFn JSRInstructionHandler;
	
	/** Actually handles execution of instructions */
	static u8 executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

	/** Called to add LDA Instruction handlers to the emulator */
	static void addHandlers(InstructionHandler* handlers[]);
};

/** Defines properties common to all JSR instructions */
struct JSR_BASE : public INSTRUCTION_BASE {
	JSR_BASE() {
		execute = JSR::executeHandler;
	}
};

/** Defines properties forJSR Absolute instruction */
struct JSR_ABS : public JSR_BASE {
	JSR_ABS() {
		opcode = INS_JSR;
		name = "JSR - Jump to Subroutine [Absolute]";
	}
};

#pragma once
#include "../instruction_handler.h"

/** LDA Instruction Opcodes */
const static Byte INS_LDA_IMM = 0xA9;
const static Byte INS_LDA_ZP = 0xA5;
const static Byte INS_LDA_ZPX = 0xB5;
const static Byte INS_LDA_ABS = 0xAD;
const static Byte INS_LDA_ABSX = 0xBD;
const static Byte INS_LDA_ABSY = 0xB9;
const static Byte INS_LDA_INDX = 0xA1;
const static Byte INS_LDA_INDY = 0xB1;

#ifndef LDA
#define LDA
namespace LDA {

	Byte instructions[] = { INS_LDA_IMM, INS_LDA_ZP, INS_LDA_ZPX,INS_LDA_ABS,
							INS_LDA_ABSX,INS_LDA_ABSY,INS_LDA_INDX,INS_LDA_INDY };

	/** One function will handle the 'execute' method for all variants */
	insHandlerFn LDAInstructionHandler = [](Memory* mem) { return 0;};

	/** Defines proprties common to all LDA instructions */
	struct BASE : public InstructionHandler {
		BASE() {
			isLegal = true;
			execute = LDAInstructionHandler;
		}
	};

	/** Defines properties for LDA Immediate instruction */
	struct LDA_IMM : public BASE {
		LDA_IMM() {
			opcode = INS_LDA_IMM;
			name = "LDA - Load Accumulator [Immediate]";
		}
	};

	/** Called to add LDA Instruction handlers to the emulator */
	static void addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_LDA_IMM] = ((InstructionHandler*) new LDA_IMM);
	}

}
#endif 


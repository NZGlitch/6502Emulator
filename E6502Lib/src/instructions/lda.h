#pragma once
#include "../types.h"

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
	insHandlerFn LDAInstructionHandler = [](Memory* mem, CPUState* state) { return 0;};

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

	/** Defines properties for LDA ZeroPage instruction */
	struct LDA_ZP : public BASE {
		LDA_ZP() {
			opcode = INS_LDA_ZP;
			name = "LDA - Load Accumulator [ZeroPage]";
		}
	};

	/** Defines properties for LDA ZeroPage(X) instruction */
	struct LDA_ZPX : public BASE {
		LDA_ZPX() {
			opcode = INS_LDA_ZPX;
			name = "LDA - Load Accumulator [ZeroPage-X]";
		}
	};

	/** Defines properties for LDA Absolute instruction */
	struct LDA_ABS : public BASE {
		LDA_ABS() {
			opcode = INS_LDA_ABS;
			name = "LDA - Load Accumulator [Absolute]";
		}
	};

	/** Defines properties for LDA Absolute(X) instruction */
	struct LDA_ABSX : public BASE {
		LDA_ABSX() {
			opcode = INS_LDA_ABSX;
			name = "LDA - Load Accumulator [Absolute-X]";
		}
	};

	/** Defines properties for LDA Absolute(Y) instruction */
	struct LDA_ABSY : public BASE {
		LDA_ABSY() {
			opcode = INS_LDA_ABSY;
			name = "LDA - Load Accumulator [Absolute-Y]";
		}
	};

	/** Defines properties for LDA Indirect(X) instruction */
	struct LDA_INDX : public BASE {
		LDA_INDX() {
			opcode = INS_LDA_INDX;
			name = "LDA - Load Accumulator [Indirect-X]";
		}
	};

	/** Defines properties for LDA Indirect(Y) instruction */
	struct LDA_INDY : public BASE {
		LDA_INDY() {
			opcode = INS_LDA_INDY;
			name = "LDA - Load Accumulator [Indirect-Y]";
		}
	};

	/** Called to add LDA Instruction handlers to the emulator */
	static void addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_LDA_IMM] = ((InstructionHandler*) new LDA_IMM);
		handlers[INS_LDA_ZP] = ((InstructionHandler*) new LDA_ZP);
		handlers[INS_LDA_ZPX] = ((InstructionHandler*) new LDA_ZPX);
		handlers[INS_LDA_ABS] = ((InstructionHandler*) new LDA_ABS);
		handlers[INS_LDA_ABSX] = ((InstructionHandler*) new LDA_ABSX);
		handlers[INS_LDA_ABSY] = ((InstructionHandler*) new LDA_ABSY);
		handlers[INS_LDA_INDX] = ((InstructionHandler*) new LDA_INDX);
		handlers[INS_LDA_INDY] = ((InstructionHandler*) new LDA_INDY);
		
	}
}

#endif 

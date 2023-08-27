#pragma once
#include "../types.h"
#include "base.h"

/** LDA Instruction Codes */
const static Byte INS_LDA_INDX	= 0xA1;
const static Byte INS_LDA_ZP	= 0xA5;
const static Byte INS_LDA_IMM	= 0xA9;
const static Byte INS_LDA_ABS	= 0xAD;
const static Byte INS_LDA_INDY	= 0xB1;
const static Byte INS_LDA_ZPX	= 0xB5;
const static Byte INS_LDA_ABSY	= 0xB9;
const static Byte INS_LDA_ABSX	= 0xBD;

class LDA : public BaseInstruction {
public:
	static constexpr Byte instructions[] = { INS_LDA_IMM, INS_LDA_ZP, INS_LDA_ZPX,INS_LDA_ABS,
							INS_LDA_ABSX,INS_LDA_ABSY,INS_LDA_INDX,INS_LDA_INDY };

	/** One function will handle the 'execute' method for all variants */
	static const insHandlerFn JSRInstructionHandler;

	/* Helper method to set the CPU flags. Only N(7) and Z(2) flags are affeted by LDA */
	static void setFlags(CPUState* state);

	/** Actually handles execution of instructions */
	static u8 executeHandler(Memory* mem, CPUState* state, InstructionCode* opCode);

	/** Called to add LDA Instruction handlers to the emulator */
	static void addHandlers(InstructionHandler* handlers[]);
};

/** Defines properties common to all LDA instructions */
struct LDA_BASE : public INSTRUCTION_BASE {
	LDA_BASE() {
		execute = LDA::executeHandler;
	}
};

/** Defines properties for LDA Immediate instruction */
struct LDA_IMM : public LDA_BASE {
	LDA_IMM() {
		opcode = INS_LDA_IMM;
		name = "LDA - Load Accumulator [Immediate]";
	}
};

/** Defines properties for LDA ZeroPage instruction */
struct LDA_ZP : public LDA_BASE {
	LDA_ZP() {
		opcode = INS_LDA_ZP;
		name = "LDA - Load Accumulator [ZeroPage]";
	}
};

/** Defines properties for LDA ZeroPage(X) instruction */
struct LDA_ZPX : public LDA_BASE {
	LDA_ZPX() {
		opcode = INS_LDA_ZPX;
		name = "LDA - Load Accumulator [ZeroPage-X]";
	}
};

/** Defines properties for LDA Absolute instruction */
struct LDA_ABS : public LDA_BASE {
	LDA_ABS() {
		opcode = INS_LDA_ABS;
		name = "LDA - Load Accumulator [Absolute]";
	}
};

/** Defines properties for LDA Absolute(X) instruction */
struct LDA_ABSX : public LDA_BASE {
	LDA_ABSX() {
		opcode = INS_LDA_ABSX;
		name = "LDA - Load Accumulator [Absolute-X]";
	}
};

/** Defines properties for LDA Absolute(Y) instruction */
struct LDA_ABSY : public LDA_BASE {
	LDA_ABSY() {
		opcode = INS_LDA_ABSY;
		name = "LDA - Load Accumulator [Absolute-Y]";
	}
};

/** Defines properties for LDA Indirect(X) instruction */
struct LDA_INDX : public LDA_BASE {
	LDA_INDX() {
		opcode = INS_LDA_INDX;
		name = "LDA - Load Accumulator [Indirect-X]";
	}
};

/** Defines properties for LDA Indirect(Y) instruction */
struct LDA_INDY : public LDA_BASE {
	LDA_INDY() {
		opcode = INS_LDA_INDY;
		name = "LDA - Load Accumulator [Indirect-Y]";
	}
};

#pragma once
#include <stdio.h>
#include "abstract_instruction.h"

/** LDA Instruction Opcodes */
const static Byte INS_LDA_IMM = 0xA9;
const static Byte INS_LDA_ZP = 0xA5;
const static Byte INS_LDA_ZPX = 0xB5;
const static Byte INS_LDA_ABS = 0xAD;
const static Byte INS_LDA_ABSX = 0xBD;
const static Byte INS_LDA_ABSY = 0xB9;
const static Byte INS_LDA_INDX = 0xA1;
const static Byte INS_LDA_INDY = 0xB1;

class LDA : public AbstractInstruction {

	void initialise(InstructionManager* insMan) override {
	}
};

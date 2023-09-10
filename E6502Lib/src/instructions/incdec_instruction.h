#pragma once
#include "base.h"

namespace E6502 {

	class IncDecInstruction : public BaseInstruction {
	public:

		// Operations mapped to op field value (Opfield is bits 7,6,5,1,0 of the OpCode). This doesn't work for implied operations!
		constexpr static Byte OP_INC = 0x1E;
		constexpr static Byte OP_DEC = 0x1A;
		
		static Byte INC(Byte v) { return v+1; }
		static Byte DEC(Byte v) { return v-1; }

		/** Handles execution of all increment/decrement instructions */
		static void incdecHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Logic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** DEC Mem By One */
	constexpr static InstructionHandler INS_DEC_ABS = { 0xCE, true, "DEC - Decrement Memory By One [Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ABX = { 0xDE, true, "DEC - Decrement Memory By One [X-Indexed Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ZP0 = { 0xC6, true, "DEC - Decrement Memory By One [Zero Page]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ZPX = { 0xD6, true, "DEC - Decrement Memory By One [X-Indexed Zero Page]", IncDecInstruction::incdecHandler };

	/** DEC Reg By One */
	constexpr static InstructionHandler INS_DEX_IMP = { 0xCA, true, "DEX - Decrement Index Register X By One [Implied]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEY_IMP = { 0x88, true, "DEY - Decrement Index Register Y By One [Implied]", IncDecInstruction::incdecHandler };
	
	/** INC Mem By One */
	constexpr static InstructionHandler INS_INC_ABS = { 0xEE, true, "INC - Increment Memory By One [Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ABX = { 0xFE, true, "INC - Increment Memory By One [X-Indexed Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ZP0 = { 0xE6, true, "INC - Increment Memory By One [Zero Page]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ZPX = { 0xF6, true, "INC - Increment Memory By One [X-Indexed Zero Page]", IncDecInstruction::incdecHandler };

	/** INC Reg By One */
	constexpr static InstructionHandler INS_INX_IMP = { 0xE8, true, "INX - Increment Index Register X By One [Implied]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INY_IMP = { 0xC8, true, "INY - Increment Index Register Y By One [Implied]", IncDecInstruction::incdecHandler };


	
	// Array of all Increment/Decrement instructions
	static constexpr InstructionHandler INCDEC_INSTRUCTIONS[] = {
		/** DEC Mem Instruction Definitions */
		INS_DEC_ABS, INS_DEC_ABX, INS_DEC_ZP0, INS_DEC_ZPX,

		/** DEC Reg Instruction Definitions */
		INS_DEX_IMP, INS_DEY_IMP, 

		/** INC Mem Instruction Definitions */
		INS_INC_ABS, INS_INC_ABX, INS_INC_ZP0, INS_INC_ZPX,

		/** INC Reg Instruction Definitions */
		INS_INX_IMP, INS_INY_IMP,
	};
}

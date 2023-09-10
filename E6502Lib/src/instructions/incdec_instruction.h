#pragma once
#include "base.h"

namespace E6502 {

	class IncDecInstruction : public BaseInstruction {
	public:

		// Operations mapped to op field value (Opfield is bits 7,6,5,1,0 of the OpCode)
		constexpr static Byte OP_INC = 0x1E;
		constexpr static Byte OP_DEC = 0x1A;
		
		static Byte INC(Byte v) { return v+1; }
		static Byte DEC(Byte v) { return v-1; }

		/** Handles execution of all logical instructions */
		static void incdecHandler(CPU* cpu, u8& cycles, Byte opCode);

		/** Called to add Logic Instruction handlers to the emulator */
		static void addHandlers(InstructionHandler* handlers[]);
	};

	/** Example: DEC Mem By One Field A: 110, Field C: 10*/
	constexpr static InstructionHandler INS_DEC_ABS = { 0xCE, true, "DEC - Decrement Memory By One [Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ABX = { 0xDE, true, "DEC - Decrement Memory By One [X-Indexed Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ZP0 = { 0xC6, true, "DEC - Decrement Memory By One [Zero Page]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_DEC_ZPX = { 0xD6, true, "DEC - Decrement Memory By One [X-Indexed Zero Page]", IncDecInstruction::incdecHandler };

	/** Example: INC Mem By One Field A: 111, Field C: 10 */
	constexpr static InstructionHandler INS_INC_ABS = { 0xEE, true, "INC - Increment Memory By One [Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ABX = { 0xFE, true, "INC - Increment Memory By One [X-Indexed Absolute]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ZP0 = { 0xE6, true, "INC - Increment Memory By One [Zero Page]", IncDecInstruction::incdecHandler };
	constexpr static InstructionHandler INS_INC_ZPX = { 0xF6, true, "INC - Increment Memory By One [X-Indexed Zero Page]", IncDecInstruction::incdecHandler };
	
	// Array of all logic instructions
	static constexpr InstructionHandler INCDEC_INSTRUCTIONS[] = {
		/** DEC Instruction Definitions */
		INS_DEC_ABS, INS_DEC_ABX, INS_DEC_ZP0, INS_DEC_ZPX,

		/** INC Instruction Definitions */
		INS_INC_ABS, INS_INC_ABX, INS_INC_ZP0, INS_INC_ZPX,
	};
}

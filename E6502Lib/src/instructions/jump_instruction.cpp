#include "jump_instruction.h"
#include <vector>

namespace E6502 {

	/**
		JSR writes PC-1 to the stack then jumps to the provided address
		pseudo code:
			addressLow = mem[PC++]
			mem[SP--] = PC >> 8
			mem[SP--] = PC & 0xFF
			addressHigh = mem[PC++]
			PC = targetAddress

		Actual cycles of a 6502:
			Read ADL; Increment PC
			Buffer ADL
			Push PCH; Decrement S
			Push PCL; Decrement S;
			Read ADH;
	*/
	void Jump::jsrHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Read ADL
		Word targetAddress = cpu->readPCByte(cycles);
		
		// Push the current program counter to the stack
		cpu->pushPCToStack(cycles);

		// Read ADH
		targetAddress |= (cpu->readPCByte(cycles) << 8);

		// Set PC
		cpu->setPC(cycles, targetAddress);
	};

	/** Implementation of addhandlers needs to be after the struct defs */
	void Jump::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : JUMP_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
			return;
		}
	}
}

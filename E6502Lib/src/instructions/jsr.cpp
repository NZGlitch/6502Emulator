#include "jsr.h"
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
	u8 JSR::jsrHandler(CPU* cpu, Byte opCode) {
		u8 cycles = 1;				// Retreiving the instruction takes 1 cycle

		// Read ADL
		Word targetAddress = cpu->readPCByte(cycles);
		
		// Push the current program counter to the stack
		cpu->pushPCToStack(cycles);

		// Read ADH
		targetAddress |= (cpu->readPCByte(cycles) << 8);

		// Set PC
		cpu->setPC(cycles, targetAddress);

		return cycles;
	};

	/** Implementation of addhandlers needs to be after the struct defs */
	void JSR::addHandlers(InstructionHandler* handlers[]) {
		handlers[INS_JSR.opcode] = &INS_JSR;
	}
}

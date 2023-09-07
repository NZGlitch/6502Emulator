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
	void JumpInstruction::jsrHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Read ADL
		Word targetAddress = cpu->readPCByte(cycles);
		
		// Push the current program counter to the stack
		Word pc = cpu->getPC(cycles);
		cpu->pushStackWord(cycles, pc);

		// Read ADH
		targetAddress |= (cpu->readPCByte(cycles) << 8);

		// Set PC
		cpu->setPC(cycles, targetAddress);

		// Cycle correction
		cycles--;
	};

	/* Handles JMP instructions */
	void JumpInstruction::jmpHandler(CPU* cpu, u8& cycles, Byte opCode) {
		// Note on JMP instrcutions:
		// An original 6502 has does not correctly fetch the target address if the indirect vector
		// falls on a page boundary(e.g.$xxFF where xx is any value from $00 to $FF).
		// In this case fetches the LSB from $xxFF as expected but takes the MSB from $xx00.
		// This is fixed in some later chips like the 65SC02 so for compatibility always ensure the
		// indirect vector is not at the end of the page.
		//
		// As it hasnt been decided exactly which model we will emulate, or if we will include bugs like this
		// We will just ignore for now

		Word targetAddress = cpu->readPCWord(cycles);

		if (opCode == INS_JMP_ABIN.opcode) {
			// Fir the indirect version we now read the word at target address as the actual target
			targetAddress = cpu->readWord(cycles, targetAddress);
		}

		cpu->setPC(cycles, targetAddress);
		
		// This instruction actually uses one less cycle as it doesnt need 'read'
		// the MSB of the target address, it can just copy it from mem to PC
		// cpu implementation doesnt currenty support this
		cycles--;
		
	}

	/* Handles RTS instructions */
	void JumpInstruction::rstHandler(CPU* cpu, u8& cycles, Byte opCode) {
		Word targetAddress = cpu->pullStackWord(cycles);
		targetAddress++; cycles++;
		cpu->setPC(cycles, targetAddress); 
		cycles++;
	}

	/** Implementation of addhandlers needs to be after the struct defs */
	void JumpInstruction::addHandlers(InstructionHandler* handlers[]) {
		for (InstructionHandler handler : JUMP_INSTRUCTIONS) {
			handlers[handler.opcode] = new InstructionHandler{
				handler.opcode, handler.isLegal, handler.name, handler.execute
			};
		}
	}
}

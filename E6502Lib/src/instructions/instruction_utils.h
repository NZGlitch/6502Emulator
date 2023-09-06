#pragma once
#include "jump_instruction.h"
#include "load_instruction.h"
#include "store_instruction.h"
#include "transfer_instruction.h"

namespace E6502 {
	/**
	* Common utilities for instruction definitions
	*/
	namespace InstructionUtils {
		struct Loader : public InstructionLoader {
			
			/** Adds all known insturctions to the given handler array */
			void load(InstructionHandler* handlers[]) override {
				JumpInstruction::addHandlers(handlers);
				LoadInstruction::addHandlers(handlers);
				StoreInstruction::addHandlers(handlers);
				TransferInstruction::addHandlers(handlers);
			}
		};

		static Loader loader;

		/** Function to get a pointer to a register in the state based on opcode */
		static u8 getRegFromInstruction(Byte instruction, CPU* cpu) {

			// Last 2 bits of opcode indicates target register
			switch (instruction & 0x03) {
				case 0x00: return CPU::REGISTER_Y;
				case 0x01: return CPU::REGISTER_A;
				case 0x02: return CPU::REGISTER_X;
				default: {
					// TODO error handling
					fprintf(stderr, "Invalid instruction provided to getRegFromInstruction: %x", instruction);
					return 0x255;
				}
			}
		}
	}
}

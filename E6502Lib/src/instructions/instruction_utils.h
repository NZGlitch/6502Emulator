#pragma once
#include "jsr.h"
#include "load_instruction.h"
#include "store_instruction.h"

namespace E6502 {
	/**
	* Common utilities for instruction definitions
	*/
	namespace InstructionUtils {
		struct Loader : public InstructionLoader {
			
			/** @override Adds all known insturctions to the given handler array */
			void load(InstructionHandler* handlers[]) override {
				JSR::addHandlers(handlers);
				LoadInstruction::addHandlers(handlers);
				StoreInstruction::addHandlers(handlers);
			}
		};

		static Loader loader;

		/** Function to get a pointer to a register in the state based on opcode */
		static Byte* getRegFromInstruction(Byte instruction, CPU* cpu) {

			// Last 2 bits of opcode indicates target register
			switch (instruction & 0x03) {
				case 0x00: return &cpu->currentState->Y;
				case 0x01: return &cpu->currentState->A;;
				case 0x02: return &cpu->currentState->X;
			}
			// TODO error handling
			return nullptr;
		}
	}
}

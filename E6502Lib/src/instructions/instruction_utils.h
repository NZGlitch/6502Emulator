#pragma once
#include "../types.h"
#include "jsr.h"
#include "load_instruction.h"
#include "store_instruction.h"

namespace E6502 {
	/**
	* Common utilities for instruction definitions
	*/
	namespace InstructionUtils {
		struct InstructionLoader {

			/** Adds all known insturctions to the given handler array */
			virtual void load(InstructionHandler* handlers[]) {
				JSR::addHandlers(handlers);
				LoadInstruction::addHandlers(handlers);
				StoreInstruction::addHandlers(handlers);
			}
		};

		static InstructionLoader loader;

		/** Function to get a pointer to a register in the state based on opcode */
		static Byte* getRegFromInstruction(Byte instruction, CPUState* state) {
			// Last 2 bits of opcode indicates target register
			switch (instruction & 0x03) {
				case 0x00: return &state->Y;
				case 0x01: return &state->A;;
				case 0x02: return &state->X;
			}
			// TODO error handling
			return nullptr;
		}
	}
}

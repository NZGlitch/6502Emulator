#pragma once
#include "../types.h"
#include "jsr.h"
#include "lda.h"

/**
* Common utilities for instruction definitions
*/
#ifndef INSTRUCTIONUTILS
#define INSTRUCTIONUTILS
namespace InstructionUtils {
	struct InstructionLoader {

		/** Adds all known insturctions to the given handler array */
		virtual void load(InstructionHandler* handlers[]) {
			JSR::addHandlers(handlers);		/* JSR Instructions */
			LDA::addHandlers(handlers);		/* LDA Instructions */
		}
	};

	static InstructionLoader loader;
}
#endif

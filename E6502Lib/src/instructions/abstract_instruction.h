#pragma once

#include "instruction_manager.h"

class AbstractInstruction {
public:
	virtual void initialise(InstructionManager* insMan) = 0;
};

#pragma once

#include "types.h"
#include "memory.h"
#include <functional>

typedef int(*insHandlerFn)(Memory*);

struct InstructionHandler {
	Byte opcode;
	bool isLegal;
	char* name;
	insHandlerFn execute;
};

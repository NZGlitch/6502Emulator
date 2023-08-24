#pragma once
#include "types.h"

// System Memory
struct Memory {
	static constexpr int MAX_MEM = 0x10000;	// Maximum addressable meory

	Byte data[MAX_MEM] = {};	//Actual data

	/* Reset memory to all 0's */
	virtual void initialise() {
		for (int i = 0; i < MAX_MEM; i++)
			data[i] = 0x00;
	}

	Byte readByte(u8& cycles, Word address) {
		cycles++;
		return data[address];
	}

	void writeByte(u8& cycles, Word address, Byte value) {
		data[address] = value;
		cycles++;
	}
};
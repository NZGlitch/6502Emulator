#pragma once
#include "types.h"

namespace E6502 {
	static constexpr int MAX_MEM = 0x10000;	// Maximum addressable meory

	// System Memory
	struct Memory {
	private:
		Byte data[MAX_MEM] = {};	//Actual data

	public:

		/* Reset memory to all 0's */
		virtual void reset() {
			for (int i = 0; i < MAX_MEM; i++)
				data[i] = 0x00;
		}

		/**
		* Load a program into memory at the given address.
		* Notes: memory will wrap around after 0xFFFF and no size check will be done on prgram array
		*/
		void loadProgram(Word address, Byte program[], u16 programSize) {
			for (u16 i = 0; i < programSize; i++) {
				Word nextAddr = address + i;
				data[nextAddr] = program[i];
			}
		}

		Byte& operator[](Word address) {
			return data[address];
		}

		const Byte& operator[](Word address) const {
			return data[address];
		}
	};
}

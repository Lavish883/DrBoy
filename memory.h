#ifndef MEMORY_CLASS
#define MEMORY_CLASS

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <iomanip>
#include "util.h"

#define KB_4 4096
#define KB_8 8192

class Memory {
	public:
		Memory(std::vector<uint8>& rom, std::vector<bit>& i_states) : rom_bank(rom), input_states(i_states) {
			last_addr_set = 0x0;
		}
		void init();
		void set(uint8 value, uint16 addr, bool from_instructions = true);
		uint8 read(uint16 addr) const;
		uint16 last_addr_set;

		bool dma_requested;
		int dma_cycles_left;
	private:
		std::vector<uint8>& rom_bank;
		std::vector<bit>& input_states;
		std::vector<uint8> wram;
		std::vector<uint8> vram;
		std::vector<uint8> oam;
		std::vector<uint8> external_ram;
		std::vector<uint8> io_registers_and_hram;

		void handle_dma_transfer(uint8 value);
		void handle_serial_transfer(uint8 value);
		uint8 handle_input_read() const;
};

#endif // !MEMORY_CLASS
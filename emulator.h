#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <map>
#include <queue>
#include <SFML/Graphics.hpp>

#include "util.h"
#include "cpu.h"
#include "ppu.h"
#include "memory.h"

class Emulator {
	private:
		std::vector<uint8> rom_bank;
		std::vector<bit> input_states;
	public:
		Memory memory;
		CPU cpu;
		PPU ppu;
		Emulator() : rom_bank(), input_states(8, 1), memory(rom_bank, input_states), cpu(memory), ppu(memory){

		};
		void init(std::string rom_name);
		unsigned int run(std::map<std::string, sf::RenderWindow*>& windows);
		void set_input(int button, bit set_to);
		// Private functions below
		void read_rom_into_vector(std::string rom_name);
};
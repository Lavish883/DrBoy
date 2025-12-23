#include "emulator.h"

void Emulator::init(std::string rom_name) {
	this->read_rom_into_vector(rom_name);
	//std::cout << "Cartidge type: " << this->rom_bank.at(0x147) << std::endl;
	this->memory.init();
	this->ppu.init();
}

unsigned int Emulator::run(std::map<std::string, sf::RenderWindow*>& windows) {
	const unsigned int t_cycles_took = this->cpu.execute();

	// update tiles if something touched in vram
	if (this->memory.last_addr_set >= 0x8000 && this->memory.last_addr_set <= 0x97FF) {
		uint16 tile_base = this->memory.last_addr_set & 0xFFF0;
		int tile_index = (tile_base - 0x8000) / 16;
		this->ppu.update_dirty_tiles(tile_index);
	}
	if (memory.last_addr_set == 0x9806) {
		std::cout << 'C' << std::endl;
		uint8 value = memory.read(0x9806);
		std::cout << value << std::endl;
		
	}
	// Update tile map
	if (memory.last_addr_set >= 0x9800 && this->memory.last_addr_set <= 0x9FFF) {
		ppu.update_dirty_bg_map(memory.last_addr_set - 0x9800);
	}
	this->ppu.execute(t_cycles_took);

	return t_cycles_took;
}

void Emulator::read_rom_into_vector(std::string rom_name) {
	std::ifstream file(rom_name, std::ios::binary | std::ios::ate);

	if (!file.is_open()) {
		std::cout << rom_name << std::endl;
		throw std::runtime_error("Unable to find a file with that name, File Name: " + rom_name);
	}

	std::streamsize file_size = file.tellg();
	file.seekg(0, std::ios::beg);

	this->rom_bank.resize(file_size);

	// Reads all the data into the rom_bank
	char* buffer = reinterpret_cast<char*>(this->rom_bank.data());
	file.read(buffer, file_size);

	if (!file.good()) {
		throw std::runtime_error("Rom wasn't able to loaded into the vector fully");
	}
}

void Emulator::set_input(int button, bit set_to) {
	input_states[button] = set_to;
}

//work_list.push(0x0);
	//work_list.push(0x08);
	//work_list.push(0x10);
	//work_list.push(0x18);
	//work_list.push(0x20);
	//work_list.push(0x28);
	//work_list.push(0x30);
	//work_list.push(0x38);
	//

/*
void Emulator::disassemble_rom_v2() {
	int i = 0;
	int byte_length;
	std::ostringstream name;

	while (i < rom_bank.size()) {
		if (rom_bank.at(i) == 0x00) {
			i += 1;
			continue;
		}
		name << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << i << ": ";
		byte_length = opcode_disassembler(rom_bank.at(i), name);
		
		if (byte_length > 1) { // 1,2,3
			name << " DATA: "; 
			for (int j = 1; j < byte_length; j++) {
				name << std::hex << std::setw(2) << std::setfill('0') << std::uppercase << rom_bank.at(i + j) << " ";
			}
		}
		
		disassembler_lines.push_back(name.str());
		i += byte_length;
		name.str("");
	}
}
void Emulator::disassemble_rom() {
	std::set<uint16> visited;
	std::queue<uint16> work_list;

	work_list.push(0x100);


	while (!work_list.empty()) {
		uint16 addr = work_list.front();
		work_list.pop();

		if (visited.count(addr)) continue;

		while (true) {
			if (visited.count(addr)) break;

			visited.insert(addr);

			if (addr >= rom_bank.size()) break;

			uint8_t opcode = rom_bank[addr];
			std::ostringstream name;
			name << std::hex << addr << ": ";

			uint8_t len = opcode_disassembler(opcode, name);

			// Now follow any logic paths
			std::string disassembled_name = name.str();

			disassembler_lines.push_back(disassembled_name);

			if (disassembled_name.find("JP") != std::string::npos) {
				uint16 target = (static_cast<uint16>(rom_bank.at(addr + 2)) << 8) + rom_bank.at(addr + 1);
				work_list.push(target);
				if (opcode == 0xC3) break; // No need to keep going if JP a16
				addr += len;
			}
			else if (disassembled_name.find("JR") != std::string::npos) {
				uint16 target = addr + static_cast<int8>(rom_bank.at(addr + 1));
				work_list.push(target);
				if (opcode == 0x18) break; // No need to keep going if JR e8
				addr += len;
			}
			else if (disassembled_name.find("CALL") != std::string::npos) {
				uint16 target = (static_cast<uint16>(rom_bank.at(addr + 2)) << 8) + rom_bank.at(addr + 1);
				work_list.push(target);
				addr += len;
			}
			else if (disassembled_name.find("RET") != std::string::npos) {
				if (opcode == 0xC9 || opcode == 0xD9) break;
				addr += len;
			}
			else {
				addr += len;
			}
		}
	}

	std::cout << "LENGTH: " << disassembler_lines.size() << std::endl;
}

*/
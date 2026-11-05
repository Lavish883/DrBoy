#include "memory.h"

void Memory::init() {
	this->wram.resize(KB_8);
	this->io_registers_and_hram.resize(256);
	this->vram.resize(KB_8);
	this->oam.resize(0xA0); // 160 bytes
	this->external_ram.resize(KB_8);
	this->set(0xCF, JOYP_ADDR, false);
	this->set(0x0, LY_ADDR, false);
	this->set(0x0, LYC_ADDR, false);
	this->set(0x85, STAT_ADDR, false);
	this->set(0x85, IF_ADDR, false);
	// Set io registers at boot
}

void Memory::handle_dma_transfer(uint8 value) { // 160 bytes copy
	dma_requested = true;
	dma_cycles_left = 640; // initiate the cycle halting for cpu
	
	uint16 src = value << 8;
	
	if (value <= 0x7F) {              // ROM
        std::copy_n(rom_bank.begin() + (src - 0x0000), 0xA0, oam.begin());
	} else if (value <= 0x9F) {         // VRAM
        std::copy_n(vram.begin() + (src - 0x8000), 0xA0, oam.begin());
	} else if (value <= 0xBF) {         // external RAM
        std::copy_n(external_ram.begin() + (src - 0xA000), 0xA0, oam.begin());
	} else if (value <= 0xDF) {         // WRAM
        std::copy_n(wram.begin() + (src - 0xC000), 0xA0, oam.begin());
	}

	for (int i = 0; i < 160; i++) {
		this->vram_writes.push_back(OAM_START_ADDR + i);
	}
}

uint8 Memory::handle_input_read() const {
	uint8_t joyp_value = io_registers_and_hram.at(JOYP_ADDR - 0xFF00);
	bit select_buttons_disabled = get_bit(joyp_value, 5);
	bit d_pad_buttons_disabled = get_bit(joyp_value, 4);

	if (select_buttons_disabled && d_pad_buttons_disabled) return 0xFF;

	if (!select_buttons_disabled) { // Active low checks if select button enabled
		joyp_value += input_states[A_BUTTON];
		joyp_value += input_states[B_BUTTON] << 1;
		joyp_value += input_states[SELECT] << 2;
		joyp_value += input_states[START] << 3;
	}

	if (!d_pad_buttons_disabled) {
		joyp_value += input_states[RIGHT];
		joyp_value += input_states[LEFT] << 1;
		joyp_value += input_states[UP] << 2;
		joyp_value += input_states[DOWN] << 3;
	}
	return joyp_value;
}

uint8 Memory::read(uint16 addr) const {
	// undocumented flag for the GB
	if (addr == 0xFF4D) return 0xFF; // https://www.reddit.com/r/EmuDev/comments/ipap0w/blarggs_cpu_tests_and_the_stop_instruction/
	
	if (addr >= 0x0 && addr <= 0x7FFF) {
		return rom_bank.at(addr);
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		return vram.at(addr - 0x8000);
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return external_ram.at(addr - 0xA000);
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		return wram.at(addr - 0xC000);
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		if (addr == JOYP_ADDR) return handle_input_read();
		return io_registers_and_hram.at(addr - 0xFF00);
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		return oam.at(addr - 0xFE00);
	} else if (addr >= 0xE00 && addr <= 0xFDFF){
		std::cout << "ECHO RAM READ WEIRD, but given read" << std::endl;
		
		return wram.at(addr - 0xE000);
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		return 0x0;
	}
	//throw std::runtime_error("THIS PART OF MEMORY NOT IMPLEMENTED");
	std::cout << "THIS PART OF MEMORY NOT IMPLEMENTED: Mem addr = " << addr << std::endl;
	return 0x0;
}

void Memory::set(uint8 value, uint16 addr, bool from_instructions) {
	last_addr_set = addr;

	if (addr >= 0x0 && addr <= 0x7FFF) {
		std::cout << "Invalid write ignoring at: " << addr << std::endl;
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		vram_writes.push_back(addr);
		vram.at(addr - 0x8000) = value;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		external_ram.at(addr - 0xA000) = value;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		wram.at(addr - 0xC000) = value;
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		if (addr == OAM_DMA_ADDR) {
			handle_dma_transfer(value);
		}
		if (addr == JOYP_ADDR && from_instructions){
			value &= 0x30;
		} 
		if (addr == 0xFF02) handle_serial_transfer(value);
		if (addr == 0xFF04 && from_instructions) value = 0;
		io_registers_and_hram.at(addr - 0xFF00) = value;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		vram_writes.push_back(addr); // IK NOT PART OF VRAM BUT HOW DATA IS UPDATED IN PPU
		oam.at(addr - 0xFE00) = value;
	} else if (addr >= 0xE00 && addr <= 0xFDFF){
		std::cout << "ECHO RAM" << std::endl;
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {

	}	
	else {
		std::cout << "Not implnemented yet mem addr " << addr;
	}
}

void Memory::handle_serial_transfer(uint8 value) {
	if (value == 0x81) {
		char out = read(0xFF01);
		std::cout << out;
		set(0x0, 0xFF02); // Mark transfer done
	}
}
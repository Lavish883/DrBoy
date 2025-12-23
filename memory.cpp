#include "memory.h"

void Memory::init() {
	this->wram.resize(KB_8);
	this->io_registers_and_hram.resize(256);
	this->vram.resize(KB_8);
	this->oam.resize(0xA0); // 160 bytes
	this->external_ram.resize(KB_8);
	this->set(0xFF, JOYP_ADDR, false);
}

void Memory::handle_dma_transfer(uint8 value) { // 160 bytes copy
	dma_requested = true;
	dma_cycles_left = 640; // initiate the cycle halting for cpu
	if (value >= 0x00 && value <= 0x7F) { // Copy from rom
		std::copy(rom_bank.begin(), rom_bank.begin() + 0xA0, oam.begin());
	}
	if (value >= 0x80 && value <= 0x9F) { // Copy from vram
		std::copy(vram.begin(), vram.begin() + 0xA0, oam.begin());
	}
	if (value >= 0xA0 && value <= 0xBF) { // Copy from external rram
		std::copy(external_ram.begin(), external_ram.begin() + 0xA0, oam.begin());
	}
	if (value >= 0x80 && value <= 0x9F) { // Copy from wram
		std::copy(wram.begin(), wram.begin() + 0xA0, oam.begin());
	}
}

uint8 Memory::handle_input_read() const {
	uint8_t joyp_value = io_registers_and_hram.at(JOYP_ADDR - 0xFF00);
	bit select_buttons_disabled = get_bit(joyp_value, 5);
	bit d_pad_buttons_disabled = get_bit(joyp_value, 4);

	if (select_buttons_disabled && d_pad_buttons_disabled) return 0x3F;

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
	}
	else if (addr >= 0xFEA0 && addr <= 0xFEFF) {
		return 0x0;
	}
	//throw std::runtime_error("THIS PART OF MEMORY NOT IMPLEMENTED");
	std::cout << "THIS PART OF MEMORY NOT IMPLEMENTED" << std::endl;
	return 0x0;
}

void Memory::set(uint8 value, uint16 addr, bool from_instructions) {
	last_addr_set = addr;
	if (addr >= 0x0 && addr <= 0x7FFF) {
		std::cout << "Invalid write ignoring at: " << addr << std::endl;
	}
	else if (addr >= 0x8000 && addr <= 0x9FFF) {
		vram.at(addr - 0x8000) = value;
	}
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		external_ram.at(addr - 0xA000) = value;
	}
	else if (addr >= 0xC000 && addr <= 0xDFFF) {
		wram.at(addr - 0xC000) = value;
	}
	else if (addr >= 0xFF00 && addr <= 0xFFFF) {
		if (addr == OAM_DMA_ADDR) return handle_dma_transfer(value);
		if (addr == JOYP_ADDR && from_instructions){
			value &= 0x30;
		} 
		if (addr == 0xFF02) handle_serial_transfer(value);
		if (addr == 0xFF04 && from_instructions) value = 0;
		io_registers_and_hram.at(addr - 0xFF00) = value;
	}
	else if (addr >= 0xFE00 && addr <= 0xFE9F) {
		oam.at(addr - 0xFE00) = value;
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
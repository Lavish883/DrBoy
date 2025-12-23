#include "cpu.h"
#include "instructions.h"

uint16_t interrupt_handler_addrs[5] = { 0x0040, 0x0048, 0x0050, 0x0058, 0x0060 };

unsigned int CPU::execute() {
	//log_to_file();
	handle_clock();
	handle_interrupts();

	if (IME_pending) {
		IME = true;
		IME_pending = false;
	}

	int t_cycles_instr_took = 0;

	if (!in_low_power_mode && !mem.dma_requested) {
		int t_cycles_before = this->t_cycles_count;
		uint8 opcode = mem.read(regPC++);
		INSTRUCTIONS::execute_instruction(*this, opcode);
		
		t_cycles_instr_took = this->t_cycles_count - t_cycles_before;
		tima_cycles_count += t_cycles_instr_took;
		div_cycles_count += t_cycles_instr_took;
	}

	if (mem.dma_requested && mem.dma_cycles_left > 0) {
		mem.dma_cycles_left -= 4;
		t_cycles_instr_took += 4;
		if (mem.dma_cycles_left == 0) mem.dma_requested = false;
	}

	return t_cycles_instr_took;
}

uint8 CPU::get_register(char reg_name) {
	switch (reg_name){
		case('A'): return regA;
		case('B'): return regB;
		case('C'): return regC;
		case('D'): return regD;
		case('E'): return regE;
		case('F'): return regF;
		case('H'): return regH;
		case('L'): return regL;
		case(r_at_HL): return mem.read(get_register("HL"));
		default:
			break;
	}
	throw std::runtime_error("Register is not valid");
}

uint16 CPU::get_register(std::string reg_name) {
	if (reg_name.size() != 2) {
		throw std::runtime_error("Reg name is not valid " + reg_name);
	}
	if (reg_name == "SP") return regSP;
	if (reg_name == "PC") return regPC;

	uint8 high_byte = get_register(reg_name.at(0));
	uint8 low_byte = get_register(reg_name.at(1));

	return (static_cast<uint16>(high_byte) << 8) + low_byte;
}

void CPU::set_register(char reg_name, uint8 value) {
	switch (reg_name){
		case('A'): { regA = value; return; }
		case('B'): { regB = value; return; }
		case('C'): { regC = value; return; }
		case('D'): { regD = value; return; }
		case('E'): { regE = value; return; }
		case('F'): { regF = value; return; }
		case('H'): { regH = value; return; }
		case('L'): { regL = value; return; }
		case(r_at_HL): { mem.set(value, get_register("HL")); return; }
		default:
			break;
	}
	throw std::runtime_error("Register is not valid");
}

void CPU::set_register(std::string reg_name, uint16 value) {
	if (reg_name.size() != 2) {
		throw std::runtime_error("Reg name is not valid " + reg_name);
	}
	if (reg_name == "SP") {
		regSP = value;
		return;
	}
	if (reg_name == "PC") {
		regPC = value;
		return;
	}

	uint8 high_byte = static_cast<uint8>(value >> 8);
	uint8 low_byte = static_cast<uint8>(value);

	set_register(reg_name.at(0), high_byte);
	set_register(reg_name.at(1), low_byte);
}

bit CPU::get_flag(char flag){
	flag = toupper(flag);
	switch (flag){
		case('Z'): return get_bit(regF, 7);
		case('N'): return get_bit(regF, 6);
		case('H'): return get_bit(regF, 5);
		case('C'): return get_bit(regF, 4);
		default: break;
	}
	throw std::runtime_error("Invalid flag was passed");
}

void CPU::set_flag(char flag, bit value) {
	flag = toupper(flag);
	switch (flag) {
		case('Z'): return set_bit(regF, 7, value);
		case('N'): return set_bit(regF, 6, value);
		case('H'): return set_bit(regF, 5, value);
		case('C'): return set_bit(regF, 4, value);
		default: break;
	}
	throw std::runtime_error("Invalid flag was passed");
}

void CPU::set_IME(bool value){
	IME = value;
}

uint8 CPU::read_immediate8_mem() {
	return mem.read(regPC++);
}

void CPU::set_immediate8_mem(uint8 value) {
	mem.set(value, regPC + 1);
}

uint16 CPU::read_immediate16_mem() {
	uint8 low_byte = mem.read(regPC++);
	uint8 high_byte = mem.read(regPC++);

	return ((static_cast<uint16>(high_byte)) << 8) + low_byte;
}
// HALT bug might not be supported?
void CPU::handle_clock() {
	if (div_cycles_count >= 256) {
		// incremnent DIV
		mem.set(mem.read(0xFF04) + 1, 0xFF04, false);
		div_cycles_count -= 256;
	}
	// Check TAC to see if clock is even enabled
	uint8 TAC = mem.read(0xFF07);

	if (get_bit(TAC, 2) == 1) { // MEANS TAC IS ENABLED
		// decided how many cycles it takes to increment TIMA
		int clock_select = read_bits_of_uint(TAC, 1, 0); 
		unsigned int number_of_t_cycles = 0;

		switch (clock_select) {
			case 0: number_of_t_cycles = 1024; break;
			case 1: number_of_t_cycles = 16; break;
			case 2: number_of_t_cycles = 64; break;
			case 3: number_of_t_cycles = 256; break;
			default: {
				throw std::runtime_error("Something went wrong with the TAC");
			}
		}

		if (tima_cycles_count >= number_of_t_cycles) {
			// Increment TIMA
			uint16 TIMA = static_cast<uint16>(mem.read(0xFF05));
			uint8 TMA = mem.read(0xFF06);
			// Interuppt flag
			uint8 IF = mem.read(0xFF0F); 


			// Check if incrementing will make it overflow
			if (TIMA + 1 > 0xFF) {
				mem.set(TMA, 0xFF05); // sets value to TMA
				// request an interrupt
				set_bit(IF, 2, 1);
				mem.set(IF, 0xFF0F);
			}
			else {
				mem.set(static_cast<uint8>((TIMA + 1)), 0xFF05);
			}
			tima_cycles_count -= number_of_t_cycles;
		}
	}
}

void CPU::handle_interrupts() {
	// Get IE (Interrupt enable) and IF (Interrupt flag)
	uint8 IE = mem.read(0xFFFF);
	uint8 IF = mem.read(0xFF0F);

	if ((IE & IF) != 0) { // WAKES UP GB
		this->in_low_power_mode = false;
	}
	if (this->IME == false) return;

	bit IE_BITS[5] = { get_bit(IE, 0), get_bit(IE, 1), get_bit(IE, 2), get_bit(IE, 3), get_bit(IE, 4) };
	bit IF_BITS[5] = { get_bit(IF, 0), get_bit(IF, 1), get_bit(IF, 2), get_bit(IF, 3), get_bit(IF, 4) };

	// Go through IE_bits and see if they are enabled then check IF from 0,1,2,3,4 as highest priotiy in that order
	for (int i = 0; i < 5; i++) {
		if (IE_BITS[i] && IF_BITS[i]) {
			// Handle the interrupt it takes 20 cycles
			this->IME = false;
			// SET bit correpsonding to this intruppt in IF to 0 in memory
			set_bit(IF, i, 0);
			mem.set(IF, 0xFF0F);
			// Now Handle actual interuppt
			mem.set(static_cast<uint8>(regPC >> 8), --regSP);
			mem.set(static_cast<uint8>(regPC), --regSP);
			regPC = interrupt_handler_addrs[i];
			
			t_cycles_count += 20;
			break;
		}
	}

	div_cycles_count += 20;
	tima_cycles_count += 20;
}

void CPU::log_to_file() {
	log_file << "A:" << regA << " ";
	log_file << "F:" << regF << " ";
	log_file << "B:" << regB << " ";
	log_file << "C:" << regC << " ";
	log_file << "D:" << regD << " ";
	log_file << "E:" << regE << " ";
	log_file << "H:" << regH << " ";
	log_file << "L:" << regL << " ";
	log_file << "SP:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << regSP << " ";
	log_file << "PC:" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << regPC << " ";
	log_file << "PCMEM:" << mem.read(regPC) << ',' << mem.read(regPC + 1) << ',' << mem.read(regPC + 2) << ','  << mem.read(regPC + 3);
	log_file << std::endl;
}
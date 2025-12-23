#include "instructions.h"

// Jumps to the value in immediate 16
void INSTRUCTIONS::uncond_jump_n16(CPU& cpu) {
	cpu.regPC = cpu.read_immediate16_mem();
	cpu.t_cycles_count += 16;
}
// If condition is true it jumps to the value in immediate 16
void INSTRUCTIONS::cond_jump_n16(CPU& cpu, std::string cond_name) {
	bool cond = check_cond(cpu, cond_name);
	if (cond) {
		uncond_jump_n16(cpu); // Will handle the cycles
	} else { // Data read either way
		cpu.read_immediate16_mem();
		cpu.t_cycles_count += 12;
	}
}
// Makees an signed realtive jump, means reg.PC = regPC + signed immediate 8 bit value
void INSTRUCTIONS::uncond_jump_e8(CPU& cpu){
	int8 jump_by = static_cast<int8>(cpu.read_immediate8_mem());
	cpu.regPC += jump_by;
	cpu.t_cycles_count += 12;
}
// If condition is true makees an signed realtive jump, means reg.PC = regPC + signed immediate 8 bit value
void INSTRUCTIONS::cond_jump_e8(CPU& cpu, std::string cond_name){
	bool cond = check_cond(cpu, cond_name);
	if (cond) {
		uncond_jump_e8(cpu); // handles cycles by itself
	} else {
		cpu.read_immediate8_mem(); // read it either way
		cpu.t_cycles_count += 8;
	}
}
// First pushes the regPC to the stack and then jumps to immediate16
void INSTRUCTIONS::uncond_call(CPU& cpu){
	// Pushes PC to the stack High byte first then lower byte as little endian
	uint16 value = cpu.regPC + 2;
	cpu.mem.set(static_cast<uint8>(value >> 8), --cpu.regSP);
	cpu.mem.set(static_cast<uint8>(value), --cpu.regSP);

	cpu.t_cycles_count += 8;
	// NOW same as JUMP immedaite 16 handles rest of cycles
	uncond_jump_n16(cpu);
}
// Same as uncond_call ecept it only calls if some condtiion is true
void INSTRUCTIONS::cond_call(CPU& cpu, std::string cond_name){
	bool cond = check_cond(cpu, cond_name);
	if (cond) {
		uncond_call(cpu); // handles its cycles
	} else {
		cpu.read_immediate16_mem(); // read it either way
		cpu.t_cycles_count += 12;
	}
}
// Just pops regPC from the stack
void INSTRUCTIONS::uncond_return(CPU& cpu){
	pop_from_stack(cpu, "PC"); // Handles its cycles
}
// Just pops regPC from the stack if the cond is true
void INSTRUCTIONS::cond_return(CPU& cpu, std::string cond_name){
	bool cond = check_cond(cpu, cond_name);
	if (cond) {
		uncond_return(cpu); // handles its cycles
		cpu.t_cycles_count += 4; // cond check cycles
	} else {
		cpu.t_cycles_count += 8; // 4 for opcode and 4 for cond
	}
}
// Pushes the 16 bit register that is given to the statkc
void INSTRUCTIONS::push_on_stack(CPU& cpu, std::string reg_name){
	uint16 reg_value = cpu.get_register(reg_name);
	// Push the reg_value on stack
	cpu.mem.set(static_cast<uint8>(reg_value >> 8), --cpu.regSP);
	cpu.mem.set(static_cast<uint8>(reg_value), --cpu.regSP);
	
	cpu.t_cycles_count += 16;
}
// Pops the 16 bit regsiter that is given from the stakc
void INSTRUCTIONS::pop_from_stack(CPU& cpu, std::string reg_name){
	uint8 low_byte = cpu.mem.read(cpu.regSP++);
	uint8 high_byte = cpu.mem.read(cpu.regSP++);
	uint16 value = (static_cast<uint16>(high_byte) << 8) + low_byte;
	
	if (reg_name == "AF") {
		value &= 0xFFF0;
	}

	cpu.set_register(reg_name, value);
	cpu.t_cycles_count += 16;
}
// Jumps to the addr stored in HL
void INSTRUCTIONS::jump_to_addr_HL(CPU& cpu){
	cpu.regPC = cpu.get_register("HL");
	cpu.t_cycles_count += 4;
}
// pushses PC to stack then sets the PC to the addr 
void INSTRUCTIONS::RST_to_addr(CPU& cpu, uint16 addr) {
	push_on_stack(cpu, "PC"); // handles all the cycles
	cpu.regPC = addr;
}

bool INSTRUCTIONS::check_cond(CPU& cpu, std::string cond_name) {
	if (cond_name == "Z") return cpu.get_flag('Z');
	if (cond_name == "NZ") return !cpu.get_flag('Z');
	if (cond_name == "C") return cpu.get_flag('C');
	if (cond_name == "NC") return !cpu.get_flag('C');
	throw std::runtime_error("NOT VALID FALG");
}
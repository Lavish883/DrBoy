#include "instructions.h"

// Copys the val of reg_two to reg_one
void INSTRUCTIONS::load_r8_to_r8(CPU& cpu, char reg_dest, char reg_source){
	cpu.set_register(reg_dest, cpu.get_register(reg_source));
	cpu.t_cycles_count += 4;
}
// Copys the val of the immediate 16 to the 16 bit register
void INSTRUCTIONS::load_r16_from_n16(CPU& cpu, std::string reg_name){
	uint16 value = cpu.read_immediate16_mem();
	cpu.set_register(reg_name, value);
	cpu.t_cycles_count += 12;
}
// Copys the val from the memory which is pointed by the register to A
void INSTRUCTIONS::load_from_r16_addr_to_A(CPU& cpu, std::string reg_name){
	// substr for HL+ nad HL-
	uint8 value = cpu.mem.read(cpu.get_register(reg_name.substr(0, 2)));
	cpu.regA = value;
	handle_16_bit_increment_from_load_instr(cpu, reg_name);
	cpu.t_cycles_count += 8;
}
// Stores the val of A to the address specified by immediate 16
void INSTRUCTIONS::load_A_to_n16_addr(CPU& cpu){
	uint16 addr = cpu.read_immediate16_mem();
	cpu.mem.set(cpu.regA, addr);
	cpu.t_cycles_count += 12;
}
// Copys the val from the address specified by immediate 16 to A
void INSTRUCTIONS::load_from_n16_addr_to_A(CPU& cpu){
	uint16 addr = cpu.read_immediate16_mem();
	cpu.regA = cpu.mem.read(addr);
	cpu.t_cycles_count += 16;
}
// Stores the val of A to the address pointed by a 16 bit register
void INSTRUCTIONS::load_A_to_r16_addr(CPU& cpu, std::string reg_name){
	cpu.mem.set(cpu.regA, cpu.get_register(reg_name.substr(0, 2)));
	handle_16_bit_increment_from_load_instr(cpu, reg_name);
	cpu.t_cycles_count += 8;
}
// Cpoys the val of immediate 8 to the register
void INSTRUCTIONS::load_n8_to_r8(CPU& cpu, char reg_name){
	uint8 data = cpu.read_immediate8_mem();
	cpu.set_register(reg_name, data);
	cpu.t_cycles_count += reg_name == r_at_HL ? 12 : 8;
}
// Stores the val of A to 0xFF00 + immediate 8
void INSTRUCTIONS::ldh_A_to_n8(CPU& cpu){
	cpu.mem.set(cpu.regA, 0xFF00 + cpu.read_immediate8_mem());
	cpu.t_cycles_count += 12;
}
// Copys the val in the addr of 0xFF00 + immediate 8 to A
void INSTRUCTIONS::ldh_n8_to_A(CPU& cpu){
	cpu.regA = cpu.mem.read(0xFF00 + cpu.read_immediate8_mem());
	cpu.t_cycles_count += 12;
}
// Stores the val of SP to the addr specified by immediate 16
void INSTRUCTIONS::load_SP_to_n16_addr(CPU& cpu){
	uint16 addr = cpu.read_immediate16_mem();
	cpu.mem.set(static_cast<uint8>(cpu.regSP), addr);
	cpu.mem.set(static_cast<uint8>(cpu.regSP >> 8), addr + 1);
	cpu.t_cycles_count += 20;
}
// Copys the val of HL to SP
void INSTRUCTIONS::load_HL_to_SP(CPU& cpu){
	cpu.regSP = cpu.get_register("HL");
	cpu.t_cycles_count += 8;
}
// Copys the value of SP + immediate signed 8 data to HL
void INSTRUCTIONS::load_HL_from_SP_and_e8(CPU& cpu){
	int8 e8 = static_cast<int8>(cpu.read_immediate8_mem());
	uint8 low_sp = cpu.regSP & 0xFF;
	uint8 low_e8 = static_cast<uint8>(e8);

	cpu.set_register("HL", cpu.regSP + e8);
	// SET FLAGS
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((low_sp & 0xF) + (low_e8 & 0xF)) > 0xF);
	cpu.set_flag('C', (low_sp + low_e8) > 0xFF);

	cpu.t_cycles_count += 12;
}
// Copys the value from the addr of Internal Ram + C to A
void INSTRUCTIONS::load_A_from_IR_and_C(CPU& cpu){
	cpu.regA = cpu.mem.read(0xFF00 + cpu.regC);
	cpu.t_cycles_count += 8;
}
// Stores the value of A to the addro of Interal Ram + C
void INSTRUCTIONS::load_A_to_IR_and_C(CPU& cpu) {
	cpu.mem.set(cpu.regA, 0xFF00 + cpu.regC);
	cpu.t_cycles_count += 8;
}

void INSTRUCTIONS::handle_16_bit_increment_from_load_instr(CPU& cpu, std::string reg_name) {
	if (reg_name.size() > 2) {
		if (reg_name.at(2) == '+') {
			cpu.set_register("HL", cpu.get_register("HL") + 1);
		} else {
			cpu.set_register("HL", cpu.get_register("HL") - 1);
		}
	}
}
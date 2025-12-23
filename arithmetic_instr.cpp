#include "instructions.h"

// INC r16
void INSTRUCTIONS::increment_r16(CPU& cpu, std::string reg_name) {
	uint16 og_reg_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, og_reg_value + 1);
	cpu.t_cycles_count += 8;
}

// DEC r16
void INSTRUCTIONS::decrement_r16(CPU& cpu, std::string reg_name) {
	uint16 og_reg_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, og_reg_value - 1);
	cpu.t_cycles_count += 8;
}

// INC r8, increases the 8 bit register by 1
void INSTRUCTIONS::increment_r8(CPU& cpu, char reg_name) {
	uint8 old_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, old_value + 1);

	// SET Flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((old_value & 0x0F) + 1) > 0x0F);

	cpu.t_cycles_count += reg_name == r_at_HL ? 12 : 4;
}

// DEC r8, decreases the 8 bit register by 1
void INSTRUCTIONS::decrement_r8(CPU& cpu, char reg_name) {
	uint8 old_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, old_value - 1);

	// SET Flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 1);
	cpu.set_flag('H', (old_value & 0x0F) == 0);

	cpu.t_cycles_count += reg_name == r_at_HL ? 12 : 4;
}

void INSTRUCTIONS::update_arithmetic_cycles(CPU& cpu, char reg_name, bool is_reg) {
	if (is_reg == false || reg_name == r_at_HL) {
		cpu.t_cycles_count += 8;
	}
	else {
		cpu.t_cycles_count += 4;
	}
}

// ORs the register passed with A (accumulator)
void INSTRUCTIONS::r8_OR_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();

	cpu.regA = cpu.regA | reg_value;
	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', 0);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// CPs compares the content of the register doesn't change value of A
void INSTRUCTIONS::r8_CP_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();
	uint8 result = cpu.regA - reg_value;

	cpu.set_flag('Z', result == 0);
	cpu.set_flag('N', 1);
	cpu.set_flag('H', ((cpu.regA & 0xF) < (reg_value & 0xF)));
	cpu.set_flag('C', cpu.regA < reg_value);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// ANDs the register passed with A (accumlator)
void INSTRUCTIONS::r8_AND_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();

	cpu.regA = cpu.regA & reg_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 1);
	cpu.set_flag('C', 0);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// XORs the register passed with A
void INSTRUCTIONS::r8_XOR_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();

	cpu.regA = cpu.regA ^ reg_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', 0);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// ADDs the register passed with A
void INSTRUCTIONS::r8_ADD_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();
	uint16 old_value = static_cast<uint16>(cpu.regA);

	cpu.regA = cpu.regA + reg_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((old_value & 0xF) + (reg_value & 0xF)) > 0xF);
	cpu.set_flag('C', (old_value + reg_value) > 0xFF);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// SUBs the register passed with A
void INSTRUCTIONS::r8_SUB_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();
	uint16 old_value = static_cast<uint16>(cpu.regA);

	cpu.regA = cpu.regA - reg_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 1);
	cpu.set_flag('H', ((old_value & 0xF) < (reg_value & 0xF)));
	cpu.set_flag('C', old_value < reg_value);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// ADCs the register passed with A
void INSTRUCTIONS::r8_ADC_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();
	uint16 old_value = static_cast<uint16>(cpu.regA);
	uint8_t flag_value = static_cast<uint8>(cpu.get_flag('C'));

	cpu.regA = cpu.regA + reg_value + flag_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((old_value & 0xF) + (reg_value & 0xF) + flag_value) > 0xF);
	cpu.set_flag('C', (old_value + reg_value + flag_value) > 0xFF);

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// SBCs the register passed with A
void INSTRUCTIONS::r8_SBC_r8(CPU& cpu, char reg_name, bool is_reg) {
	uint8 reg_value = is_reg ? cpu.get_register(reg_name) : cpu.read_immediate8_mem();
	uint16 old_value = static_cast<uint16>(cpu.regA);
	uint8_t flag_value = static_cast<uint8>(cpu.get_flag('C'));

	cpu.regA = cpu.regA - reg_value - flag_value;

	// Set all flags
	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('N', 1);
	cpu.set_flag('H', (old_value & 0xF) < ((reg_value & 0xF) + flag_value));
	cpu.set_flag('C', old_value < (reg_value + flag_value));

	update_arithmetic_cycles(cpu, reg_name, is_reg);
}

// ADD SP, e8 adds signed immediate 8
void INSTRUCTIONS::add_signed_immediate_8_to_SP(CPU& cpu) {
	int8 e8 = static_cast<int8>(cpu.read_immediate8_mem());
	uint16 sp_val = cpu.get_register("SP");

	cpu.set_register("SP", sp_val + e8);
	// SET Flags
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((sp_val & 0xF) + (e8 & 0xF)) > 0xF);
	cpu.set_flag('C', ((sp_val & 0xFF) + (e8 & 0xFF)) > 0xFF);

	cpu.t_cycles_count += 16;
}

// ADD HL, r16
void INSTRUCTIONS::add_r16_to_HL(CPU& cpu, std::string reg_name) {
	uint16 reg_HL_value = cpu.get_register("HL");
	uint16 adding_reg_value = cpu.get_register(reg_name);

	cpu.set_register("HL", reg_HL_value + adding_reg_value);

	// SET FLAGS
	cpu.set_flag('N', 0);
	cpu.set_flag('H', ((reg_HL_value & 0x0FFF) + (adding_reg_value & 0x0FFF)) > 0x0FFF);
	cpu.set_flag('C', (reg_HL_value + adding_reg_value) > 0xFFFF);

	cpu.t_cycles_count += 8;
}

// CPL
void INSTRUCTIONS::CPL(CPU& cpu) {
	cpu.regA = ~(cpu.regA);
	// Set flags
	cpu.set_flag('N', 1);
	cpu.set_flag('H', 1);

	cpu.t_cycles_count += 4;
}

// RRA
void INSTRUCTIONS::RRA(CPU& cpu) {
	uint8 old_value = cpu.regA;

	cpu.regA = (cpu.regA >> 1) + (cpu.get_flag('C') << 7);

	// Set flags
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', old_value & 0x01);

	cpu.t_cycles_count += 4;
}

// RLA
void INSTRUCTIONS::RLA(CPU& cpu) {
	uint8 old_value = cpu.regA;

	cpu.regA = (cpu.regA << 1) + cpu.get_flag('C');

	// Set flags
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x80) >> 7);

	cpu.t_cycles_count += 4;
}

// RRCA
void INSTRUCTIONS::RRCA(CPU& cpu) {
	uint8 old_value = cpu.regA;

	cpu.regA = (cpu.regA >> 1) | (cpu.regA << 7);

	// Set flags
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', old_value & 0x01);

	cpu.t_cycles_count += 4;
}

// RLCA
void INSTRUCTIONS::RLCA(CPU& cpu) {
	uint8 old_value = cpu.regA;

	cpu.regA = (cpu.regA << 1) | (cpu.regA >> 7);

	// Set flags
	cpu.set_flag('Z', 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x80) >> 7);

	cpu.t_cycles_count += 4;
}

// DDA
void INSTRUCTIONS::DAA(CPU& cpu) {
	uint8_t adjustment = 0;

	bool n_flag = cpu.get_flag('N');
	bool h_flag = cpu.get_flag('H');
	bool c_flag = cpu.get_flag('C');

	if (n_flag == true) {
		if (h_flag == true) adjustment += 0x06;
		if (c_flag == true) adjustment += 0x60;
		cpu.regA -= adjustment;
	}
	else {
		if (h_flag == true || (cpu.regA & 0x0F) > 0x09) adjustment += 0x06;
		if (c_flag == true || cpu.regA > 0x99) {
			adjustment += 0x60;
			cpu.set_flag('C', 1);
		}
		cpu.regA += adjustment;
	}

	cpu.set_flag('Z', cpu.regA == 0);
	cpu.set_flag('H', 0);

	cpu.t_cycles_count += 4;
}
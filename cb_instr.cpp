#include "instructions.h"

void INSTRUCTIONS::SRL_r8(CPU& cpu, char reg_name){
	uint8 old_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, old_value >> 1);

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', old_value & 0x01);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::RR_r8(CPU& cpu, char reg_name){
	uint8_t old_value = cpu.get_register(reg_name);

	cpu.set_register(reg_name, (old_value >> 1) + (cpu.get_flag('C') << 7));

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', old_value & 0x01);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::RL_r8(CPU& cpu, char reg_name){
	uint8_t old_value = cpu.get_register(reg_name);

	cpu.set_register(reg_name, (old_value << 1) + cpu.get_flag('C'));

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x80) >> 7);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

// Basically swap the high bits (7-4) with (3-0) low bits
void INSTRUCTIONS::SWAP_r8(CPU& cpu, char reg_name){
	uint8_t value = cpu.get_register(reg_name);
	uint8_t high_nibble =  value >> 4;
	uint8_t low_nibble = value & 0xF;

	cpu.set_register(reg_name, ((low_nibble) << 4) + high_nibble);

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', 0);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::RLC_r8(CPU& cpu, char reg_name){
	uint8_t old_value = cpu.get_register(reg_name);

	cpu.set_register(reg_name, (old_value << 1) | (old_value >> 7));

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x80) >> 7);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::RRC_r8(CPU& cpu, char reg_name){
	uint8_t old_value = cpu.get_register(reg_name);

	cpu.set_register(reg_name, (old_value >> 1) | (old_value << 7));

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x01));

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::SRA_r8(CPU& cpu, char reg_name){
	uint8_t old_value = cpu.get_register(reg_name);
	uint8_t value_to_set = (old_value >> 1) + (old_value & 0x80);

	cpu.set_register(reg_name, value_to_set);
	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', old_value & 0x01);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}

void INSTRUCTIONS::SLA_r8(CPU& cpu, char reg_name){
	uint8 old_value = cpu.get_register(reg_name);
	cpu.set_register(reg_name, old_value << 1);

	// Set flags
	cpu.set_flag('Z', cpu.get_register(reg_name) == 0);
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 0);
	cpu.set_flag('C', (old_value & 0x80) >> 7);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}
void INSTRUCTIONS::COPY_CPL_BIT_TO_Z_r8(CPU& cpu, int bit, char reg_name){
	uint8 value = cpu.get_register(reg_name);

	cpu.set_flag('Z', !get_bit(value, bit));
	cpu.set_flag('N', 0);
	cpu.set_flag('H', 1);

	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}
void INSTRUCTIONS::RES_BIT_r8(CPU& cpu, int bit, char reg_name){
	uint8 value = cpu.get_register(reg_name);
	set_bit(value, bit, 0);
	cpu.set_register(reg_name, value);
	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}
void INSTRUCTIONS::SET_BIT_r8(CPU& cpu, int bit, char reg_name) {
	uint8 value = cpu.get_register(reg_name);
	set_bit(value, bit, 1);
	cpu.set_register(reg_name, value);
	cpu.t_cycles_count += reg_name != r_at_HL ? 8 : 16;
}
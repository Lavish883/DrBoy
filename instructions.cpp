#include "instructions.h"
#include <set>

const std::string condition_code_table[4] = { "NZ","Z","NC", "C" };
const std::string r16_group1_table[4] = { "BC", "DE", "HL", "SP" };
const std::string r16_group2_table[4] = { "BC","DE","HL+","HL-" };
const std::string r16_group3_table[4] = { "BC","DE","HL","AF" };
const std::string opcode_group_1_table[8] = { "RLCA", "RRCA", "RLA","RRA", "DAA", "CPL", "SCF", "CCF" };
const std::string opcode_group_2_table[8] = { "ADD","ADC","SUB","SBC","AND","XOR","OR","CP" };
const std::string opcode_group_3_table[8] = { "RLC","RRC","RL", "RR", "SLA","SRA","SWAP","SRL" };
const std::string misc_opcode_group_table[4] = { "RET","RETI","JP HL", "LD SP, HL" };
const char r8_table[8] = { 'B', 'C', 'D', 'E', 'H', 'L', r_at_HL, 'A' };
const std::string r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };

const opcode_group2_fp INSTRUCTIONS::opcode_group2_all_fp[8] = {
	&r8_ADD_r8,
	&r8_ADC_r8,
	&r8_SUB_r8,
	&r8_SBC_r8,
	&r8_AND_r8,
	&r8_XOR_r8,
	&r8_OR_r8,
	&r8_CP_r8
};
const opcode_group3_fp INSTRUCTIONS::opcode_group3_all_fp[8] = {
	&RLC_r8,
	&RRC_r8,
	&RL_r8,
	&RR_r8,
	&SLA_r8,
	&SRA_r8,
	&SWAP_r8,
	&SRL_r8,
};


void INSTRUCTIONS::execute_instruction(CPU& cpu, uint8 opcode) {
	std::ostringstream name;
	//name << "0x" << std::hex << std::setw(4) << std::setfill('0') << std::uppercase << cpu.regPC << ": ";
	opcode_deconstructor(cpu, opcode, name);
	//disassembler_lines.insert(name.str());
	//std::cout << "Instruction Name: " << name.str() << " Opcode: " << opcode << std::endl;
	if (name.str() == "NOT DONE") {
		throw std::runtime_error("SOMETHING WRONG WITH OPCODES");
	}
}
void INSTRUCTIONS::opcode_deconstructor(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	switch (opcode) {
	case 0x00: {
		cpu.t_cycles_count += 4;
		name << "NOP";
		return;
	}
	case 0x08: {
		name << "LD [n16], SP";
		return load_SP_to_n16_addr(cpu);
	}
	// NEED impleantation
	case 0x10: { 
		name << "STOP n8";
		return;
	}
	case 0x07: {
		name << "RLCA";
		return RLCA(cpu);
	}
	case 0x0F: {
		name << "RRCA";
		return RRCA(cpu);
	}
	case 0x17: {
		name << "RLA";
		return RLA(cpu);
	}
	case 0x1F: {
		name << "RRA";
		return RRA(cpu);
	}
	case 0x18: {
		name << "JR e8";
		return uncond_jump_e8(cpu);
	}
	case 0x27: {
		name << "DAA";
		return DAA(cpu);
	}
	case 0x2F: {
		name << "CPL";
		return CPL(cpu);
	}
	case 0x37: {
		cpu.set_flag('N', 0); cpu.set_flag('H', 0); cpu.set_flag('C', 1);
		name << "SCF";
		return;
	}
	case 0x3F: {
		cpu.set_flag('N', 0); cpu.set_flag('H', 0); cpu.set_flag('C', !cpu.get_flag('C'));
		name << "CCF";
		return;
	}
	case 0x76: {
		cpu.in_low_power_mode = true;
		cpu.t_cycles_count += 4;
		name << "HALT";
		return;
	}
	case 0xE0: {
		name << "LDH [a8], A";
		return ldh_A_to_n8(cpu);
	}
	case 0xE8: {
		name << "ADD SP, e8";
		return add_signed_immediate_8_to_SP(cpu);
	}
	case 0xF0: {
		name << "LDH A, [a8]";
		return ldh_n8_to_A(cpu);
	}
	case 0xF8: {
		name << "LD HL, SP + e8";
		return load_HL_from_SP_and_e8(cpu);
	}
	case 0xE2: {
		name << "LDH [C], A";
		return load_A_to_IR_and_C(cpu);
	}
	case 0xEA: {
		name << "LD [n16], A"; // LD [n16], A
		return load_A_to_n16_addr(cpu);
	}
	case 0xF2: {
		name << "LDH A, [C]";
		return load_A_from_IR_and_C(cpu);
	}
	case 0xFA: {
		name << "LD A, [n16]";
		return load_from_n16_addr_to_A(cpu);
	}
	case 0xC3: {
		name << "JP n16";
		return uncond_jump_n16(cpu);
	}
	case 0xD9: {
		name << "RETI";
		cpu.set_IME(true);
		uncond_return(cpu);
		return;
	}
	case 0xC9: {
		name << "RET";
		return uncond_return(cpu);
	}
	case 0xCB: {
		// handle CB-prefixed instructions if needed
		return opcode_deconstructor_cb(cpu, cpu.read_immediate8_mem(), name);
	}
	case 0xF3: {
		cpu.set_IME(false);
		cpu.t_cycles_count += 4;
		name << "DI";
		return;
	}
	case 0xFB: {
		cpu.IME_pending = true;
		cpu.t_cycles_count += 4;
		name << "EI";
		return;
	}
	case 0xCD: {
		name << "CALL n16";
		return uncond_call(cpu);
	}
	case 0xE9: {
		name << "JP HL";
		return jump_to_addr_HL(cpu);
	}
	case 0xF9: {
		name << "LD SP, HL";
		return load_HL_to_SP(cpu);
	}
	case 0xE4: {
		name << "ILLEGAL";
		return;
	}
	case 0xEC: {
		name << "ILLEGAL";
		return;
	}
	case 0xF4: {
		name << "ILLEGAL";
		return;
	}
	case 0xFC: {
		name << "ILLEGAL";
		return;
	}
	default:
		break;
	}

	int first_2_bits_of_opcode = read_bits_of_uint(opcode, 7, 6);
	switch (first_2_bits_of_opcode) {
		// cases should return so no need for a break
		case(0x00): return opcode_deconstructor_for_bits_00(cpu, opcode, name);
		case(0x01): return opcode_deconstructor_for_bits_01(cpu, opcode, name);
		case(0x02): return opcode_deconstructor_for_bits_10(cpu, opcode, name);
		case(0x03): return opcode_deconstructor_for_bits_11(cpu, opcode, name);
		default: break;
	}
}

void INSTRUCTIONS::opcode_deconstructor_for_bits_00(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	int info;
	// JR Conditional check
	if (read_bits_of_uint(opcode, 7, 5) == 0x01 &&
		read_bits_of_uint(opcode, 2, 0) == 0x00
		) {
		info = read_bits_of_uint(opcode, 4, 3);
		name << "JR " << condition_code_table[info] << ", e8";
		return cond_jump_e8(cpu, condition_code_table[info]);
	}

	if (read_bits_of_uint(opcode, 2, 2) == 0x00) {
		// Checks last 4 bits of opcode to find more patterns
		int last_4_bits_of_opcode = read_bits_of_uint(opcode, 3, 0);
		info = read_bits_of_uint(opcode, 5, 4);

		switch (last_4_bits_of_opcode) {
			// LD r16 n16
			case(0x01):{
				name << "LD , " << r16_group1_table[info];
				return load_r16_from_n16(cpu, r16_group1_table[info]);
			}
			// ADD HL, r16
			case (0x09): {
				name << "ADD HL, " << r16_group1_table[info];
				return add_r16_to_HL(cpu, r16_group1_table[info]);
			}
			// LD (r16), A
			case (0x02): {
				name << "LD [" << r16_group2_table[info] << "], A";
				return load_A_to_r16_addr(cpu, r16_group2_table[info]);
			}
			case(0x0A): { // LD A,(r16)
				name << "LD A, [" << r16_group2_table[info] << "]";
				return load_from_r16_addr_to_A(cpu, r16_group2_table[info]);
			}
			case(0x03): { // INC r16
				name << "INC " << r16_group1_table[info];
				return increment_r16(cpu, r16_group1_table[info]);
			}
			case(0x0B): { // DEC r16
				name << "DEC " << r16_group1_table[info];
				return decrement_r16(cpu, r16_group1_table[info]);
			}
		}
	} else {
		int last_3_bits_of_opcode = read_bits_of_uint(opcode, 2, 0);
		info = read_bits_of_uint(opcode, 5, 3);

		switch (last_3_bits_of_opcode) {
			// INC r8
			case(0x04): { 
				name << "INC " << (r8_table[info] == r_at_HL ? "[HL]" : std::string(1, r8_table[info]));
				return increment_r8(cpu, r8_table[info]);
			}
			// DEC r8
			case(0x05): { 
				name << "DEC " << (r8_table[info] == r_at_HL ? "[HL]" : std::string(1, r8_table[info]));
				return decrement_r8(cpu, r8_table[info]);
			}
			// LD r8, n8
			case(0x06): { 
				name << "LD " << (r8_table[info] == r_at_HL ? "[HL]" : std::string(1, r8_table[info])) << ", n8";
				return load_n8_to_r8(cpu, r8_table[info]);	
			}
			case(0x07): {
				// Handled by the switch statemtns in opcode deconstrucotr
				name << opcode_group_1_table[info];
				return;
			}
			default:
				break;
			}
	}

	name << "NOT DONE";
}
void INSTRUCTIONS::opcode_deconstructor_for_bits_01(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	int r8_dest = read_bits_of_uint(opcode, 5, 3);
	int r8_source = read_bits_of_uint(opcode, 2, 0);
	// LD r8, r8
	name << "LD " << (r8_table[r8_dest] == r_at_HL ? "[HL]" : std::string(1, r8_table[r8_dest])) << ", " << (r8_table[r8_source] == r_at_HL ? "[HL]" : std::string(1, r8_table[r8_source]));
	return load_r8_to_r8(cpu, r8_table[r8_dest], r8_table[r8_source]);
}
void INSTRUCTIONS::opcode_deconstructor_for_bits_10(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	int group_info = read_bits_of_uint(opcode, 5, 3);
	int operand_info = read_bits_of_uint(opcode, 2, 0);

	std::string opcode_group = opcode_group_2_table[group_info];
	char r8_operand_2 = r8_table[operand_info];
	// (Add/xor/or/sub...) A, r8
	name << opcode_group << " A, " << (r8_table[operand_info] == r_at_HL ? "[HL]" : std::string(1, r8_table[operand_info]));
	opcode_group2_all_fp[group_info](cpu, r8_operand_2, true);
}
void INSTRUCTIONS::opcode_deconstructor_for_bits_11(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	int last_bit_of_opcode = read_bits_of_uint(opcode, 0, 0);

	switch (last_bit_of_opcode) {
		case(0x0): {
			int last_3_bits_of_opcode = read_bits_of_uint(opcode, 2, 0);
			int info = read_bits_of_uint(opcode, 4, 3);
			switch (last_3_bits_of_opcode) {
				case(0x0): {
					name << "RET " << condition_code_table[info];
					return cond_return(cpu, condition_code_table[info]);
				}
				case(0x02): {
					name << "JP " << condition_code_table[info] << ", n16";
					return cond_jump_n16(cpu, condition_code_table[info]);
				}
				case(0x04): {
					name << "CALL " << condition_code_table[info] << ", n16";
					return cond_call(cpu, condition_code_table[info]);
				}
				case(0x06): {
					int group_info = read_bits_of_uint(opcode, 5, 3);
					name << opcode_group_2_table[group_info] << " A, n8";
					return opcode_group2_all_fp[group_info](cpu, '1', false);
				}
				default: break;
			}
		}
		case(0x01): {
			int last_4_bits_of_opcode = read_bits_of_uint(opcode, 3, 0);
			int info = read_bits_of_uint(opcode, 5, 4);
			
			switch (last_4_bits_of_opcode) {
				//POP r16
				case(0x01): {
					name << "POP " << r16_group3_table[info];
					return pop_from_stack(cpu, r16_group3_table[info]);
				}
				case(0x09): {
					// implented in the swithc cases
					name << misc_opcode_group_table[info];
					return;
				}
				case(0x05): { // PUSH r16
					//push_on_stack(hCpu, info, pT_cycles_count);
					name << "PUSH " << r16_group3_table[info];
					return push_on_stack(cpu, r16_group3_table[info]);
				}
				default: break;
			}
			
			// meaning RST instruction or ILLEGAL
			if (read_bits_of_uint(opcode, 2, 0) == 7) {
				info = read_bits_of_uint(opcode, 5, 3);
				name << "RST $" << std::hex << static_cast<uint8>(info << 3);
				return RST_to_addr(cpu, static_cast<uint16>(info << 3));
			} else {
				name << "ILLEGAL";
				return;
			}
		}
		default: break;
	}
	name << "NOT DONE";
}
void INSTRUCTIONS::opcode_deconstructor_cb(CPU& cpu, uint8 opcode, std::ostringstream& name) {
	uint8 first_2_bits_of_opcode = read_bits_of_uint(opcode, 7, 6);
	int input_1 = read_bits_of_uint(opcode, 5, 3);
	int input_2 = read_bits_of_uint(opcode, 2, 0);

	switch (first_2_bits_of_opcode) {
		case(0x0): {
			name << opcode_group_3_table[input_1] << " " << (r8_table[input_2] == r_at_HL ? "[HL]" : std::string(1, r8_table[input_2]));
			return opcode_group3_all_fp[input_1](cpu, r8_table[input_2]);
		}
		case(0x01): {
			name << "BIT " << input_1 << ", " << (r8_table[input_2] == r_at_HL ? "[HL]" : std::string(1, r8_table[input_2]));
			return COPY_CPL_BIT_TO_Z_r8(cpu, input_1, r8_table[input_2]);
		}
		case(0x02): {
			name << "RES " << input_1 << ", " << (r8_table[input_2] == r_at_HL ? "[HL]" : std::string(1, r8_table[input_2]));
			return RES_BIT_r8(cpu, input_1, r8_table[input_2]);;
		}
		case(0x03): {
			name << "SET " << input_1 << ", " << (r8_table[input_2] == r_at_HL ? "[HL]" : std::string(1, r8_table[input_2]));
			return SET_BIT_r8(cpu, input_1, r8_table[input_2]);
		}
	}
	name << "NOT DONE";
}
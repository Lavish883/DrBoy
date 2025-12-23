#include "disassembler.h"

const std::string condition_code_table[4] = { "NZ","Z","NC", "C" };
const std::string r16_group1_table[4] = { "BC", "DE", "HL", "SP" };
const std::string r16_group2_table[4] = { "BC","DE","HL+","HL-" };
const std::string r16_group3_table[4] = { "BC","DE","HL","AF" };
const std::string opcode_group_1_table[8] = { "RLCA", "RRCA", "RLA","RRA", "DAA", "CPL", "SCF", "CCF" };
const std::string opcode_group_2_table[8] = { "ADD","ADC","SUB","SBC","AND","XOR","OR","CP" };
const std::string opcode_group_3_table[8] = { "RLC","RRC","RL", "RR", "SLA","SRA","SWAP","SRL" };
const std::string misc_opcode_group_table[4] = { "RET","RETI","JP HL", "LD SP, HL" };
const std::string r8_table[8] = { "B", "C", "D", "E", "H", "L", "HL", "A" };
const std::string r16_table_all[6] = { "BC","DE","HL","SP","PC", "AF" };


uint8 opcode_disassembler_for_bits_00(uint8 opcdeo, std::ostringstream& name);
uint8 opcode_disassembler_for_bits_01(uint8 opcdeo, std::ostringstream& name);
uint8 opcode_disassembler_for_bits_10(uint8 opcdeo, std::ostringstream& name);
uint8 opcode_disassembler_for_bits_11(uint8 opcdeo, std::ostringstream& name);


uint8 opcode_disassembler(uint8 opcode, std::ostringstream& name) {
	switch (opcode) {
	case 0x00: name << "NOP"; return 1;
	case 0x08: name << "LD [a16], SP"; return 3;
	case 0x10: name << "STOP n8"; return 2;
	case 0x07: name << "RLCA"; return 1;
	case 0x0F: name << "RRCA"; return 1;
	case 0x17: name << "RLA"; return 1;
	case 0x1F: name << "RRA"; return 1;
	case 0x18: name << "JR e8"; return 2;
	case 0x27: name << "DAA"; return 1;
	case 0x2F: name << "CPL"; return 1;
	case 0x37: name << "SCF"; return 1;
	case 0x3F: name << "CCF"; return 1;
	case 0x76: name << "HALT"; return 1;
	case 0xE0: name << "LDH [a8], A"; return 2;
	case 0xE8: name << "ADD SP, e8"; return 2;
	case 0xF0: name << "LDH A, [a8]"; return 2;
	case 0xF8: name << "LD HL, SP+e8"; return 2;
	case 0xE2: name << "LD [C], A"; return 1;
	case 0xEA: name << "LD [a16], A"; return 3;
	case 0xF2: name << "LD A, [C]"; return 1;
	case 0xFA: name << "LD A, [a16]"; return 3;
	case 0xC3: name << "JP a16"; return 3;
	case 0xD9: name << "RETI"; return 1;
	case 0xC9: name << "RET"; return 1;
	case 0xCB: name << "PREFIX CB"; return 2;
	case 0xF3: name << "DI"; return 1;
	case 0xFB: name << "EI"; return 1;
	case 0xCD: name << "CALL a16"; return 3;
	case 0xE9: name << "JP HL"; return 1;
	case 0xF9: name << "LD SP, HL"; return 1;
	case 0xE4: name << "ILLEGAL"; return 1;
	case 0xEC: name << "ILLEGAL"; return 1;
	case 0xF4: name << "ILLEGAL"; return 1;
	case 0xFC: name << "ILLEGAL"; return 1;
	default: break;
	}

	// Fallthrough to pattern-based decoding
	int first_2_bits = read_bits_of_uint(opcode, 7, 6);
	switch (first_2_bits) {
	case 0x00: return opcode_disassembler_for_bits_00(opcode, name);
	case 0x01: return opcode_disassembler_for_bits_01(opcode, name);
	case 0x02: return opcode_disassembler_for_bits_10(opcode, name);
	case 0x03: return opcode_disassembler_for_bits_11(opcode, name);
		default:   name << "UNKNOWN"; return 1;
	}
}

uint8 opcode_disassembler_for_bits_00(uint8 opcode, std::ostringstream& name) {
	int info;

	// JR cc, e8
	if ((opcode & 0b11100000) == 0b00100000 && (opcode & 0b00000111) == 0x00) {
		info = (opcode >> 3) & 0b11;
		name << "JR " << condition_code_table[info] << ", e8";
		return 2;
	}

	if ((opcode & 0b00000100) == 0x00) {
		int last4 = opcode & 0x0F;
		info = (opcode >> 4) & 0b11;

		switch (last4) {
		case 0x01: name << "LD " << r16_group1_table[info] << ", n16"; return 3;
		case 0x09: name << "ADD HL, " << r16_group1_table[info]; return 1;
		case 0x02: name << "LD [" << r16_group2_table[info] << "], A"; return 1;
		case 0x0A: name << "LD A, [" << r16_group2_table[info] << "]"; return 1;
		case 0x03: name << "INC " << r16_group1_table[info]; return 1;
		case 0x0B: name << "DEC " << r16_group1_table[info]; return 1;
		}
	}
	else {
		int last3 = opcode & 0b111;
		info = (opcode >> 3) & 0b111;

		switch (last3) {
		case 0x04: name << "INC " << r8_table[info]; return 1;
		case 0x05: name << "DEC " << r8_table[info]; return 1;
		case 0x06: name << "LD " << r8_table[info] << ", n8"; return 2;
		case 0x07: name << opcode_group_1_table[info]; return 1;
		}
	}

	name << "NOT DONE";
	return 1;
}

uint8 opcode_disassembler_for_bits_01(uint8 opcode, std::ostringstream& name) {
	int dest = (opcode >> 3) & 0x07;
	int src = opcode & 0x07;

	name << "LD " << r8_table[dest] << ", " << r8_table[src];
	return 1;
}

uint8 opcode_disassembler_for_bits_10(uint8 opcode, std::ostringstream& name) {
	int group_info = (opcode >> 3) & 0x07;
	int operand_info = opcode & 0x07;

	name << opcode_group_2_table[group_info] << " A, " << r8_table[operand_info];
	return 1;
}

uint8 opcode_disassembler_for_bits_11(uint8 opcode, std::ostringstream& name) {
	int high = (opcode >> 3) & 0x07;
	int low = opcode & 0x07;

	if ((opcode & 0x01) == 0x00) {
		switch (low) {
		case 0x0: name << "RET " << condition_code_table[high >> 1]; return 1;
		case 0x2: name << "JP " << condition_code_table[high >> 1] << ", a16"; return 3;
		case 0x4: name << "CALL " << condition_code_table[high >> 1] << ", a16"; return 3;
		case 0x6: name << opcode_group_2_table[high] << " A, n8"; return 2;
		}
	}
	else {
		switch (low) {
		case 0x1: name << "POP " << r16_group3_table[high >> 1]; return 1;
		case 0x5: name << "PUSH " << r16_group3_table[high >> 1]; return 1;
		case 0x9: name << misc_opcode_group_table[high >> 1]; return 1;
		}

		// RST or ILLEGAL
		if ((low & 0b111) == 0x07) {
			name << "RST $" << std::hex << static_cast<uint8>((high << 3));
			return 1;
		}

		name << "ILLEGAL";
		return 1;
	}

	name << "NOT DONE";
	return 1;
}
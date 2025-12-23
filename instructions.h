#ifndef INSTRUCTIONS_CLASS
#define INSTRUCTIONS_CLASS

#include <iostream>
#include <sstream>
#include <set>
#include "cpu.h"
#include "util.h"

using opcode_group2_fp = void(*)(CPU&, char, bool);
using opcode_group3_fp = void(*)(CPU&, char);

class INSTRUCTIONS {
	public:
		static void execute_instruction(CPU& cpu, uint8 opcode);
	private:
		// Functiom pointer arrays
		static const opcode_group2_fp opcode_group2_all_fp[8];
		static const opcode_group3_fp opcode_group3_all_fp[8];


		static void opcode_deconstructor(CPU& cpu, uint8 opcode, std::ostringstream& name);
		static void opcode_deconstructor_cb(CPU& cpu, uint8 opcode, std::ostringstream& name);
		
		// Divides the deconstructot of opcodes into 4 functions base don their first 2 digits of opcode
		static void opcode_deconstructor_for_bits_00(CPU& cpu, uint8 opcode, std::ostringstream& name);
		static void opcode_deconstructor_for_bits_01(CPU& cpu, uint8 opcode, std::ostringstream& name);
		static void opcode_deconstructor_for_bits_10(CPU& cpu, uint8 opcode, std::ostringstream& name);
		static void opcode_deconstructor_for_bits_11(CPU& cpu, uint8 opcode, std::ostringstream& name);

		/* Arithmetic */
		static void increment_r16(CPU& cpu, std::string reg_name);
		static void decrement_r16(CPU& cpu, std::string reg_name);
		
		static void add_r16_to_HL(CPU& cpu, std::string reg_name);

		static void increment_r8(CPU& cpu, char reg_name);
		static void decrement_r8(CPU& cpu, char reg_name);

		static void update_arithmetic_cycles(CPU& cpu, char reg_name, bool is_reg); // This works for OR, CP, AND, XOR, ADD, SUB, ADC, SBC
		static void r8_OR_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_CP_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_AND_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_XOR_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_ADD_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_SUB_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_ADC_r8(CPU& cpu, char reg_name, bool is_reg);
		static void r8_SBC_r8(CPU& cpu, char reg_name, bool is_reg);

		static void add_signed_immediate_8_to_SP(CPU& cpu);

		static void CPL(CPU& cpu);
		static void RRA(CPU& cpu);
		static void RLA(CPU& cpu);
		static void RRCA(CPU& cpu);
		static void RLCA(CPU& cpu);
		static void DAA(CPU& cpu);
		/* */
		/* Loading 
			r8 means register 8 bit
			r16 means register 16 bit
			n16 immediate 16
			n8 immediate 8
			e8 means immeidate signed
			IR internal ram
		*/
		static void load_r8_to_r8(CPU& cpu, char reg_dest, char reg_source);
		static void load_r16_from_n16(CPU& cpu, std::string reg_name);
		static void load_from_r16_addr_to_A(CPU& cpu, std::string reg_name);
		static void load_A_to_n16_addr(CPU& cpu);
		static void load_from_n16_addr_to_A(CPU& cpu);
		
		static void ldh_A_to_n8(CPU& cpu);
		static void ldh_n8_to_A(CPU& cpu);
		
		static void load_A_to_r16_addr(CPU& cpu, std::string reg_name);
		static void load_n8_to_r8(CPU& cpu, char reg_name);

		static void load_SP_to_n16_addr(CPU& cpu);
		static void load_HL_to_SP(CPU& cpu);
		static void load_HL_from_SP_and_e8(CPU& cpu);
		
		static void load_A_from_IR_and_C(CPU& cpu);
		static void load_A_to_IR_and_C(CPU& cpu);

		static void handle_16_bit_increment_from_load_instr(CPU& cpu, std::string rg_name);

		// Control transfer instructions below
		static void uncond_jump_n16(CPU& cpu);
		static void cond_jump_n16(CPU& cpu, std::string cond);
		static void uncond_jump_e8(CPU& cpu);
		static void cond_jump_e8(CPU& cpu, std::string cond_name);
		static void uncond_call(CPU& cpu);
		static void cond_call(CPU& cpu, std::string cond_name);
		static void uncond_return(CPU& cpu);
		static void cond_return(CPU& cpu, std::string cond_name);
		static void push_on_stack(CPU& cpu, std::string reg_name);
		static void pop_from_stack(CPU& cpu, std::string reg_name);
		static void jump_to_addr_HL(CPU& cpu);
		static void RST_to_addr(CPU& cpu, uint16 addr);

		static bool check_cond(CPU& cpu, std::string cond_name);

		//
		static void SRL_r8(CPU& cpu, char reg_name);
		static void RR_r8(CPU& cpu, char reg_name);
		static void RL_r8(CPU& cpu, char reg_name);
		static void SWAP_r8(CPU& cpu, char reg_name);
		static void RLC_r8(CPU& cpu, char reg_name);
		static void RRC_r8(CPU& cpu, char reg_name);
		static void SRA_r8(CPU& cpu, char reg_name);
		static void SLA_r8(CPU& cpu, char reg_name);
		static void COPY_CPL_BIT_TO_Z_r8(CPU& cpu, int bit, char reg_name);
		static void RES_BIT_r8(CPU& cpu, int bit, char reg_name);
		static void SET_BIT_r8(CPU& cpu, int bit, char reg_name);
};

#endif
#ifndef CPU_CLASS
#define CPU_CLASS

#include <cstdint>
#include <iostream>
#include <fstream>
#include <set>

#include "util.h"
#include "memory.h"

#define r_at_HL 't' // Did this so we can refer [HL] with one letter makes it better for get_reg and other r8 opcodes reusability

class INSTRUCTIONS;

class CPU {
	friend class INSTRUCTIONS;
	public:
		CPU(Memory& memory) : mem(memory) {
			log_file.open("log.txt");
			if (!log_file.is_open()) {
				throw std::runtime_error("Could not open to log to file");
			}
		}
		// Returns the amount of t_cycles took
		unsigned int execute();

		uint8 get_register(char reg_name);
		void set_register(char reg_name, uint8 value);

		uint16 get_register(std::string reg_name);
		void set_register(std::string reg_name, uint16 value);

		bit get_flag(char flag);
		void set_flag(char flag, bit value);

		void set_IME(bool value);
	private:
		void handle_clock();
		void handle_interrupts();
		void log_to_file();

		std::ofstream log_file;
		// TO allow instructions to properly do their work
		uint8 read_immediate8_mem();
		void set_immediate8_mem(uint8 value);
		uint16 read_immediate16_mem();
		//void set_immediate16_mem(uint16 value);

		uint8 regA = 0x01;
		uint8 regF = 0xB0;
		uint8 regB = 0x00;
		uint8 regC = 0x13;
		uint8 regD = 0x00;
		uint8 regE = 0xD8;
		uint8 regH = 0x01;
		uint8 regL = 0x4D;
		uint16 regSP = 0xFFFE;
		uint16 regPC = 0x0100;
		bool IME = false;
		bool IME_pending = false;
		unsigned long t_cycles_count = 0;
		unsigned int div_cycles_count = 0;
		unsigned int tima_cycles_count = 0;
		bool in_low_power_mode = false;
		int dma_cycles_left = 0;
		Memory& mem;
};


#endif
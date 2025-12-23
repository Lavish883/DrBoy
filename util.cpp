#include "util.h"

std::ostream& operator<<(std::ostream& os, uint8 num) {
	os << "0x" << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(num);
	return os;
}


void set_bit(uint8& num, uint8 bit_to_set, bit value) {
	if (value == 0) {
		num = num & ~(1 << bit_to_set);
	} else if (value == 1) {
		num = num | (1 << bit_to_set);
	}
	else {
		throw new std::runtime_error("Bit must be 0 or 1");
	}
}

// Inclusive on both ends
uint8 read_bits_of_uint(uint8 num, uint8 upper_bit, uint8 lower_bit){
	return (static_cast<uint8>(num << (7 - upper_bit))) >> (7 - upper_bit + lower_bit);
}
#ifndef UTIL_H
#define UTIL_H

#include <iostream>
#include <cstdint>
#include <iomanip>

#define LY_ADDR 0xFF44
#define LYC_ADDR 0xFF45
#define STAT_ADDR 0xFF41
#define IF_ADDR 0xFF0F
#define LCDC_ADDR 0xFF40
#define BGP_ADDR 0xFF47
#define WY_ADDR 0xFF4A
#define WX_ADDR 0xFF4B
#define SCY_ADDR 0xFF42
#define SCX_ADDR 0xFF43
#define VRAM_START 0x8000
#define TILE_MAP_START 0x9800
#define TILE_SIZE 8
#define TILE_COLS 12
#define PIXEL_SCALE 7
#define TILE_ROWS 32
#define ATLAS_WIDTH  (TILE_COLS * TILE_SIZE)
#define ATLAS_HEIGHT (TILE_ROWS * TILE_SIZE)
#define OAM_START_ADDR 0xFE00
#define OAM_DMA_ADDR 0xFF46

#define JOYP_ADDR 0xFF00


#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCALE_FACTOR 4
#define CYCLES_PER_FRAME 70224
#define SCREEN_WIDTH_WITH_HIDDEN 256
#define SCREEN_HEIGHT_WITH_HIDDEN 256

#define UP 0 
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define A_BUTTON 4
#define B_BUTTON 5
#define START 6
#define SELECT 7

using int8 = std::int8_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using bit = bool;

std::ostream& operator<<(std::ostream& os, uint8 num);

inline bit get_bit(uint8 num, uint8 bit_to_set) {
	return (num & (0x01 << bit_to_set)) >> bit_to_set;
}
void set_bit(uint8& num, uint8 bit_to_set, bit value);
uint8 read_bits_of_uint(uint8 num, uint8 upper_bit, uint8 lower_bit);
#endif
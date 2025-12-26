#ifndef PPU_CLASS
#define PPU_CLASS

#include "memory.h"
#include <bitset>
#include <vector>
#include <bit>
#include <array>
#include "util.h"
#include <string.h>
#include <SFML/Graphics.hpp>

class PPU {
public:
	// tile_pixels
	uint8_t tile_pixels[384][8][8]; // 2 bit values, 384 tiles that are 8 px by 8px
	std::array<std::array<std::array<uint16_t, 32>, 32>, 2> tile_map; // How the tiles are mapped, 32 since 256px total / 8px per tile, [2] since two tile maps
	uint32_t main_screen_framebuffer [144][160]; // Since gameboy renders row by row



	PPU(Memory& memory) : mem(memory) {
	}
	void init();
	void execute(unsigned int t_cycles_took);
	void update_dirty_tiles(int tile_index);
	void update_dirty_bg_map(int tile_place);
	void get_tiles_data(uint32_t* frame_buffer, int tiles_per_row, int tile_size);
	void get_tile_map_data(uint32_t* frame_buffer, uint16 start_addr);
private:
	void update_tile_pixels();
	void update_tile_maps();		
	void render_scanline();
	void handle_mode();
	void handle_stat_interuppts(uint8 LY, uint8 LYC);
	
	int mode = 2;
	int prev_mode = 2;
	int prev_LYC_coincidence = 0;
	int window_line_counter = 0;


	void oam_search();
	void update_LY_register();
	unsigned int t_cycles_count = 0;
	
	void* tile_atlas_texture = nullptr;
	void* bg_atlas_texture = nullptr;
	
	std::bitset<384> dirty_tiles;
	std::bitset<2048> dirty_bg_map; // Can place 1024 tiles, but two tilemaps

	Memory& mem;
	bool line_rendered = false;
};


#endif // !PPU_CLASS
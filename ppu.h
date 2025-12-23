#ifndef PPU_CLASS
#define PPU_CLASS

#include "memory.h"
#include <bitset>
#include <vector>
#include "util.h"
#include <SFML/Graphics.hpp>

class PPU {
public:
	std::vector<uint32_t> atlas_framebuffer;
	std::vector<uint32_t> bg_pixels;
	std::vector<uint32_t> screen_framebuffer;
	// tile_pixels
	uint8_t tile_pixels[384][8][8]; // 2 bit values, 384 tiles that are 8 px by 8px
	uint16_t tile_map [2][32][32]; // How the tiles are mapped, 32 since 256px total / 8px per tile, [2] since two tile maps
	uint32_t main_screen_framebuffer [144][160]; // Since gameboy renders row by row



	PPU(Memory& memory) : atlas_framebuffer(ATLAS_WIDTH * ATLAS_HEIGHT), bg_pixels(SCREEN_WIDTH_WITH_HIDDEN * SCREEN_HEIGHT_WITH_HIDDEN), screen_framebuffer(SCREEN_WIDTH * SCREEN_HEIGHT), mem(memory) {
	}
	void init();
	void execute(unsigned int t_cycles_took);
	void update_dirty_tiles(int tile_index);
	void update_dirty_bg_map(int tile_place);

	void update_tile_atlas();
	void update_bg_atlas();
private:
	void update_tile_pixels();
	void update_tile_maps();		
	void render_scanline_v2();



	void oam_search();
	void update_LY_register();
	void render_scanline();
	int get_tile_id(bit LCDC_4_value, uint8 tile_index);
	unsigned int t_cycles_count = 0;
	
	void* tile_atlas_texture = nullptr;
	void* bg_atlas_texture = nullptr;
	
	std::bitset<384> dirty_tiles;
	std::bitset<2048> dirty_bg_map; // Can place 1024 tiles, but two tilemaps

	Memory& mem;
	bool line_rendered = false;
};


#endif // !PPU_CLASS
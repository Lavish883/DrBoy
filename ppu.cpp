#include "ppu.h"

sf::Color dmgPalette[4] = {
    {255, 255, 255}, // White (0)
    {169, 169, 169}, // ligt gray (1)
    {84, 84, 84}, // dark gray (2)
    {0, 0, 0} // black (3)
};

void PPU::init() {
	return;
}

void PPU::execute(unsigned int t_cycles_took) {
	t_cycles_count += t_cycles_took;
	this->update_LY_register();
}

void PPU::oam_search() {
	for (int i = 0; i < 40; i++) { // Since 40 sprites can be held in the OAM

	}
}

void PPU::render_scanline() {
	uint8 LY = mem.read(LY_ADDR);
	std::vector<uint32_t> line_of_pixels(160); // 160 pixels in one scanline
	std::vector<int> oam_in_line(10); // Max 10 sprites allowed per line

	uint8 LCDC_value = mem.read(LCDC_ADDR);
	bit obj_size = get_bit(LCDC_value, 2); // if 0 8x8 else 8x16
	int obj_height = obj_size == 1 ? 16: 8;

	for (int i = 0; i < 40; i++) { // Since 40 sprites can be held in the OAM
		if (oam_in_line.size() == 10) break;
		uint8 y_pos = mem.read(OAM_DMA_ADDR + 4 * i); // 4 * since one sprite takes 4 bytes
		if (LY + 16 >= y_pos && LY + 16 <= y_pos + obj_height) {
			oam_in_line.push_back(i);
		}
	}

	uint8 global_y = LY + mem.read(SCY_ADDR);
	bit LCDC_3_value = get_bit(LCDC_value, 3);

	uint16 bg_start_addr = LCDC_3_value == 1 ? 0x9C00 : 0x9800;
	
	// We get what tile first corresponds to that pixel position get that tile and tehn look at its pixel color at that pos
	for (int i = 0; i < 160; i++) { // 160 pixels for that scanline bg
		uint8 global_x = i + mem.read(SCX_ADDR);	
		uint8 tile_row = global_y / 8;		
		uint8 tile_col = global_x / 8;	
		
		uint16 map_offset = tile_row * 32 + tile_col;
		uint16 tile_index_addr = bg_start_addr + map_offset;

		uint8 tile_index = mem.read(tile_index_addr);
		uint8 tile_id = get_tile_id(get_bit(LCDC_value, 4), tile_index); // Corrects the tile id to match the index from the atlas_framebuffer
		//std::cout << tile_id;
		uint8 x_pos = i % 8;
		uint8 y_pos = global_y % 8;
		
		uint32_t pixel_color = atlas_framebuffer.at(tile_id * 16 + (x_pos + y_pos * ATLAS_WIDTH));
		screen_framebuffer.at(LY * SCREEN_WIDTH + i) = pixel_color;
		//mem.read(VRAM_START + tile_id);
	}
	
	line_rendered = false;
}

void PPU::render_scanline_v2() {
	uint8 LY = mem.read(LY_ADDR);
	uint8 LCDC_value = mem.read(LCDC_ADDR);
	bit LCDC_3_value = get_bit(LCDC_value, 3);
	bit LCDC_4_value = get_bit(LCDC_value, 4);

	bit tile_map_chosen = LCDC_3_value == 1 ? 1 : 0;

	for (int x = 0; x < 160; x++) { // 160 pixels per scanline
		uint8 tile_index = tile_map[tile_map_chosen][LY >> 3][x >> 3];
		int corrected_tile_index = LCDC_4_value == 1 ? tile_index : 256 + static_cast<int8_t>(tile_index);
		uint8 pixel_2bit = tile_pixels[corrected_tile_index][LY % 8][x % 8];

		main_screen_framebuffer[LY][x] = dmgPalette[pixel_2bit].toInteger();
	}

	line_rendered = false;
}


void PPU::update_LY_register() {
	uint8 LY = mem.read(LY_ADDR);
	if (LY < 144 && t_cycles_count >= 172 && !line_rendered) {
		update_tile_pixels();
		update_tile_maps();
		render_scanline_v2();
	}

	if (t_cycles_count >= 456) {
		if (LY == 153) {
			mem.set(0, LY_ADDR);
		}
		else {
			mem.set(LY + 1, LY_ADDR);
		}

		if (LY + 1 == 144) { // VBLANK Interuppt
			mem.set(mem.read(IF_ADDR) | 0x01, IF_ADDR);
		}
		line_rendered = false;
		t_cycles_count -= 456;
	}
}


void PPU::update_dirty_tiles(int tile_index) {
	dirty_tiles.set(tile_index);
}

void PPU::update_tile_pixels() {
	if (dirty_tiles.none()) return;
	for (int tile_index = 0; tile_index < 384; tile_index++) {
		if (!dirty_tiles.test(tile_index)) continue;
		// First 16 bytes would be of tile 1, grouped by 2 to get rows
		for (int row = 0; row < TILE_SIZE; row++) {
			uint8 low_byte = mem.read(VRAM_START + (tile_index << 4) + (row << 1));
			uint8 high_byte = mem.read(VRAM_START + (tile_index << 4) + (row << 1) + 1);
			
			for (int col = 0; col < TILE_SIZE; col++) {
				uint8 color_index = (get_bit(high_byte, 7 - col) << 1) + get_bit(low_byte, 7 - col);
				tile_pixels[tile_index][row][col] = color_index;
			}
		} 
	}
	dirty_tiles.reset();
}

void PPU::update_dirty_bg_map(int tile_place) {
	dirty_bg_map.set(tile_place);
}

void PPU::update_tile_maps() {
	if (dirty_bg_map.none()) return;
	for (int tile = 0; tile < 2048; tile++){
		if (!dirty_bg_map.test(tile)) continue;
		int map_num = tile >= 1024 ? 1: 0;
		tile_map[map_num][tile / 32][tile % 32] = mem.read(TILE_MAP_START + tile);
	}
	dirty_bg_map.reset();
}





void PPU::update_tile_atlas() {
	if (dirty_tiles.none()) return;
	for (int tile_index = 0; tile_index < 384; tile_index++) {
		if (dirty_tiles.test(tile_index)) {
			int tile_x_index = tile_index % TILE_COLS; // horizontal tile index
			int tile_y_index = tile_index / TILE_COLS; // vertical tile index
			uint16 tile_base = VRAM_START + tile_index * 16;

			for (int row = 0; row < TILE_SIZE; row++) {
				uint8 low_byte = mem.read(tile_base + row * 2);
				uint8 high_byte = mem.read(tile_base + row * 2 + 1);

				for (int col = 0; col < TILE_SIZE; col++) {
					int px = tile_x_index * 8 + col;
					int py = tile_y_index * 8 + row;

					uint8 color_index = get_bit(high_byte, 7 - col) * 2 + get_bit(low_byte, 7 - col);
					atlas_framebuffer[(py * ATLAS_WIDTH + px) + 0] = (dmgPalette[color_index].a << 24) | (dmgPalette[color_index].r << 16) | (dmgPalette[color_index].g << 8) | (dmgPalette[color_index].b);
					// Reddish tint
					//atlas_framebuffer[(py * ATLAS_WIDTH + px) + 0] = (dmgPalette[color_index].r << 24) | (dmgPalette[color_index].g << 16) | (dmgPalette[color_index].b << 8) | 255;
				}
			}
		}
	}
	dirty_tiles.reset();
	update_bg_atlas();
}

int PPU::get_tile_id(bit LCDC_4_value, uint8 tile_index) {
	if (LCDC_4_value) {
		return tile_index; // unsigned index into 0x8000
	}
	else {
		int8_t signed_index = static_cast<int8_t>(tile_index);
		return 256 + signed_index; // shift signed index to valid range
	}
}

void PPU::update_bg_atlas() {
	uint8 LCDC_value = mem.read(LCDC_ADDR);

	bit LCDC_4_value = get_bit(LCDC_value, 4);
	bit LCDC_3_value = get_bit(LCDC_value, 3);

	uint16 bg_start_addr = LCDC_3_value == 1 ? 0x9C00 : 0x9800;
	uint16 bg_end_addr = LCDC_3_value == 1 ? 0x9FFF : 0x9BFF;

	for (uint16 bg_addr = bg_start_addr; bg_addr <= bg_end_addr; bg_addr++) {
		uint8 tile_index = mem.read(bg_addr);
		int tile_id = get_tile_id(LCDC_4_value, tile_index);

		int tile_x = tile_id % TILE_COLS;
		int tile_y = tile_id / TILE_COLS;

		int map_x = (bg_addr - bg_start_addr) % 32;
		int map_y = (bg_addr - bg_start_addr) / 32;

		for (int row = 0; row < TILE_SIZE; row++) {
			for (int col = 0; col < TILE_SIZE; col++) {
				// Get pixel from atlas
				int atlas_px = tile_x * TILE_SIZE + col;
				int atlas_py = tile_y * TILE_SIZE + row;
				uint32_t pixel_color = atlas_framebuffer.at(atlas_py * ATLAS_WIDTH + atlas_px);

				// Write to bg_pixels
				int bg_px = map_x * TILE_SIZE + col;
				int bg_py = map_y * TILE_SIZE + row;
				if (bg_px < 256 && bg_py < 256) {
					bg_pixels.at(bg_py * 256 + bg_px) = pixel_color;
				}
			}
		}
	}

}
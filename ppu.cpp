#include "ppu.h"

uint32_t dmgPalette[4] = {
    0xFFFFFFFF, // white (0)
    0xFFA9A9A9, // light gray (1)
    0xFF545454, // dark gray (2)
    0xFF000000 // black (3)
};

uint32_t dmgGreenPalette[4] = {
    0xffe0f8d0, // lightest green (white)
    0xff88c070, // light green
    0xff346856, // dark green
    0xff081820  // darkest green (black)
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
	uint8 bg_palette_mapping = mem.read(BG_PALETTE_ADDR);
	uint32_t bg_palette[4] = {
		dmgPalette[(get_bit(bg_palette_mapping, 1) << 1) + get_bit(bg_palette_mapping, 0)], // 0
		dmgPalette[(get_bit(bg_palette_mapping, 3) << 1) + get_bit(bg_palette_mapping, 2)], // 1
		dmgPalette[(get_bit(bg_palette_mapping, 5) << 1) + get_bit(bg_palette_mapping, 4)], // 2
		dmgPalette[(get_bit(bg_palette_mapping, 7) << 1) + get_bit(bg_palette_mapping, 6)]  // 3
	};

	
	uint8 LY = mem.read(LY_ADDR);
	uint8 LCDC_value = mem.read(LCDC_ADDR);
	uint8 SCX_value = mem.read(SCX_ADDR);
	uint8 SCY_value = mem.read(SCY_ADDR);
	uint8 WX_value = mem.read(WX_ADDR);
	uint8 WY_value = mem.read(WY_ADDR);
	
	bit LCDC_0_value = get_bit(LCDC_value, 0); // BG and Window enable/priority
	bit LCDC_3_value = get_bit(LCDC_value, 3);
	bit LCDC_4_value = get_bit(LCDC_value, 4);
	bit LCDC_5_value = get_bit(LCDC_value, 5);
	bit LCDC_6_value = get_bit(LCDC_value, 6);


	bit bg_tile_map_chosen = LCDC_3_value == 1 ? 1 : 0;
	bit window_tile_map_chosen = LCDC_6_value == 1 ? 1 : 0;
	
	bool window_pixel_used = false;
	// Renders the background and window
	for (int x = 0; x < 160; x++) { // 160 pixels per scanline
		if (LCDC_0_value == 0) {
			main_screen_framebuffer[LY][x] = dmgPalette[0]; // White if LCDC_0 is 0
			continue;
		}
		bool render_window = LCDC_5_value == 1 && WY_value <= LY &&	x >= WX_value - 7; // means render window pixel
		if (render_window) window_pixel_used = true;

		uint8 bg_x = render_window ? x - WX_value + 7  : x + SCX_value;
		uint8 bg_y = render_window ? window_line_counter : LY + SCY_value;

		uint8 tile_index = tile_map.at(render_window ? window_tile_map_chosen : bg_tile_map_chosen).at(bg_y >> 3).at(bg_x >> 3);
		int corrected_tile_index = LCDC_4_value == 1 ? tile_index : 256 + static_cast<int8_t>(tile_index);
		uint8 color_index = tile_pixels[corrected_tile_index][bg_y & 7][bg_x & 7]; // & 7 is mod 8

		main_screen_framebuffer[LY][x] = bg_palette[color_index];
	}

	//Render the objects/sprites
	uint8 obj0_palette_mapping = mem.read(OBJ0_PALETTE_ADDR);
	uint8 obj1_palette_mapping = mem.read(OBJ1_PALETTE_ADDR);

	uint32_t obj0_palette[4] = {
		0x0, 	// 0 (always transparent) handled in if code
		dmgPalette[(get_bit(obj0_palette_mapping, 3) << 1) | get_bit(obj0_palette_mapping, 2)], // 1
		dmgPalette[(get_bit(obj0_palette_mapping, 5) << 1) | get_bit(obj0_palette_mapping, 4)],	// 2
		dmgPalette[(get_bit(obj0_palette_mapping, 7) << 1) | get_bit(obj0_palette_mapping, 6)]	// 3
	};

	uint32_t obj1_palette[4] = {
		0x0, 	// 0 (always transparent) handled in if code
		dmgPalette[(get_bit(obj1_palette_mapping, 3) << 1) | get_bit(obj1_palette_mapping, 2)], // 1
		dmgPalette[(get_bit(obj1_palette_mapping, 5) << 1) | get_bit(obj1_palette_mapping, 4)],	// 2
		dmgPalette[(get_bit(obj1_palette_mapping, 7) << 1) | get_bit(obj1_palette_mapping, 6)]	// 3
	};


	bit LCDC_2_value = get_bit(LCDC_value, 2); // obj size
	int objs[10];
	int obj_count = 0;

	int obj_height = LCDC_2_value == 0 ? 8 : 16;

	for (int sprite_num = 0; sprite_num < 40; sprite_num++) { // Since max 40 sprites in oam, 0 y pos, 1 x pos, 2 tile index, 3 attribute/flags
		if (LY + 16 >= oam_map[sprite_num][OAM_SPRITE_Y] && LY + 16 <= oam_map[sprite_num][OAM_SPRITE_Y] + obj_height) {
			objs[obj_count] = sprite_num;
			obj_count++;
			if (obj_count == 10) break;
		}
	}

	for (int sprite_num = 0; sprite_num < obj_count; sprite_num++) {
		auto sprite = oam_map[objs[sprite_num]];
		uint8 sprite_x = sprite[OAM_SPRITE_X]; 

		if (LCDC_2_value == 0){ // HANDLE 8x8 
			if (sprite_x == 0 || sprite_x >= 168) continue; // sprite is out of screen

			for (int x = 0; x < 8; x++) { // Potentially 8 pixels for witdh to display
				// Check if the pixel are on screen
				if (x + sprite_x > 8 && x + sprite_x < 168) {
					uint8 color_index = tile_pixels[sprite[OAM_SPRITE_TILE]][(LY + 16) & 7][x];
					uint32_t* palette = get_bit(sprite[OAM_SPRITE_FLAGS], 4) == 0 ? obj0_palette : obj1_palette;
					
					if (color_index != 0) { // Since 0 would be transparent 
						main_screen_framebuffer[LY][x + sprite_x - 8] =  palette[color_index];
					}
				}
			}
		} else { // HANDE 8x16, 

		}
	}

	if (window_pixel_used) window_line_counter += 1;
	line_rendered = true;
}

void PPU::get_tiles_data(uint32_t* frame_buffer, int tiles_per_row, int tile_size) {
	int entire_tile_row_width = tiles_per_row * tile_size; 

	for (int i = 0; i < 384; i++){
		for (int r = 0; r < tile_size; r++){
			for (int c = 0; c < tile_size; c++) {
				frame_buffer[
					(r * entire_tile_row_width) + 
					((i / tiles_per_row) * tile_size * entire_tile_row_width) + 
					(c + (i % tiles_per_row) * tile_size)
				] = dmgPalette[tile_pixels[i][r][c]];
			}
		}
	}
}

void PPU::get_tile_map_data(uint32_t* frame_buffer, uint16 start_addr) {
	int map_to_use = start_addr == 0x9800 ? 0 : 1;
	int tile_size = 8;
	int entire_tile_row_width = tile_size * 32; // 8px, 32 tiles

	for (int tile_y = 0; tile_y < 32; tile_y++) { // Since each tile map contains 1024 tiles
		for (int tile_x = 0; tile_x < 32; tile_x++) {
			int tile_index = tile_map.at(map_to_use).at(tile_y).at(tile_x);
			for (int r = 0; r < 8; r++) {
				for (int c = 0; c < 8; c++) {
					frame_buffer [
						(r * entire_tile_row_width) + 
						(tile_y * tile_size * entire_tile_row_width) + 
						(c + (tile_x) * tile_size)
					] = dmgPalette[tile_pixels[tile_index][r][c]];
				}
			}
		}
	}
}

void PPU::handle_mode() {
	if (t_cycles_count <= 80) { mode = 2; return; }
	if (t_cycles_count > 80 && t_cycles_count <= 252) { mode = 3; return; }
	mode = 0; return;
}

void PPU::handle_stat_interuppts(uint8 LY, uint8 LYC) {
	uint8 STAT = mem.read(STAT_ADDR);
	uint8 IF_value = mem.read(IF_ADDR);

	if (
		(get_bit(STAT, 6) == 1 && LYC == LY && prev_LYC_coincidence != (LYC == LY)) ||
		(get_bit(STAT, 5) == 1 && mode == 2 && prev_mode != 2) ||
		(get_bit(STAT, 4) == 1 && mode == 1 && prev_mode != 1) ||
		(get_bit(STAT, 3) == 1 && mode == 0 && prev_mode != 0) // If any of those conditions are true set an interrupt
		) {
		IF_value |= 0b10;
		mem.set(IF_value, IF_ADDR, false);
	}

	// Update STAT
	set_bit(STAT, 2, LYC == LY);
	
	set_bit(STAT, 1, 0);
	set_bit(STAT, 0, 0); // Clear mode bits
	
	STAT |= mode;
	mem.set(STAT, STAT_ADDR, false);

	prev_LYC_coincidence = (LYC == LY);
}

void PPU::update_LY_register() {
	handle_mode();
	uint8 LY = mem.read(LY_ADDR);
	uint8 LYC = mem.read(LYC_ADDR);

	if (LY >= 144) {
		mode = 1; // V_BLANK
		window_line_counter = 0;
	}

	switch (mode)
	{
		case 3: {
			mem.set_vram_write_access_allowed(false);
			if (line_rendered) break;
			update_tile_pixels();
			update_tile_maps();
			update_oam_map();
			render_scanline();
			break;
		}
		default: {
			mem.set_vram_write_access_allowed(true);
			break;
		}
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
	handle_stat_interuppts(LY, LYC);
	
	prev_mode = mode;
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

void PPU::update_dirty_oam(int place) {
	dirty_oam.set(place);
}

void PPU::update_oam_map() {
	if (dirty_oam.none()) return;
	for (int byte = 0; byte < 160; byte++){
		if (!dirty_oam.test(byte)) continue;
		oam_map.at(byte / 4).at(byte % 4) = mem.read(OAM_START_ADDR + byte);
	}
	dirty_oam.reset();
}

void PPU::update_tile_maps() {
	if (dirty_bg_map.none()) return;
	for (int tile = 0; tile < 2048; tile++){
		if (!dirty_bg_map.test(tile)) continue;
		int map_num = tile >= 1024 ? 1: 0;
		tile_map.at(map_num).at((tile % 1024) / 32).at((tile % 1024) % 32) = mem.read(TILE_MAP_START + tile);
	}
	dirty_bg_map.reset();
}
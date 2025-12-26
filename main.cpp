#include <iostream>
#include "util.h"
#include "emulator.h"
#include <sstream>
#include <map>

#include <SFML/Graphics.hpp>

const int num_of_instructions[11] = {
	1456935, // 1256935
	181458,
	1266762,
	1460805,
	1961527,
	261512,
	607600,
	241832,
	4618250,
	6912600,
	7627800,
};

void test_all_cpu_blargg(int test_num, std::map<std::string, sf::RenderWindow *> &windows);

void handle_input(Emulator& emu, sf::Event event);

int main(int argc, char *argv[])
{
	std::map<std::string, sf::RenderWindow *> windows;
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH * SCALE_FACTOR, SCREEN_HEIGHT * SCALE_FACTOR), "Game Boy Emulator");
	sf::RenderWindow tiles_window(sf::VideoMode(ATLAS_WIDTH * SCALE_FACTOR, ATLAS_HEIGHT * SCALE_FACTOR), "Tiles");
	//sf::RenderWindow background_window(sf::VideoMode(256 * SCALE_FACTOR/2, 256 * SCALE_FACTOR/2), "BG");


	sf::Texture bg_screen_texture;
	sf::Texture tiles_texture;
	sf::Texture main_screen_texture;

	main_screen_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	bg_screen_texture.create(256, 256);
	tiles_texture.create(ATLAS_WIDTH, ATLAS_HEIGHT);

	sf::Sprite bg_screen(bg_screen_texture);
	sf::Sprite screen(main_screen_texture);
	sf::Sprite tiles_sprite(tiles_texture);

	tiles_sprite.setScale(4.f, 4.f);
	bg_screen.setScale(2.f, 2.f);
	screen.setScale(4.f, 4.f);

	std::string str = "main";
	windows.insert({str, &window});

	Emulator emu;
	emu.init("tests/dmg-acid2.gb");

	//test_all_cpu_blargg(1, windows);

	uint32_t* tiles_framebuffer = (uint32_t*)malloc(sizeof(uint32_t) * 384 * 64); // 384 tiles each 64 pixels
	uint32_t* tile_map_9800 = (uint32_t*)malloc(sizeof(uint32_t) * 256 * 256); // 256px by 256 px for tilemap starting at 0x9800

	uint64_t frame_num = 0;
	while (window.isOpen() || tiles_window.isOpen()) //  || background_window.isOpen()
	{
		sf::Event event;
		sf::Event event2;
		while (window.isOpen() && window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				tiles_window.close();
				//background_window.close();
				break;
			}
		}

		while (tiles_window.isOpen() && tiles_window.pollEvent(event2))
		{
			if (event2.type == sf::Event::Closed)
			{
				tiles_window.close();
			}
		}

		handle_input(emu, event);
		
		int cycles_this_frame = 0;
		while (cycles_this_frame < CYCLES_PER_FRAME)
		{
			unsigned int cycles_took = emu.run(windows);
			if (cycles_took == 0) {
				throw std::runtime_error("Fuk");
			}
			cycles_this_frame += cycles_took;
		}

		emu.ppu.get_tiles_data(tiles_framebuffer, TILE_COLS, TILE_SIZE);
		emu.ppu.get_tile_map_data(tile_map_9800, 0X9C00);
		//emu.ppu.update_tile_atlas();
		//emu.ppu.update_bg_atlas();

		tiles_texture.update((uint8_t *)tiles_framebuffer);
		main_screen_texture.update((uint8_t *)emu.ppu.main_screen_framebuffer);
		bg_screen_texture.update((uint8_t *)tile_map_9800);

		window.clear();
		tiles_window.clear();
		//background_window.clear();
		tiles_window.draw(tiles_sprite);
		//background_window.draw(bg_screen);
		window.draw(screen);
		window.display();
		tiles_window.display();
		//background_window.display();
		frame_num++;
	}

	free(tiles_framebuffer);
	return 0;
}

void test_all_cpu_blargg(int test_num, std::map<std::string, sf::RenderWindow *> &windows)
{
	if (test_num > 11)
		return;

	Emulator emu;
	std::ostringstream fileName;
	fileName << "tests/" << std::setw(2) << std::setfill('0') << test_num << "-blargg.gb";

	emu.init(fileName.str());

	for (int i = 0; i < num_of_instructions[test_num - 1]; i++)
	{
		emu.run(windows);
	}
	std::cout << std::endl;
	test_all_cpu_blargg(test_num + 1, windows);
}

void handle_input(Emulator& emu, sf::Event event) {
	emu.set_input(START,  !sf::Keyboard::isKeyPressed(sf::Keyboard::Enter));
	emu.set_input(SELECT,  !sf::Keyboard::isKeyPressed(sf::Keyboard::Backspace));
    emu.set_input(A_BUTTON,  !sf::Keyboard::isKeyPressed(sf::Keyboard::Z));
    emu.set_input(B_BUTTON,  !sf::Keyboard::isKeyPressed(sf::Keyboard::X));
    emu.set_input(UP, !sf::Keyboard::isKeyPressed(sf::Keyboard::Up));
    emu.set_input(DOWN, !sf::Keyboard::isKeyPressed(sf::Keyboard::Down));
    emu.set_input(LEFT, !sf::Keyboard::isKeyPressed(sf::Keyboard::Right));
    emu.set_input(RIGHT, !sf::Keyboard::isKeyPressed(sf::Keyboard::Left));
}
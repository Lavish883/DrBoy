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

	sf::Texture bg_screen_texture;
	sf::Texture tiles_texture;
	sf::Texture main_screen_texture;

	main_screen_texture.create(SCREEN_WIDTH, SCREEN_HEIGHT);
	bg_screen_texture.create(SCREEN_WIDTH_WITH_HIDDEN, SCREEN_HEIGHT_WITH_HIDDEN);
	tiles_texture.create(ATLAS_WIDTH, ATLAS_HEIGHT);

	sf::Sprite bg_screen(bg_screen_texture);
	sf::Sprite screen(main_screen_texture);
	sf::Sprite tiles_sprite(tiles_texture);
	tiles_sprite.setScale(4.f, 4.f);
	bg_screen.setScale(4.f, 4.f);
	screen.setScale(4.f, 4.f);

	std::string str = "main";
	windows.insert({str, &window});

	Emulator emu;
	emu.init("roms/drMario.gb");

	// test_all_cpu_blargg(1, windows);

	while (window.isOpen() || tiles_window.isOpen())
	{
		sf::Event event;
		sf::Event event2;
		while (window.isOpen() && window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
				tiles_window.close();
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
			cycles_this_frame += cycles_took;
		}

		//emu.ppu.update_tile_atlas();
		//emu.ppu.update_bg_atlas();

		//tiles_texture.update((uint8_t *)emu.ppu.atlas_framebuffer.data());
		//bg_screen_texture.update((uint8_t *)emu.ppu.bg_pixels.data());
		main_screen_texture.update((uint8_t *)emu.ppu.main_screen_framebuffer);

		window.clear();
		tiles_window.clear();
		tiles_window.draw(tiles_sprite);
		window.draw(screen);
		window.display();
		tiles_window.display();
	}
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
#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int width = 800;
int height = 600;

sf::RenderWindow window(sf::VideoMode(800, 600), "Stars");
sf::Texture texture;

int load_file_with_nj(int argc, char *argv[])
{
	if (argc!=2) {
		std::cout << "Usage: " << argv[0] << " <imagefile.jpg>" << std::endl;
		return -1;
	}
	int width, height, channels;

	unsigned char* imageData = stbi_load(argv[1], &width, &height, &channels, 4);
	if (!imageData) {
		std::cerr << "Failed to load image: " << argv[1] << std::endl;
		return -1;
	}
	std::cout << "Image loaded successfully!" << std::endl;
	std::cout << "Width: " << width << std::endl;
	std::cout << "Height: " << height << std::endl;
	std::cout << "Channels: " << channels << std::endl;

	texture.create(width,height);
	texture.update(imageData);
	stbi_image_free(imageData);
	return 0;
}

int main(int argc, char *argv[])
{
	load_file_with_nj(argc, argv);
	srand(time(NULL));
	window.setFramerateLimit(60);
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized) {
				width=event.size.width;
				height=event.size.height;
				window.setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
			}
			else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		// clear the window with black color
		window.clear(sf::Color::Black);
    float s =(float)width/texture.getSize().x;
    if (s*texture.getSize().y>height) s = (float)height/texture.getSize().y;
		sf::Sprite sprite(texture);
    sprite.scale(s,s);
		window.draw(sprite);
		window.display();
	}
	return 0;
}

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include "ujpeg.h"
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>

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
  uJPEG jpeg;
  jpeg.decodeFile(argv[1]);
	texture.create(jpeg.getWidth(), jpeg.getHeight());
  std::cout << "width = " << jpeg.getWidth() << ", height = " << jpeg.getHeight() << std::endl;
  std::vector<unsigned char> rgba;
  rgba.reserve(jpeg.getImageSize()*4/3);
  std::for_each(jpeg.getImage(), jpeg.getImage()+jpeg.getImageSize(), [&](auto v) {static int count = 0; rgba.push_back(v);
      if (++count % 3 == 0) rgba.push_back('\xff'); });
	texture.update((unsigned char *)rgba.data());
  std::cout << "end of load_file_with_nj function " << std::endl;
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
    sf::Sprite sprite(texture);
	  window.draw(sprite);
		window.display();
	}
	return 0;
}

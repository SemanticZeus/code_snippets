#include <iostream>
#include <ctime>
#include <fstream>
#include <pigpiod_if2.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>

std::vector<std::vector<int>> half_step_seq {
	{1, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 1, 0},
	{0, 0, 1, 1},
	{0, 0, 0, 1},
	{1, 0, 0, 1}
};

std::vector<int> gpio_stepper_pins{17, 18, 27, 22};
int current_position = 0;
double step_angle = 360.0f/2048.0f/2;
int pi = pigpio_start(NULL, NULL);
void stepper(int pos) {
	std::cout << pos << std::endl;
	if (pos < 0 || pos > 360) return;
	if (pos > current_position) {
		int step_number = (pos-current_position)/step_angle;
		for (int i=0;i<step_number;i++) {
			int index = i % half_step_seq.size();
			auto &level = half_step_seq[index]; 
			for (int k=0;k<4;k++) gpio_write(pi, gpio_stepper_pins[k], level[k]);
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}
	} else {
		int step_number = (current_position-pos)/step_angle;
		for (int i=0;i<step_number;i++) {
			int index = i % half_step_seq.size();
			index = half_step_seq.size() - index-1;
			auto &level = half_step_seq[index]; 
			for (int k=0;k<4;k++) gpio_write(pi, gpio_stepper_pins[k], level[k]);
			std::this_thread::sleep_for(std::chrono::milliseconds(2));
		}


	}
	for (int k=0;k<4;k++) gpio_write(pi, gpio_stepper_pins[k], 0);
	current_position = pos;
}

int main() {
	stepper(360);
	stepper(0);
	return 0;
}

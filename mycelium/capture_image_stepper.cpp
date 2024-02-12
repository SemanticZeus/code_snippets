#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <fstream>
#include <pigpiod_if2.h>
#include <sstream>
#include <chrono>
#include <thread>
#include <vector>

double map_(double v, double start, double end, double new_start, double new_end)
{
	return (v-start)*(new_end-new_start)/(end-start) + new_start;
}

using namespace cv;
using namespace std;

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
	VideoCapture cap(0);
	cap.set(CAP_PROP_FRAME_WIDTH, 2592);
	cap.set(CAP_PROP_FRAME_HEIGHT, 1944);
	if (!cap.isOpened()) {
		cerr << "Error: Unable to open the camera." << endl;
		return -1;
	}

	time_t now = time(0);
	tm *ltm = localtime(&now);
	std::vector<int> angles;
	for (int i=0; i<=180; i+=20) angles.push_back(i);
	for (int i=0; i<angles.size(); i++) {
		int angle = angles[i];
		char filename[100];
		sprintf(filename, "/home/pi/mycelium/image-%04d-%02d-%02d-%02d-%02d-%02d-angle-%d.png",
		        ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday, ltm->tm_hour, ltm->tm_min, ltm->tm_sec, angle);

		Mat frame;
		// Capture frame-by-frame
		stepper(angle);
		cap >> frame;
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		// Check if the frame is empty
		if (frame.empty()) {
			cerr << "Error: Captured frame is empty." << endl;
			return -1;
		}

		// Save the frame as an image
		imwrite(filename, frame);

		cout << "Image saved as: " << filename << endl;
	}

	// Release the camera capture
	cap.release();
	stepper(0);
	return 0;
}

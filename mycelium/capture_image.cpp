#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <fstream>
#include <pigpiod_if2.h>
#include <sstream>
#include <chrono>
#include <thread>

double map_(double v, double start, double end, double new_start, double new_end)
{
	return (v-start)*(new_end-new_start)/(end-start) + new_start;
}

using namespace cv;
using namespace std;

void servo(int pos)
{
	int pwm_frequency = 50;
	int gpio_pin = 18;
	int pi = pigpio_start(NULL, NULL);
	set_mode(pi, gpio_pin, PI_OUTPUT);
	set_PWM_frequency(pi, gpio_pin, 50);
	double k = map_(pos, 0, 180, 255.0*50*.6/1000.0, 255.0*2.4*50/1000.0);
	set_PWM_dutycycle(pi, gpio_pin, k);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	pigpio_stop(pi);
}

void stop_servo()
{
	int pwm_frequency = 50;
	int gpio_pin = 18;
	int pi = pigpio_start(NULL, NULL);
	set_mode(pi, gpio_pin, PI_OUTPUT);
	set_PWM_frequency(pi, gpio_pin, 50);
	set_PWM_dutycycle(pi, gpio_pin, 0);
	pigpio_stop(pi);

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
		servo(angle);
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
	stop_servo();
	return 0;
}

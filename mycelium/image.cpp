#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>
#include <fstream>

using namespace cv;
using namespace std;

int main() {
    VideoCapture cap(0);
    cap.set(CAP_PROP_FRAME_WIDTH, 1920);
    cap.set(CAP_PROP_FRAME_HEIGHT, 1080);
    if (!cap.isOpened()) {
        cerr << "Error: Unable to open the camera." << endl;
        return -1;
    }

    time_t now = time(0);
    tm *ltm = localtime(&now);
    char filename[100];
    sprintf(filename, "/home/pi/mycelium/image-%04d-%02d-%02d-%02d-%02d-%02d.png", ltm->tm_year + 1900, ltm->tm_mon + 1, ltm->tm_mday,
            ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

    Mat frame;
    // Capture frame-by-frame
    cap >> frame;

    // Check if the frame is empty
    if (frame.empty()) {
        cerr << "Error: Captured frame is empty." << endl;
        return -1;
    }

    // Save the frame as an image
    imwrite(filename, frame);

    cout << "Image saved as: " << filename << endl;

    // Release the camera capture
    cap.release();

    return 0;
}

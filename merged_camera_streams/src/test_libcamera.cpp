#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace std;

int main() {
    cout << "Testing OpenCV backends..." << endl;

    cout << "Available backends: " << endl;
    cout << "CAP_V4L2: " << cv::CAP_V4L2 << endl;
    cout << "CAP_LIBCAMERA: " << cv::CAP_LIBCAMERA << endl;

    // Try libcamera backend
    cv::VideoCapture cap(cv::CAP_LIBCAMERA);
    if (cap.open(0)) {
        cout << "Successfully opened camera with CAP_LIBCAMERA" << endl;

        cv::Mat frame;
        if (cap.read(frame)) {
            cout << "Successfully captured frame: " << frame.cols << "x" << frame.rows << endl;
        } else {
            cout << "Failed to capture frame with CAP_LIBCAMERA" << endl;
        }
        cap.release();
    } else {
        cout << "Failed to open camera with CAP_LIBCAMERA" << endl;
    }

    cout << "Test completed" << endl;
    return 0;
}
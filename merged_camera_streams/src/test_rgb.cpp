#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

using namespace std;

int main() {
    cout << "Testing RGB camera in isolation..." << endl;

    cv::VideoCapture cap;
    vector<string> camera_paths = {"/dev/video0", "/dev/video1", "/dev/video2", "/dev/video3"};

    for (const auto& path : camera_paths) {
        cap.open(path, cv::CAP_V4L2);
        if (cap.isOpened()) {
            cout << "Successfully opened camera at: " << path << endl;
            break;
        }
    }

    if (!cap.isOpened()) {
        cerr << "Failed to open any camera" << endl;
        return -1;
    }

    // Try different resolutions
    vector<pair<int, int>> resolutions = {{640, 480}, {1280, 720}, {1920, 1080}};
    for (auto& res : resolutions) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, res.first);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, res.second);

        double actual_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
        double actual_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

        cout << "Trying resolution: " << res.first << "x" << res.second
             << " -> got " << actual_width << "x" << actual_height << endl;

        // Try to capture a few frames
        bool success = false;
        for (int i = 0; i < 5; ++i) {
            cv::Mat frame;
            if (cap.read(frame)) {
                cout << "Successfully captured frame " << i << ": " << frame.cols << "x" << frame.rows << endl;
                success = true;
                break;
            } else {
                cout << "Failed to capture frame " << i << endl;
            }
        }

        if (success) {
            cout << "RGB camera works at " << actual_width << "x" << actual_height << endl;
            break;
        }
    }

    cap.release();
    cout << "Test completed" << endl;
    return 0;
}
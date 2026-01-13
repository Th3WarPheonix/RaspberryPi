#include <opencv2/opencv.hpp>


int main() {
    // Open the default camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera" << std::endl;
        return -1;
    }

    // Load the overlay image
    cv::Mat overlay = cv::imread("overlay.png", cv::IMREAD_UNCHANGED);
    if (overlay.empty()) {
        std::cerr << "Error: Could not load overlay image" << std::endl;
        return -1;
    }

    // Create a window to display the results
    cv::namedWindow("Camera Feed", cv::WINDOW_AUTOSIZE);

    while (true) {
        cv::Mat frame;
        cap >> frame; // Capture a new frame from the camera

        if (frame.empty()) {
            std::cerr << "Error: Could not grab a frame" << std::endl;
            break;
        }

        // Resize overlay to match the frame size
        cv::Mat resizedOverlay;
        cv::resize(overlay, resizedOverlay, frame.size());

        // Create a mask of the overlay
        cv::Mat mask;
        cv::cvtColor(resizedOverlay, mask, cv::COLOR_BGR2GRAY);
        cv::threshold(mask, mask, 1, 255, cv::THRESH_BINARY);

        // Create the inverse mask
        cv::Mat inverseMask;
        cv::bitwise_not(mask, inverseMask);

        // Black-out the area of the overlay in the frame
        cv::Mat frameBackground;
        cv::bitwise_and(frame, frame, frameBackground, inverseMask);

        // Take only the region of the overlay from the overlay image
        cv::Mat overlayForeground;
        cv::bitwise_and(resizedOverlay, resizedOverlay, overlayForeground, mask);

        // Add the overlay to the frame
        cv::Mat combined;
        cv::add(frameBackground, overlayForeground, combined);

        // Display the resulting frame
        cv::imshow("Camera Feed", combined);

        // Break the loop if the user presses 'q'
        if (cv::waitKey(30) >= 0) {
            break;
        }
    }

    // Release the camera and close all OpenCV windows
    cap.release();
    cv::destroyAllWindows();

    return 0;
}
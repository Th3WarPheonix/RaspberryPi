// merged_display.cpp - Side-by-side RGB and Depth Camera Display
//
// This program demonstrates how to capture and display frames from two cameras
// simultaneously on Raspberry Pi 5: an RGB camera (imx708) and a depth camera (Arducam).
//
// KEY TECHNICAL ISSUES ADDRESSED:
//
// 1. OpenCV V4L2 Backend Incompatibility:
//    - Raspberry Pi 5 uses libcamera instead of legacy V4L2 for camera access
//    - OpenCV's CAP_V4L2 backend expects direct V4L2 ioctl access to camera hardware
//    - libcamera provides a different API that wraps V4L2, causing capture failures
//    - SOLUTION: Use rpicam-vid (libcamera tool) to capture MJPEG streams, then decode with OpenCV
//
// 2. CSI Interface Conflicts:
//    - Raspberry Pi 5 has two CSI interfaces: CSI0 and CSI1
//    - Each CSI interface can only handle ONE camera simultaneously
//    - RGB camera (imx708) uses CSI0 (configured as cam0 in /boot/firmware/config.txt)
//    - Depth camera (Arducam) uses CSI1 (configured as cam1 in /boot/firmware/config.txt)
//    - If both cameras tried to use the same CSI interface, they would conflict
//    - SOLUTION: Ensure cameras are on different CSI interfaces (cam0 vs cam1)
//
// Required config.txt entries:
//   camera_auto_detect=0
//   dtoverlay=imx708,cam0        # RGB camera on CSI0
//   dtoverlay=arducam-pivariety,cam1  # Depth camera on CSI1
//
// Build with: make merged_display
// Run with: ./build/merged_display

#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <vector>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "ArducamTOFCamera.hpp"

using namespace std;
using namespace Arducam;

// Thread-safe frame buffers
cv::Mat rgb_frame, depth_frame;
mutex rgb_mutex, depth_mutex;
atomic<bool> rgb_ready(false), depth_ready(false);
atomic<bool> running(true);

// RGB Camera Thread (Camera Module 3 - Using rpicam-vid)
// NOTE: OpenCV's V4L2 backend doesn't work with Raspberry Pi 5's camera stack!
// Raspberry Pi 5 uses libcamera instead of the legacy V4L2 interface that OpenCV expects.
// The V4L2 backend can open the camera device but fails to capture frames because the
// underlying camera stack has changed. We work around this by using rpicam-vid (libcamera)
// to capture MJPEG streams and decode them with OpenCV instead.
// v4l2-ctl --device=/dev/video0 --all showed the camera was configured correctly
// v4l2-ctl --device=/dev/video0 --stream-mmap failed with "Invalid argument"
// rpicam-still --camera 0 worked perfectly
void camera3_thread_func() {
    try {
        cout << "Starting RGB camera thread with rpicam-vid..." << endl;

        // Use rpicam-vid to capture MJPEG stream from camera 0
        // --camera 0: Use the first camera (imx708 on CSI0/cam0)
        // --codec mjpeg: Output MJPEG compressed frames for efficient streaming
        // --output -: Send output to stdout so we can read it from our C++ program
        // --nopreview: Don't show preview window (we handle display ourselves)
        // --timeout 0: Run indefinitely until terminated
        string cmd = "rpicam-vid --camera 0 --width 640 --height 480 --codec mjpeg --output - --nopreview --timeout 0";
        FILE* pipe = popen(cmd.c_str(), "r");
        
        if (!pipe) {
            cerr << "Failed to start rpicam-vid process" << endl;
            return;
        }

        vector<uchar> buffer;
        int rgb_frame_count = 0;
        
        while (running) {
            // Read MJPEG data from pipe
            // MJPEG streams contain multiple JPEG frames back-to-back
            // We need to parse the stream to extract individual JPEG frames
            buffer.clear();
            bool found_jpeg = false;
            
            // Look for JPEG SOI (Start of Image) marker (0xFF 0xD8)
            // This marks the beginning of each JPEG frame in the MJPEG stream
            while (!found_jpeg && running) {
                int c1 = fgetc(pipe);
                if (c1 == EOF) break;
                
                if (c1 == 0xFF) {
                    int c2 = fgetc(pipe);
                    if (c2 == EOF) break;
                    
                    if (c2 == 0xD8) {
                        // Found JPEG start - include the marker in our buffer
                        buffer.push_back(0xFF);
                        buffer.push_back(0xD8);
                        found_jpeg = true;
                    } else {
                        // Not a JPEG marker, continue scanning
                        continue;
                    }
                }
            }
            
            if (!found_jpeg) {
                cerr << "Failed to find JPEG marker" << endl;
                break;
            }
            
            // Read until JPEG EOI (End of Image) marker (0xFF 0xD9)
            // This marks the end of the current JPEG frame
            bool found_end = false;
            while (!found_end && running) {
                int c1 = fgetc(pipe);
                if (c1 == EOF) break;
                
                buffer.push_back(c1);
                
                if (c1 == 0xFF) {
                    int c2 = fgetc(pipe);
                    if (c2 == EOF) break;
                    
                    buffer.push_back(c2);
                    
                    if (c2 == 0xD9) {
                        // Found JPEG end marker
                        found_end = true;
                    }
                }
            }
            
            if (!found_end) {
                cerr << "Failed to find JPEG end marker" << endl;
                break;
            }
            
            // Decode the complete JPEG frame using OpenCV
            cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
            if (frame.empty()) {
                cerr << "Failed to decode JPEG frame" << endl;
                continue;
            }

            // Add timestamp and frame info
            string timestamp = "Frame: " + to_string(rgb_frame_count);
            cv::putText(frame, "Camera Module 3", cv::Point(10, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(255, 255, 255), 2);
            cv::putText(frame, timestamp, cv::Point(10, 60),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 2);

            {
                lock_guard<mutex> lock(rgb_mutex);
                rgb_frame = frame.clone();
                rgb_ready = true;
            }

            // Debug output every 30 frames
            if (++rgb_frame_count % 30 == 0) {
                cout << "RGB Camera: captured " << rgb_frame_count << " frames" << endl;
            }

            // Small delay
            this_thread::sleep_for(chrono::milliseconds(10));
        }

        pclose(pipe);
        cout << "RGB Camera thread stopped" << endl;

    } catch (const exception &e) {
        cerr << "RGB Camera error: " << e.what() << endl;
    }
}

// Depth Camera Thread (Arducam)
// NOTE: Camera CSI Interface Conflicts!
// Raspberry Pi 5 has two CSI interfaces: CSI0 and CSI1
// Each CSI interface can only handle ONE camera at a time
// - CSI0 (/dev/media1): RGB camera (imx708) configured as cam0
// - CSI1 (/dev/media2): Depth camera (Arducam) configured as cam1
// If both cameras tried to use the same CSI interface, they would conflict
// and neither would work properly. The Arducam SDK uses CSI index 8 for cam1.
void depth_camera_thread_func() {
    try {
        ArducamTOFCamera tof;
        
        // Open camera via CSI connection - cam1 for arducam-pivariety
        // Try different camera indices - cam1 might be index 1, not 8
        // CSI index 8 corresponds to cam1 on CSI1 interface
        bool depth_opened = false;
        vector<int> cam_indices = {1, 8, 0, 2, 3, 4, 5, 6, 7, 9, 10};
        
        for (int cam_index : cam_indices) {
            if (tof.open(Arducam::Connection::CSI, cam_index) == 0) {
                cout << "Depth camera opened successfully on CSI index: " << cam_index << endl;
                depth_opened = true;
                break;
            }
        }
        
        if (!depth_opened) {
            cerr << "Failed to open depth camera on any CSI index" << endl;
            return;
        }

        // Start camera with depth frame type
        if (tof.start(Arducam::FrameType::DEPTH_FRAME)) {
            cerr << "Failed to start depth camera" << endl;
            return;
        }

        // Get camera info
        auto info = tof.getCameraInfo();
        cout << "Depth camera info: " << info.width << "x" << info.height << endl;

        int depth_frame_count = 0;
        while (running) {
            ArducamFrameBuffer *frame = tof.requestFrame(2000);
            if (!frame) {
                cerr << "Failed to get depth frame" << endl;
                continue;
            }

            Arducam::FrameFormat format;
            frame->getFormat(Arducam::FrameType::DEPTH_FRAME, format);

            float *depth_ptr = (float *)frame->getData(Arducam::FrameType::DEPTH_FRAME);
            if (depth_ptr) {
                // Convert depth data to OpenCV Mat (32-bit float)
                cv::Mat depth_raw(format.height, format.width, CV_32F, depth_ptr);

                // Normalize and convert to 8-bit for display
                cv::Mat depth_display;
                cv::normalize(depth_raw, depth_display, 0, 255, cv::NORM_MINMAX, CV_8UC1);

                // Apply colormap for better visualization
                cv::Mat depth_colored;
                cv::applyColorMap(depth_display, depth_colored, cv::COLORMAP_TURBO);

                {
                    lock_guard<mutex> lock(depth_mutex);
                    depth_frame = depth_colored.clone();
                    depth_ready = true;
                }

                // Debug output every 30 frames
                if (++depth_frame_count % 30 == 0) {
                    cout << "Depth thread: produced frame " << depth_frame_count << endl;
                }
            }

            tof.releaseFrame(frame);
        }

        tof.close();

    } catch (const exception &e) {
        cerr << "Depth Camera error: " << e.what() << endl;
    }
}

// Set up cameras as follows
// /boot/firmware/config.txt
// dtparam=i2c_arm=on
// camera_auto_detect=0
// dtoverlay=imx708,cam0           # RGB camera -> CSI0 interface
// dtoverlay=arducam-pivariety,cam1 # Depth camera -> CSI1 interface
//
// cameras should be plugged into the corresponding CSI ports
// reboot if changes were made
//
// CSI Interface Mapping:
// - cam0 (imx708 RGB): CSI0 -> /dev/media1 -> rpicam-vid --camera 0
// - cam1 (Arducam depth): CSI1 -> /dev/media2 -> Arducam SDK CSI index 8
//
// The key insight: CSI0 and CSI1 are separate interfaces that can work simultaneously,
// but each interface can only handle one camera. This prevents conflicts between cameras.
int main() {
    cout << "Starting merged camera display..." << endl;

    // Start camera threads
    thread rgb_thread(camera3_thread_func);
    thread depth_thread(depth_camera_thread_func);

    // Give cameras time to initialize
    this_thread::sleep_for(chrono::seconds(2));

    cout << "Starting display loop..." << endl;

    // Display loop
    int frame_count = 0;
    while (running) {
        cv::Mat combined_frame;
        cv::Mat current_rgb, current_depth;

        // Get RGB frame
        {
            lock_guard<mutex> lock(rgb_mutex);
            if (rgb_ready) {
                current_rgb = rgb_frame.clone();
            }
        }

        // Get Depth frame
        {
            lock_guard<mutex> lock(depth_mutex);
            if (depth_ready) {
                current_depth = depth_frame.clone();
            }
        }

        // Debug output every 30 frames (~1 second at 30fps)
        if (++frame_count % 30 == 0) {
            cout << "Frame " << frame_count << ": RGB=" << (!current_rgb.empty() ? "YES" : "NO")
                 << " Depth=" << (!current_depth.empty() ? "YES" : "NO") << endl;
        }

        if (!current_rgb.empty() && !current_depth.empty()) {
            // Both cameras ready - combine them
            // Use the smaller height as target to avoid distortion
            int target_height = min(current_rgb.rows, current_depth.rows);
            if (target_height > 480) target_height = 480; // Cap at 480 for display

            cv::Mat rgb_resized, depth_resized;

            if (current_rgb.rows != target_height) {
                double scale = (double)target_height / current_rgb.rows;
                int new_width = (int)(current_rgb.cols * scale);
                cv::resize(current_rgb, rgb_resized, cv::Size(new_width, target_height));
            } else {
                rgb_resized = current_rgb;
            }

            if (current_depth.rows != target_height) {
                double scale = (double)target_height / current_depth.rows;
                int new_width = (int)(current_depth.cols * scale);
                cv::resize(current_depth, depth_resized, cv::Size(new_width, target_height));
            } else {
                depth_resized = current_depth;
            }

            // Combine horizontally
            cv::hconcat(rgb_resized, depth_resized, combined_frame);

            // Add labels
            cv::putText(combined_frame, "RGB Camera", cv::Point(20, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
            cv::putText(combined_frame, "Depth Camera", cv::Point(rgb_resized.cols + 20, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);

            cv::imshow("Merged Camera Streams", combined_frame);
        } else if (!current_rgb.empty()) {
            // Only RGB ready
            cv::putText(current_rgb, "RGB Camera Only", cv::Point(20, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
            cv::imshow("Merged Camera Streams", current_rgb);
        } else if (!current_depth.empty()) {
            // Only depth ready
            cv::putText(current_depth, "Depth Camera Only", cv::Point(20, 30),
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
            cv::imshow("Merged Camera Streams", current_depth);
        } else {
            // No frames ready yet
            cv::Mat waiting_frame(480, 1280, CV_8UC3, cv::Scalar(50, 50, 50));
            cv::putText(waiting_frame, "Waiting for camera frames...", cv::Point(400, 240),
                       cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::imshow("Merged Camera Streams", waiting_frame);
        }

        int key = cv::waitKey(1);
        if (key == 'q' || key == 27) { // 'q' or ESC
            running = false;
            break;
        }
    }

    cout << "Shutting down..." << endl;
    running = false;

    rgb_thread.join();
    depth_thread.join();

    cv::destroyAllWindows();
    cout << "Done!" << endl;

    return 0;
}

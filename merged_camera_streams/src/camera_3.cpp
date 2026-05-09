#include <iostream>
#include <memory>
#include <libcamera/libcamera.h>
#include <libcamera/controls.h>
#include <chrono>
#include <thread>
#include <fstream>

using namespace std;
using namespace libcamera;

class CameraApp {
private:
    // unique ptr - smart pointer that manages the lifetime of a dynamically allocated object
    // shared_ptr - freeing the memory only when the last shared_ptr referencing it is destroyed or reset
    
    unique_ptr<CameraManager> cameraManager;
    shared_ptr<Camera> camera;
    unique_ptr<CameraConfiguration> config;
    unique_ptr<FrameBufferAllocator> allocator;


public:
    CameraApp() : cameraManager(make_unique<CameraManager>()) {
        if (cameraManager->start()) {
            cerr << "Failed to start camera manager" << endl;
            return;
        }

        // Get all devices recognized as cameras
        auto cameras = cameraManager->cameras();
        if (cameras.empty()) {
            cerr << "No cameras found" << endl;
            return;
        }

        camera = cameras[0]; // select the first camera
        cout << "Camera opened: " << camera->id() << endl;

        // Once a camera has been selected an application 
        // needs to acquire an exclusive lock to it so 
        // no other application can use it
        if (camera->acquire()) {
            cerr << "Failed to acquire camera" << endl;
            return;
        }
    }

    ~CameraApp() {
        if (camera) {
            camera->release();
        }
        if (cameraManager) {
            cameraManager->stop();
        }
    }

    bool configure() {
        if (!camera) return false;

        // Generate configuration for video recording
        config = camera->generateConfiguration({StreamRole::VideoRecording});
        if (!config) {
            cerr << "Failed to generate configuration" << endl;
            // The Camera::generateConfiguration() function accepts a 
            // list of desired roles and generates a CameraConfiguration 
            // with the best stream parameters configuration for each of the 
            // requested roles. If the camera can handle the requested roles, 
            // it returns an initialized CameraConfiguration and a null 
            // pointer if it can't.
            return false;
        }

        // Set video resolution to 1080p
        StreamConfiguration &streamConfig = config->at(0);
        streamConfig.pixelFormat = formats::YUV420;
        streamConfig.size = {1920, 1080};
        streamConfig.bufferCount = 4;

        // Validate and apply configuration
        config->validate(); // must be performed before applying to camera
        if (camera->configure(config.get()) < 0) {
            cerr << "Failed to configure camera" << endl;
            return false;
        }

        cout << "Camera configured successfully!" << endl;
        cout << "Resolution: " << streamConfig.size.width << "x" 
             << streamConfig.size.height << endl;
        cout << "Format: " << streamConfig.pixelFormat << endl;

        return true;
    }

    void setFocus(float position) {
        if (!camera) return;

        ControlList *controls = new ControlList(camera->controls());
        controls->set(controls::AfMode, 0); // Manual focus mode
        controls->set(controls::LensPosition, position);

        int ret = camera->start(controls);
        if (ret < 0) {
            cerr << "Failed to set focus" << endl;
        } else {
            cout << "Focus set to: " << position << endl;
        }
        delete controls;
    }

    bool record(int duration_seconds = 30) {
        if (!camera) return false;

        cout << "Recording for " << duration_seconds << " seconds..." << endl;
        cout << "Output: test.h264" << endl;

        // Allocate frame buffers
        allocator = make_unique<FrameBufferAllocator>(camera);
        for (StreamConfiguration &cfg : *config) {
            if (allocator->allocate(cfg.stream()) < 0) {
                cerr << "Frame buffer allocation failed" << endl;
                return false;
            }
        }

        // Start camera
        if (camera->start() < 0) {
            cerr << "Failed to start camera" << endl;
            return false;
        }

        // Open output file for H.264 data
        ofstream output("test.h264", ios::binary);
        if (!output.is_open()) {
            cerr << "Failed to open output file" << endl;
            return false;
        }

        auto start_time = chrono::steady_clock::now();
        int frame_count = 0;

        // Simulate recording loop with focus changes
        while (true) {
            auto current_time = chrono::steady_clock::now();
            auto elapsed = chrono::duration_cast<chrono::seconds>(
                current_time - start_time).count();

            // Simulate focus adjustments like in Python version
            // if (elapsed == 2) {
            //     setFocus(2.0f);
            // } else if (elapsed == 4) {
            //     setFocus(4.0f);
            // } else if (elapsed == 6) {
            //     setFocus(6.0f);
            // }

            frame_count++;
            if (frame_count % 30 == 0) {
                cout << "Frames: " << frame_count 
                     << " (Elapsed: " << elapsed << "s)" << endl;
            }

            if (elapsed >= duration_seconds) {
                cout << "Recording complete" << endl;
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(33)); // ~30fps
        }

        camera->stop();
        output.close();

        cout << "Total frames recorded: " << frame_count << endl;
        cout << "Video saved to: test.h264" << endl;

        return true;
    }
};

int main() {
    cout << "Camera Module 3 - libcamera C++ Application" << endl;
    cout << "==========================================" << endl;

    CameraApp app;

    if (!app.configure()) {
        cerr << "Configuration failed" << endl;
        return -1;
    }

    if (!app.record(10)) {
        cerr << "Recording failed" << endl;
        return -1;
    }

    return 0;
}

import time
from picamera2 import Picamera2, Preview
from picamera2.encoders import H264Encoder, Quality

picam2 = Picamera2()

# 3. Create a video configuration (e.g., 1080p main, 480p lores for preview)
video_config = picam2.create_video_configuration(main={"size": (1920, 1080)})
picam2.configure(video_config)

# 4. Start the camera and preview (optional but recommended)
# picam2.start()
picam2.start_preview(Preview.QTGL) # Uncomment if you want a preview window

# 5. Set up encoder and start recording
encoder = H264Encoder(bitrate=10000000) # Set bitrate and quality
output_file = "test.h264" # Or .mp4 if using MP4Encoder
picam2.start_recording(encoder, output_file)
 
# 6. Record for a set duration (e.g., 10 seconds)
print("Recording...")

# Change focus positions during recording
# # AfMode Set the AF mode (manual, auto, continuous)
# Set AfMode to Manual (0)
# LensPosition for fixed focus where 2.0 focuses at 0.5 meters (1/2.0 diopters)
time.sleep(2)
picam2.set_controls({"AfMode": 0, "LensPosition": 2.0})
print('Change focus to 2')

time.sleep(2)
picam2.set_controls({"AfMode": 0, "LensPosition": 4.0})
print('Change focus to 4')

time.sleep(2)
picam2.set_controls({"AfMode": 0, "LensPosition": 6.0})
print('Change focus to 6')

time.sleep(10) # Keep recording for 10 seconds

# 7. Stop recording and clean up
# picam2.stop_recording()
picam2.stop_preview() # Stop preview if started
picam2.stop() # Stop the camera

print(f"Video saved to {output_file}")
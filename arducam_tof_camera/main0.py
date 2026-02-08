import cv2
import numpy as np
import ArducamDepthCamera as ac
import matplotlib.pyplot as plt
from matplotlib.widgets import Slider, Button
from scipy import ndimage, stats
import open3d

# MAX_DISTANCE value modifiable is 2000 or 4000 mm
MAX_DISTANCE = 4000
CONFIDENCE_THRESHOLD = 30

def getPreviewRGB(preview: np.ndarray, confidence: np.ndarray) -> np.ndarray:
    preview = np.nan_to_num(preview)
    preview[confidence < CONFIDENCE_THRESHOLD] = (0, 0, 0)
    return preview

def usage(argv0):
    print("Usage: python " + argv0 + " [options]")
    print("Available options are:")
    print(" -d        Choose the video to use")

def main():
    print("Arducam Depth Camera Demo.")
    print("  SDK version:", ac.__version__)

    cam = ac.ArducamCamera()
    cfg_path = None
    # cfg_path = "file.cfg"

    black_color = (0, 0, 0)
    white_color = (255, 255, 255)

    ret = 0
    if cfg_path is not None:
        ret = cam.openWithFile(cfg_path, 0)
    else:
        ret = cam.open(ac.Connection.CSI, 0)
    if ret != 0:
        print("Failed to open camera. Error code:", ret)
        return

    ret = cam.start(ac.FrameType.DEPTH)
    if ret != 0:
        print("Failed to start camera. Error code:", ret)
        cam.close()
        return

    cam.setControl(ac.Control.RANGE, MAX_DISTANCE)

    info = cam.getCameraInfo()
    print(f"Camera resolution: {info.width}x{info.height}")
    
    for i in range(1):
        frame = cam.requestFrame(2000)
        if frame is not None and isinstance(frame, ac.DepthData):
            depth_buf = frame.depth_data

            x = np.linspace(0, depth_buf.shape[1]-1, depth_buf.shape[1])
            y = np.linspace(0, depth_buf.shape[0]-1, depth_buf.shape[0])
            X, Y = np.meshgrid(x, y)

            X = X[depth_buf < 304.8]
            Y = Y[depth_buf < 304.8]
            depth_obj = depth_buf[depth_buf < 304.8]
            filtered_image = ndimage.median_filter(depth_obj, size=3)

            depth = depth_obj.astype(np.float32)
    
            # Calculate gradients in x and y directions
            dx = cv2.Sobel(depth, cv2.CV_32F, 1, 0, ksize=3)
            dy = cv2.Sobel(depth, cv2.CV_32F, 0, 1, ksize=3)
            
            # Calculate magnitude of gradient
            gradient_mag = cv2.magnitude(dx, dy)
            a = gradient_mag.reshape(gradient_mag.shape[0])
            a_median = np.median(a)
            a_std = np.std(a)

            z = stats.zscore(a)
            zgt3 = z > 2 # greater than 3 is outlier in guassian distribution
            az3 = a[~zgt3]
            fi = depth_obj[~zgt3]
            Xz3 = X[~zgt3]
            Yz3 = Y[~zgt3]

            fig = plt.figure()
            ax = fig.add_subplot(111, projection='3d')

            # n = 1
            # # ax.scatter(X, Y, c=depth_obj, marker='.', cmap='jet', s=1)
            # # t = ax.scatter(X[::n], Y[::n], filtered_image[::n], c=filtered_image[::n], cmap='jet', marker='.', s=1)
            t = ax.scatter(Xz3, Yz3, fi, c=fi, cmap='jet', marker='.', s=1)

            # plt.colorbar(t, label='Depth (mm)')
            # c_min = 0
            # c_max = MAX_DISTANCE

            # ax_cmin = plt.axes([0.25, 0.1, 0.65, 0.03])
            # ax_cmax  = plt.axes([0.25, 0.15, 0.65, 0.03])

            # s_cmin = Slider(ax_cmin, 'min', np.min(filtered_image), np.max(filtered_image), valinit=c_min)
            # s_cmax = Slider(ax_cmax, 'max', np.min(filtered_image), np.max(filtered_image), valinit=c_max)

            # def update(val, s=None):
            #     _cmin = s_cmin.val
            #     _cmax = s_cmax.val
            #     t.set_clim([_cmin, _cmax])
            #     plt.draw()

            # s_cmin.on_changed(update)
            # s_cmax.on_changed(update)


            # ax.set_xlabel('X Pixel')
            # ax.set_ylabel('Y Pixel')
            # ax.set_zlabel('Depth (mm)')
            # ax.set_aspect('equal')
            # plt.savefig("depth_contour.png")
            plt.show()
            plt.close()

            cam.releaseFrame(frame)

    cam.stop()
    cam.close()


if __name__ == "__main__":
    main()
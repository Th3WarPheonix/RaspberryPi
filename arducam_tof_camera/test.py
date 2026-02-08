import open3d as o3d
import numpy as np
import matplotlib.pyplot as plt
import os
import sys
import cv2
import ArducamDepthCamera as ac

def setup_camera():
    # MAX_DISTANCE value modifiable  is 2000 or 4000
    MAX_DISTANCE = 4000

    cam = ac.ArducamCamera()
    cfg_path = None
    # cfg_path = "file.cfg"

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
    
    return cam

def main():
    cam = setup_camera()
    if cam is None:
        return
    
    fx = cam.getControl(ac.Control.INTRINSIC_FX)/100
    fy = cam.getControl(ac.Control.INTRINSIC_FY)/100
    cx = cam.getControl(ac.Control.INTRINSIC_CX)/100
    cy = cam.getControl(ac.Control.INTRINSIC_CY)/100
    height = cam.getControl(ac.Control.FMT_HEIGHT)
    width = cam.getControl(ac.Control.FMT_WIDTH)

    camera_intrinsic_o3d = o3d.camera.PinholeCameraIntrinsic(
        width=width, height=height, fx=fx,fy=fy, cx=cx, cy=cy)
    print(camera_intrinsic_o3d.intrinsic_matrix)

    # Load in color and depth image to create the point cloud
    depth_raw = o3d.io.read_image("saved_image.png")
    o3d.geometry.Image(np.asarray(depth_raw))
    d2 = cv2.imread('saved_image.png')
    rgbd_image = o3d.geometry.PointCloud.create_from_depth_image(depth_raw, camera_intrinsic_o3d) 
    print(rgbd_image)

    # Plot the images
    plt.subplot(1, 2, 1)
    plt.title('Grayscale image')
    plt.imshow(rgbd_image.color)
    plt.subplot(1, 2, 2)
    plt.title('Depth image')
    plt.imshow(rgbd_image.depth)
    plt.show()

    # # Camera intrinsic parameters built into Open3D for Prime Sense
    # camera_intrinsic = o3d.camera.PinholeCameraIntrinsic(
    #         o3d.camera.PinholeCameraIntrinsicParameters.PrimeSenseDefault)

    # # Create the point cloud from images and camera intrisic parameters
    # pcd = o3d.geometry.PointCloud.create_from_rgbd_image(rgbd_image, camera_intrinsic)
        
    # # Flip it, otherwise the pointcloud will be upside down
    # pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])
    # o3d.visualization.draw_geometries([pcd], zoom=0.5)

    # # Camera intrinsic parameters from camera used to get color and depth images - Camera Calibration
    # cv_file = cv2.FileStorage()
    # cv_file.open('C:/Users/nhoei/ComputerVision/monocularDepth/cameraIntrinsic.xml', cv2.FileStorage_READ)

    # camera_intrinsic = cv_file.getNode('intrinsic').mat()
    # print(camera_intrinsic)



    # # Create the point cloud from images and camera intrisic parameters
    # pcd = o3d.geometry.PointCloud.create_from_rgbd_image(rgbd_image, camera_intrinsic_o3d)
        
    # # Flip it, otherwise the pointcloud will be upside down
    # pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])
    # o3d.visualization.draw_geometries([pcd], zoom=0.5)

    # # Load in the point cloud created from OpenCV to compared to Open3D
    # opencv_pcd_path = "C:/Users/nhoei/ComputerVision/monocularDepth/reconstructedMono.ply"
    # pcd = o3d.io.read_point_cloud(opencv_pcd_path)

    # # Flip it, otherwise the pointcloud will be upside down
    # pcd.transform([[1, 0, 0, 0], [0, -1, 0, 0], [0, 0, -1, 0], [0, 0, 0, 1]])
    # o3d.visualization.draw_geometries([pcd], zoom=0.5)

if __name__ == "__main__":
    main()
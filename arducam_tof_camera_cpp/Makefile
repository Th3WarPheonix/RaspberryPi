
CXX = /usr/bin/c++
CXX_FLAGS2 = -Wall -Wextra -O2
TARGET = preview_pointcloud

SOURCES = preview_pointcloud.cpp
OBJECTS = $(SOURCES:.cpp=.cpp.o)
DEPENDENCIES = $(SOURCES:.cpp=.cpp.o.d)

CXX_DEFINES = -DFMT_SHARED -D_GLIBCXX_USE_CXX11_ABI=1
CXX_INCLUDES = -isystem /usr/include/opencv4 -isystem /usr/include/eigen3
CXX_FLAGS = -Wno-psabi


# -c option
# compile or assemble the source files but stop before the linking stage

# - M options
# Preprocessor options used to generate Makefile dependency files (.d) alongside object files
# They track header inclusions so make knows when to recompile
# MD generates the dependency file
# MF names the dependency file
# MT sets the target rule name in the dependency file

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) /usr/lib/libArducamDepthCamera.so /usr/lib/aarch64-linux-gnu/libOpen3D.so.0.16.1 /usr/lib/aarch64-linux-gnu/libopencv_stitching.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_alphamat.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_aruco.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_barcode.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_bgsegm.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_bioinspired.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_ccalib.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_cvv.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_dnn_objdetect.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_dnn_superres.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_dpm.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_face.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_freetype.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_fuzzy.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_hdf.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_hfs.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_img_hash.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_intensity_transform.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_line_descriptor.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_mcc.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_quality.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_rapid.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_reg.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_rgbd.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_saliency.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_shape.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_stereo.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_structured_light.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_superres.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_surface_matching.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_tracking.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_videostab.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_viz.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_wechat_qrcode.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_xobjdetect.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_xphoto.so.4.6.0 /usr/lib/libsi4c.so /usr/lib/aarch64-linux-gnu/libfmt.so.9.1.0 /usr/lib/aarch64-linux-gnu/libopencv_highgui.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_datasets.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_plot.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_text.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_ml.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_phase_unwrapping.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_optflow.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_ximgproc.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_video.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_videoio.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_imgcodecs.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_objdetect.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_calib3d.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_dnn.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_features2d.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_flann.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_photo.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_imgproc.so.4.6.0 /usr/lib/aarch64-linux-gnu/libopencv_core.so.4.6.0 

%.cpp.o: %.cpp
	@echo "test"
	$(CXX) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT $(OBJECTS) -MF $(M_DEPENDENCIES) -o $@ -c $<

run:
	export MESA_GL_VERSION_OVERRIDE=4.5 && ./$(TARGET)

.PHONY: all clean run
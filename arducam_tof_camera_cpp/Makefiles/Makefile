# Single-file build system for Arducam TOF camera examples instead of the CMake system
# Usage: make [all|examples|preview_pointcloud|clean]

SHELL := /bin/sh

CC ?= cc
CXX ?= c++
PKG_CONFIG ?= pkg-config

CXXFLAGS ?= -std=c++14 -Wall -Wno-psabi
CFLAGS ?= -std=c11 -Wall
ARDUCAM_CXXFLAGS ?= -I/usr/include
ARDUCAM_CXX_LDFLAGS ?= -lArducamDepthCamera
ARDUCAM_C_LDFLAGS ?= -lArducamDepthCamera4C
WITH_OPENCV_WORLD ?= 0


# get header files with --cflags
# get from binaries with --libs
OPENCV_PKG ?= opencv4
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(OPENCV_PKG) 2>/dev/null)
OPENCV_LDFLAGS := $(shell $(PKG_CONFIG) --libs $(OPENCV_PKG) 2>/dev/null)
# if opencv4 is not found use older version
ifneq ($(OPENCV_CFLAGS),)
else
OPENCV_PKG := opencv
OPENCV_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(OPENCV_PKG) 2>/dev/null)
OPENCV_LDFLAGS := $(shell $(PKG_CONFIG) --libs $(OPENCV_PKG) 2>/dev/null)
endif

EIGEN_PKG ?= eigen3
EIGEN_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(EIGEN_PKG) 2>/dev/null)
EIGEN_LDFLAGS := $(shell $(PKG_CONFIG) --libs $(EIGEN_PKG) 2>/dev/null)

# ope3d does not always install a pkg-config file
# if not pkgconfig fails try to find headers and libs manually
# find /usr /opt /usr/local -name "Open3D.h": Searches the common installation directories for a file named exactly Open3D.h.
#| head -1: Takes only the first match
#| xargs dirname: Passes the file path to dirname, which strips the filename and returns the directory (e.g., Open3D.h becomes open3d).
#| xargs dirname: Runs dirname again on the result to go up one more level (e.g., open3d becomes include). This assumes the header is in a subdirectory like open3d/Open3D.h under the include root.
OPEN3D_PKG ?= open3d
OPEN3D_CFLAGS := $(shell $(PKG_CONFIG) --cflags $(OPEN3D_PKG) 2>/dev/null)
OPEN3D_LDFLAGS := $(shell $(PKG_CONFIG) --libs $(OPEN3D_PKG) 2>/dev/null)
ifeq ($(OPEN3D_CFLAGS),)
OPEN3D_INCLUDE := $(shell find /usr /opt /usr/local -name "Open3D.h" 2>/dev/null | head -1 | xargs dirname 2>/dev/null | xargs dirname 2>/dev/null)
OPEN3D_CFLAGS := $(if $(OPEN3D_INCLUDE),-I$(OPEN3D_INCLUDE),)
endif
ifeq ($(OPEN3D_LDFLAGS),)
OPEN3D_LIB_DIR := $(shell find /usr /opt /usr/local -name "libOpen3D.so*" 2>/dev/null | head -1 | xargs dirname 2>/dev/null)
OPEN3D_LDFLAGS := $(if $(OPEN3D_LIB_DIR),-L$(OPEN3D_LIB_DIR) -lOpen3D,)
endif

# Note: If still not found, set OPEN3D_CFLAGS and OPEN3D_LDFLAGS manually.
# Example: OPEN3D_CFLAGS := -I/usr/include
#         OPEN3D_LDFLAGS := -lOpen3D

# Allows using OpenCV's single opencv_world library instead of individual libs
# better for smaller binaries or specific builds
# If WITH_OPENCV_WORLD is set to 1, append -lopencv_world to CORE_LDFLAGS
# Otherwise, use the full OPENCV_LDFLAGS (which includes all individual OpenCV libs).
ifeq ($(WITH_OPENCV_WORLD),1)
CORE_LDFLAGS := $(OPENCV_LDFLAGS) -lopencv_world
else
CORE_LDFLAGS := $(OPENCV_LDFLAGS)
endif

CXXFLAGS += $(OPENCV_CFLAGS) $(ARDUCAM_CXXFLAGS) $(OPEN3D_CFLAGS) $(EIGEN_CFLAGS)
CFLAGS += $(OPENCV_CFLAGS) $(ARDUCAM_CXXFLAGS)

EXAMPLE_LDFLAGS := $(ARDUCAM_CXX_LDFLAGS) $(OPENCV_LDFLAGS)
PREVIEW_LDFLAGS := $(OPEN3D_LDFLAGS) $(ARDUCAM_CXX_LDFLAGS) $(CORE_LDFLAGS) $(EIGEN_CFLAGS)

PREVIEW_DEPTH_CPP_SRC := example/cpp/preview_depth.cpp
CAPTURE_RAW_CPP_SRC := example/cpp/capture_raw.cpp
PREVIEW_USB_CPP_SRC := example/cpp/preview_by_usb.cpp
EXAMPLE_CPP_SRCS := example/cpp/example/parse_arg.cpp example/cpp/example/pid.c example/cpp/example/test.cpp
PREVIEW_POINTCLOUD_SRC := open3d_preview/preview_pointcloud.cpp
PREVIEW_DEPTH_C_SRC := example/c/preview_depth.c

PREVIEW_DEPTH_TARGET := build/example/cpp/preview_depth
CAPTURE_RAW_TARGET := build/example/cpp/capture_raw
PREVIEW_USB_TARGET := build/example/cpp/preview_usb
EXAMPLE_TARGET := build/example/cpp/example
PREVIEW_DEPTH_C_TARGET := build/example/c/preview_depth_c
OPEN3D_PREVIEW_TARGET := build/open3d_preview/preview_pointcloud

EXAMPLE_TARGETS := $(PREVIEW_DEPTH_TARGET) $(CAPTURE_RAW_TARGET) $(PREVIEW_USB_TARGET) $(EXAMPLE_TARGET) $(PREVIEW_DEPTH_C_TARGET)

.PHONY: all examples preview_pointcloud clean help

all: examples
# ensure open3d is available before trying to build preview_pointcloud
ifneq ($(OPEN3D_CFLAGS),)
all: $(OPEN3D_PREVIEW_TARGET)
else
	@printf "Warning: Open3D not found. Install Open3D or set OPEN3D_CFLAGS/OPEN3D_LDFLAGS to build preview_pointcloud.\n"
endif

examples: $(EXAMPLE_TARGETS)

preview_pointcloud: $(OPEN3D_PREVIEW_TARGET)

$(PREVIEW_DEPTH_TARGET): $(PREVIEW_DEPTH_CPP_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(EXAMPLE_LDFLAGS)

$(CAPTURE_RAW_TARGET): $(CAPTURE_RAW_CPP_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(EXAMPLE_LDFLAGS)

$(PREVIEW_USB_TARGET): $(PREVIEW_USB_CPP_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(EXAMPLE_LDFLAGS)

$(EXAMPLE_TARGET): $(EXAMPLE_CPP_SRCS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(EXAMPLE_LDFLAGS)

$(PREVIEW_DEPTH_C_TARGET): $(PREVIEW_DEPTH_C_SRC)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ $(ARDUCAM_C_LDFLAGS)

$(OPEN3D_PREVIEW_TARGET): $(PREVIEW_POINTCLOUD_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(PREVIEW_LDFLAGS)

run_pointcloud:
	export MESA_GL_VERSION_OVERRIDE=4.5 && ./$(OPEN3D_PREVIEW_TARGET)

clean:
	rm -rf build

help:
	@printf "Usage:\n"
	@printf "  make all               Build all example binaries and preview_pointcloud if Open3D is available.\n"
	@printf "  make examples          Build only the example binaries.\n"
	@printf "  make preview_pointcloud  Build the Open3D pointcloud preview.\n"
	@printf "  make clean             Remove generated build artifacts.\n"
	@printf "  make print-open3d      Print detected Open3D flags.\n"

print-open3d:
	@printf "OPEN3D_CFLAGS = %s\n" "$(OPEN3D_CFLAGS)"
	@printf "OPEN3D_LDFLAGS = %s\n" "$(OPEN3D_LDFLAGS)"


SHELL := /bin/sh

CC ?= cc
CXX ?= c++
PKG_CONFIG ?= pkg-config

CXXFLAGS ?= -std=c++17 -Wall -Wno-psabi -Wextra -O2
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

# libcamera
LIBCAM_PKG := libcamera
LIBCAM_CFLAGS = $(shell $(PKG_CONFIG) --cflags $(LIBCAM_PKG) 2>/dev/null)
LIBCAM_LDFLAGS = $(shell $(PKG_CONFIG) --libs $(LIBCAM_PKG) 2>/dev/null)

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

CXXFLAGS += $(OPENCV_CFLAGS) $(ARDUCAM_CXXFLAGS) $(OPEN3D_CFLAGS) $(EIGEN_CFLAGS) $(LIBCAM_CFLAGS)
CFLAGS += $(OPENCV_CFLAGS) $(ARDUCAM_CXXFLAGS)

TARGET_LDFLAGS := $(OPEN3D_LDFLAGS) $(ARDUCAM_CXX_LDFLAGS) $(CORE_LDFLAGS) $(EIGEN_LDFLAGS) $(LIBCAM_LDFLAGS)

MERGED_DISPLAY_SRC := src/merged_display.cpp
MERGED_DISPLAY_TARGET := build/merged_display

.PHONY: all examples run clean help

all: merged_display
# ensure open3d is available before trying to build
ifneq ($(OPEN3D_CFLAGS),)
all: $(MERGED_DISPLAY_TARGET)
else
	@printf "Warning: Open3D not found. Install Open3D or set OPEN3D_CFLAGS/OPEN3D_LDFLAGS to build merged_display.\n"
endif

# -c option
# compile or assemble the source files but stop before the linking stage

# - M options
# Preprocessor options used to generate Makefile dependency files (.d) alongside object files
# They track header inclusions so make knows when to recompile
# MD generates the dependency file
# MF names the dependency file
# MT sets the target rule name in the dependency file

# # Default target
# all: $(TARGET)

# %.cpp.o: %.cpp
# 	@echo "test"
# 	$(CXX) $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT $(OBJECTS) -MF $(M_DEPENDENCIES) -o $@ -c $<

$(MERGED_DISPLAY_TARGET): $(MERGED_DISPLAY_SRC)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $< -o $@ $(TARGET_LDFLAGS)

merged_display: $(MERGED_DISPLAY_TARGET)

run:
	export MESA_GL_VERSION_OVERRIDE=4.5 && ./$(TARGET)

clean:
	rm -rf build

print-open3d:
	@printf "OPEN3D_CFLAGS = %s\n" "$(OPEN3D_CFLAGS)"
	@printf "OPEN3D_LDFLAGS = %s\n" "$(OPEN3D_LDFLAGS)"

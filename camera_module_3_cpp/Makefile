CXX = g++

# Compiler flags
# -std=c++17: Use C++17 standard for modern features
# -Wall: Enable all common warnings
# -Wextra: Enable extra warnings for stricter code checking
# -O2: Optimization level 2 (good balance between speed and compile time)
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# Linker flags - dynamically fetch libcamera library paths
# $(shell ...) runs a shell command at make-time to get the value
# Dynamically retrieved means: determined when 'make' runs, not hardcoded
# This makes the Makefile portable across different systems where libcamera
# may be installed in different locations (e.g., /usr/lib vs /usr/local/lib)
# pkg-config automatically finds where libcamera is installed and returns:
# -L/path/to/libs -lcamera -lcamera-base (varies per system)
LDFLAGS = $(shell pkg-config --libs libcamera)

# Include flags - dynamically fetch libcamera include paths
# Same dynamic approach: finds libcamera header files location (-I/path/to/headers)
# Without this, compiler won't find #include <libcamera/...> statements
INCLUDES = $(shell pkg-config --cflags libcamera)

# Output executable name
TARGET = camera_module_3

# Source files
SRC = main.cpp

# Object files (convert .cpp to .o)
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Build executable by linking object files
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJ) $(TARGET) test.h264

# Build and run the program
run: $(TARGET)
	./$(TARGET)

# Declare phony targets (not actual files)
.PHONY: all clean run

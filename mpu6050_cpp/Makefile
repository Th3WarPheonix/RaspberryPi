CC = gcc
CFLAGS = -Wall -Wextra -O2
LIBS = -lwiringPi -lm
TARGET = mpu6050_app

# Source files
SOURCES = main0.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LIBS)

# Compile source files
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the program (requires root for GPIO/I2C access)
run: $(TARGET)
	sudo ./$(TARGET)

# Realtime plotting with Python script
# make sure to activate python environment with matplotlib installed
# sudo ./mpu6050_app | python3 plot_realtime.py

.PHONY: all clean run

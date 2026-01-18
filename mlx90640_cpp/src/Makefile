CC = gcc
CFLAGS = -Wall -Wextra -O2 -I../inc
LDFLAGS = -lm
TARGET = mlx90640_app

# Source files
SOURCES = MLX90640_API.c MLX90640_I2C_Driver.c main.c i2c.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS) $(LDFLAGS)

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the program (requires root for GPIO/I2C access)
run: $(TARGET)
	sudo ./$(TARGET)

.PHONY: all clean run
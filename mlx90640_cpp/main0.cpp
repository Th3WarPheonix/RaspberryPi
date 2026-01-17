#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>


#define MLX90640_ADDR 0x33



struct mlx90640 {
    int fd;
    int width = 32;
    int height = 24;
    float pixel_data[32 * 24];
};

int MLX90640_Init(struct mlx90640 *mlx) {
    // Initialize wiringPi
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed\n");
        return -1;
    }

    // Open I2C connection
    mlx->fd = wiringPiI2CSetup(MLX90640_ADDR);
    if (mlx->fd == -1) {
        printf("Failed to open I2C connection\n");
        return -1;
    }

    printf("MLX90640 initialized successfully\n");
    return 0;
}
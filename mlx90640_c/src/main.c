#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <math.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_Driver.h"

#define MLX_I2C_ADDR 0x33

int main() {

    printf("Initializing I2C...\n");
    MLX90640_I2CInit();

    uint16_t eeMLX90640[832];
    printf("Dumping EEPROM...\n");
    int status = MLX90640_DumpEE(MLX_I2C_ADDR, eeMLX90640);
    printf("DumpEE status: %d\n", status);
    if (status != 0) {
        printf("Failed to dump EEPROM: %d\n", status);
        return 1;
    }

    // for (int i = 0; i < 832; i+=16)
    // {
    //     printf ("0x%04X:", 0x2400+i);
    //     for (int j = 0; j < 16; j++)
    //     {
    //     printf (" %04X", eeMLX90640[i+j]);
    //     }
    //     printf("\n");
    // }

    paramsMLX90640 mlx90640;
    printf("Extracting parameters...\n");
    status = MLX90640_ExtractParameters(&eeMLX90640, &mlx90640);
    printf("ExtractParameters status: %d\n", status);
    if (status != 0) {
        printf("Failed to extract parameters: %d\n", status);
        return 1;
    }

    MLX90640_SetRefreshRate(MLX_I2C_ADDR, MLX90640_REFRESH_2HZ);
    MLX90640_SetChessMode(MLX_I2C_ADDR);
    printf("Mode: %d\n", MLX90640_GetCurMode(MLX_I2C_ADDR));

    uint16_t mlx90640Frame[834];
    float mlx90640To[768];
    float emissivity = 0.95;
    float tr = 23.15;

    printf("Starting frame capture...\n");
    float avg_temp = 0.0f;
    while (1) {

        status = MLX90640_GetFrameData(MLX_I2C_ADDR, mlx90640Frame);
        if (status < 0) {
            printf("GetFrame Error: %d\n", status);
            continue;
        }

        float vdd = MLX90640_GetVdd(mlx90640Frame, &mlx90640);
        float Ta = MLX90640_GetTa(mlx90640Frame, &mlx90640);

        float tr_new = tr - 8.0f;
        if (tr_new < -40.0f) tr_new = -40.0f;
        if (tr_new > 125.0f) tr_new = 125.0f;
    
        MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr_new, mlx90640To);

        for (int i = 0; i < 768; i++) {
            avg_temp += mlx90640To[i];
        }
        avg_temp /= 768.0f;

        printf("Average Temperature: %.2f C\n", avg_temp);

        sleep(.5);  // 500ms delay
    
    }
    return 0;
}
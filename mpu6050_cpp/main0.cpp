#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

// [1] https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
// [2] https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

// MPU6050 I2C Address [1]
// 6.4 Electrical Specifications, Continued
#define MPU6050_ADDR 0x68

// MPU6050 Register Addresses [2]
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B
#define ACCEL_XOUT_L 0x3C
#define ACCEL_YOUT_H 0x3D
#define ACCEL_YOUT_L 0x3E
#define ACCEL_ZOUT_H 0x3F
#define ACCEL_ZOUT_L 0x40
#define TEMP_OUT_H 0x41
#define TEMP_OUT_L 0x42
#define GYRO_XOUT_H 0x43
#define GYRO_XOUT_L 0x44
#define GYRO_YOUT_H 0x45
#define GYRO_YOUT_L 0x46
#define GYRO_ZOUT_H 0x47
#define GYRO_ZOUT_L 0x48
#define ACCEL_CONFIG 0x1C
#define GYRO_CONFIG 0x1B

// Clock Rate [1]
// 6.4 Electrical Specifications, Continued
#define FAST_MODE 400000
#define STANDARD_MODE 100000

// Sensitivity scales [1]
// 6.1 - 6.3
#define ACCEL_SENSITIVITY 16384.0f // For ±2g range
#define GYRO_SENSITIVITY 131.0f // For ±250°/s range
#define TEMP_SENSITIVITY 340.0f
#define TEMP_OFFSET 35.0f

struct MPU6050 {
    int fd; // File descriptor for I2C
    float accel_x, accel_y, accel_z;
    float gyro_x, gyro_y, gyro_z;
    float temperature;
};

// Reads a 16-bit signed integer from two consecutive 8-bit registers
// MPU6050 sensor data is split across high and low bytes
int16_t readInt16(int fd, uint8_t reg) {
    uint8_t high = wiringPiI2CReadReg8(fd, reg); // Read high byte from register
    uint8_t low = wiringPiI2CReadReg8(fd, reg + 1); // Read low byte from next register
    return (int16_t)((high << 8) | low); // Combine: shift high byte left 8 bits, OR with low byte
}

int MPU6050_Init(struct MPU6050 *mpu) {
    // Initialize wiringPi
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed\n");
        return -1;
    }

    // Open I2C connection
    mpu->fd = wiringPiI2CSetup(MPU6050_ADDR);
    if (mpu->fd == -1) {
        printf("Failed to open I2C connection\n");
        return -1;
    }

    // Wake up MPU6050 (clear sleep bit)
    wiringPiI2CWriteReg8(mpu->fd, PWR_MGMT_1, 0x00);
    sleep(1); // Wait for device to wake up

    // Set accelerometer range to ±2g
    wiringPiI2CWriteReg8(mpu->fd, ACCEL_CONFIG, 0x00);

    // Set gyroscope range to ±250°/s
    wiringPiI2CWriteReg8(mpu->fd, GYRO_CONFIG, 0x00);

    printf("MPU6050 initialized successfully\n");
    return 0;
}

void MPU6050_ReadData(struct MPU6050 *mpu) {
    // Read accelerometer data
    int16_t accel_raw_x = readInt16(mpu->fd, ACCEL_XOUT_H);
    int16_t accel_raw_y = readInt16(mpu->fd, ACCEL_YOUT_H);
    int16_t accel_raw_z = readInt16(mpu->fd, ACCEL_ZOUT_H);

    // Read temperature data
    int16_t temp_raw = readInt16(mpu->fd, TEMP_OUT_H);

    // Read gyroscope data
    int16_t gyro_raw_x = readInt16(mpu->fd, GYRO_XOUT_H);
    int16_t gyro_raw_y = readInt16(mpu->fd, GYRO_YOUT_H);
    int16_t gyro_raw_z = readInt16(mpu->fd, GYRO_ZOUT_H);

    // Convert raw data to actual values
    mpu->accel_x = accel_raw_x / ACCEL_SENSITIVITY;
    mpu->accel_y = accel_raw_y / ACCEL_SENSITIVITY;
    mpu->accel_z = accel_raw_z / ACCEL_SENSITIVITY;

    mpu->temperature = (temp_raw / TEMP_SENSITIVITY) + TEMP_OFFSET;

    mpu->gyro_x = gyro_raw_x / GYRO_SENSITIVITY;
    mpu->gyro_y = gyro_raw_y / GYRO_SENSITIVITY;
    mpu->gyro_z = gyro_raw_z / GYRO_SENSITIVITY;
}

void PrintData(struct MPU6050 *mpu) {
    // Output CSV format for real-time plotting: accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp
    printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\n",
           mpu->accel_x, mpu->accel_y, mpu->accel_z,
           mpu->gyro_x, mpu->gyro_y, mpu->gyro_z,
           mpu->temperature);
    fflush(stdout);  // Ensure data is sent immediately
}

int main(void) {
    struct MPU6050 mpu;

    if (MPU6050_Init(&mpu) == -1) {
        return 1;
    }

    // Continuously read and output CSV data for real-time plotting
    fprintf(stderr, "Starting sensor data stream...\n");
    
    while (1) {
        MPU6050_ReadData(&mpu);
        PrintData(&mpu);
        usleep(100000);  // 100ms delay (10 samples/sec)
    }

    return 0;
}

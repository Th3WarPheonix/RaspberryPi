import time
import board
import adafruit_mpu6050
import matplotlib.pyplot as plt

i2c = board.I2C()  # uses board.SCL and board.SDA
# i2c = board.STEMMA_I2C()  # For using the built-in STEMMA QT connector on a microcontroller
mpu = adafruit_mpu6050.MPU6050(i2c)

# while True:

    # 
#     print(f"Temperature: {mpu.temperature:.2f} C")
#     print("")
#     time.sleep(1)


fig, ax = plt.subplots(3, 1)

i = 0
while True:
    # print(f"Acceleration: X:{mpu.acceleration[0]:.2f}, Y: {mpu.acceleration[1]:.2f}, Z: {mpu.acceleration[2]:.2f} m/s^2") # noqa: E501)
    # print(f"Gyro X:{mpu.gyro[0]:.2f}, Y: {mpu.gyro[1]:.2f}, Z: {mpu.gyro[2]:.2f} rad/s")
    ax[0].scatter(mpu.acceleration[0], mpu.acceleration[1])
    ax[1].scatter(mpu.acceleration[1], mpu.acceleration[2])
    ax[2].scatter(i, mpu.temperature)
    plt.pause(0.005)
    i += 1

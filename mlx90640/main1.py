##################################
# MLX90640 Test with Raspberry Pi
##################################
#
import time,board,busio
import numpy as np
import adafruit_mlx90640
from gpiozero import CPUTemperature

i2c = busio.I2C(board.SCL, board.SDA, frequency=400000) # setup I2C
mlx = adafruit_mlx90640.MLX90640(i2c) # begin MLX90640 with I2C comm
mlx.refresh_rate = adafruit_mlx90640.RefreshRate.REFRESH_2_HZ # set refresh rate

frame = np.zeros((24*32,)) # setup array for storing all 768 temperatures
while True:
    try:
        mlx.getFrame(frame) # read MLX temperatures into frame var
        break
    except ValueError:
        continue # if error, just read again

# print out the average temperature from the MLX90640
print(f'Average MLX90640 Temperature: {np.mean(frame):2.1f}C '\
      f'({(((9.0/5.0)*np.mean(frame))+32.0):2.1f}F)')

cpu = CPUTemperature()

if cpu.temperature > 80:
    print(f'CPU Temperature: {cpu.temperature:2.1f}C ')
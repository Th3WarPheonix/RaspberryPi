# Overview
This repo contains many small scripts and programs for the setup of Raspberry Pi external peripherals.

# Setup

## General
1. Create a virtual environment that can see and use packages on the system packages.

- `python3 -m venv --system-site-packages venv_folder_path`
- `source venv_folder_path/bin/activate`
- `pip install matplotlib scipy`
- Camera Module 3 and MLX90640 require different versions of numpy

## Camera Module 3
1. Create isolated virtual environment and install packages

- `source venv_folder_path/bin/activate`
- `pip install picamera2`

## MLX90640
### Python
1. Install system-level dependencies to use the CLI to check if I2C connection is setup properly

- `sudo apt-get install -y i2c-tools`

2. Enable I2C communciation on the Raspberry Pi 5

- `sudo raspi-config`
- Interface Options -> I2C

3. Change the buadrate of the Pi

- `sudo nano /boot/firmware/config.txt`
- dtparam=i2c_arm_baudrate=400000

4. Reboot the Pi

5. Check connection of MLX90640
- `i2cdetect -y 1`
- Should show as 33 by default

6. Install Python packages

- `source venv_folder_path/bin/activate`
- `pip install RPI.GPIO adafruit-blinka adafruit-circuitpython-mlx90640`

### C
#### Driver
1. Driver and I2C communication code provided by Melexis

- https://github.com/melexis/mlx90640-library/
- https://github.com/melexis-fir/mlx90640-driver-devicetree-py/

2. WiringPi was unable to handle the MLX90640's 16-bit memory addressess
3. Switched to using straight `ioctl()`
5. The arguments are the I2C bus file descriptor, a request (defined in `linux/i2c-dev.h`), and a final argument
- The final argument when the request is `I2C_RDWR` (==`0x0707`) is the address of the struct `i2c_rdwr_ioctl_data` (`linux/i2c-dev.h`). It contains the number of messages to be sent over the bus, and an array of 2 message structs, `i2c_msg` (`linux/i2c.h`)
- The message struct, `i2c_msg`, contains the address of the device to communicate with, a flag (i.e. read or write), the length of buffer in bytes, and the buffer
    - The buffer in a write operation is a pointer to a memory address on the Raspberry Pi where the data to write is stored
    - The buffer in a read operation is a pointer to a memory address on the Raspberry Pi to the buffer where the information from the MLX90640 will be stored
- To read from the a device using I2C you must:
    1. Write the memory address from which you want to start reading to the device:
    ```
    ioctl_msg[0].len = 2;
    ioctl_msg[0].addr = 0x33;
    ioctl_msg[0].buf = pointer to array of unsigned char;
    ioctl_msg[0].flags = 0;
    ```
    - The buffer of the first message points to an unsigned char = {0x24, 0x00}, which is the memory adress on the MLX90640 to read from broken down by bytes. Not broken down it is 0x2400, which is the start of the EEPROM on the MLX90640. It must be an array of unsigned char because each element is 1 byte which prevents endianness issues and the array can be expanded to 4 elements for I2C standards. Therefore in binary the address to be accessed looks like {0b00100100, 0b00000000}, which is how we want the address to be sent over the bus.

    2. Read from the device a chunk of memory of a certain size:
    ```
    ioctl_msg[1].len = 1664;
    ioctl_msg[1].addr = 0x33;
    ioctl_msg[1].buf = (memory address of array of size 1664);
    ioctl_msg[1].flags = 1;
    ```




#### Datahseet
1. https://www.melexis.com/en/product/MLX90640/Far-Infrared-Thermal-Sensor-Array?_gl=1*1kk3ies*_up*MQ..*_ga*ODIyNzAxNjc1LjE3NjgzNDE5MTU.*_ga_PS2G499C7D*czE3NjgzNDE5MTQkbzEkZzAkdDE3NjgzNDE5MTQkajYwJGwwJGgw

## MPU6050
### Python
- `source venv_folder_path/bin/activate`
- `pip install adafruit-circuitpython-mpu6050`

### C
#### Datasheets
1. https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
2. https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

##### WiringPi
- https://github.com/WiringPi/WiringPi/blob/master/documentation/english/functions.md

##### Snippet
Create Debian package:

sudo apt install git
git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build debian
mv debian-template/wiringpi_3.16_arm64.deb .
Install Debian package:

sudo apt install ./wiringpi_3.16_arm64.deb
Uninstall Debian package:

sudo apt purge wiringpi

## Slamtec Lidar
### Python
- `source venv_folder_path/bin/activate`
- `pip install rplidar-roboticia`

## Remove Packages
`sudo apt-get purge packagename`
`sudo apt autoremove`

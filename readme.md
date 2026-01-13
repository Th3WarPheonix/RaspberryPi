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
- Should show as 33

6. Install Python packages

- `source venv_folder_path/bin/activate`
- `pip install RPI.GPIO adafruit-blinka adafruit-circuitpython-mlx90640`

## MPU6050
### Python
- `source venv_folder_path/bin/activate`
- `pip install adafruit-circuitpython-mpu6050`

### C++
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

## Remove Packages
`sudo apt-get purge packagename`
`sudo apt autoremove`

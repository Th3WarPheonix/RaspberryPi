import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sys
from collections import deque

# Store up to 200 data points
MAX_POINTS = 200
data = {
    'accel_x': deque(maxlen=MAX_POINTS),
    'accel_y': deque(maxlen=MAX_POINTS),
    'accel_z': deque(maxlen=MAX_POINTS),
    'gyro_x': deque(maxlen=MAX_POINTS),
    'gyro_y': deque(maxlen=MAX_POINTS),
    'gyro_z': deque(maxlen=MAX_POINTS),
    'temp': deque(maxlen=MAX_POINTS),
}

# Create figure with 3 subplots
fig, axes = plt.subplots(3, 1, figsize=(12, 8))
fig.suptitle('MPU6050 Real-Time Sensor Data', fontsize=14, fontweight='bold')

# Line objects for acceleration
line_accel_x, = axes[0].plot([], [], label='Accel X', color='red', marker='o', markersize=3)
line_accel_y, = axes[0].plot([], [], label='Accel Y', color='green', marker='s', markersize=3)
line_accel_z, = axes[0].plot([], [], label='Accel Z', color='blue', marker='^', markersize=3)
axes[0].set_ylabel('Acceleration (g)', fontsize=10)
axes[0].set_title('Accelerometer Data', fontsize=11)
axes[0].legend(loc='upper left')
axes[0].grid(True, alpha=0.3)
axes[0].set_ylim(-2, 2)

# Line objects for gyroscope
line_gyro_x, = axes[1].plot([], [], label='Gyro X', color='red', marker='o', markersize=3)
line_gyro_y, = axes[1].plot([], [], label='Gyro Y', color='green', marker='s', markersize=3)
line_gyro_z, = axes[1].plot([], [], label='Gyro Z', color='blue', marker='^', markersize=3)
axes[1].set_ylabel('Rotation (°/s)', fontsize=10)
axes[1].set_title('Gyroscope Data', fontsize=11)
axes[1].legend(loc='upper left')
axes[1].grid(True, alpha=0.3)
axes[1].set_ylim(-300, 300)

# Line object for temperature
line_temp, = axes[2].plot([], [], label='Temperature', color='orange', marker='o', markersize=3)
axes[2].set_ylabel('Temperature (°C)', fontsize=10)
axes[2].set_xlabel('Sample Number', fontsize=10)
axes[2].set_title('Temperature Data', fontsize=11)
axes[2].legend(loc='upper left')
axes[2].grid(True, alpha=0.3)
axes[2].set_ylim(20, 60)

def update(frame):
    """Read one line from stdin and update plots"""
    try:
        # read a line from standard input
        # so if you pipe c++ data to this script it reads it
        line = sys.stdin.readline()

        if not line:
            return [line_accel_x, line_accel_y, line_accel_z, 
                    line_gyro_x, line_gyro_y, line_gyro_z, line_temp]
        
        # Parse CSV: accel_x,accel_y,accel_z,gyro_x,gyro_y,gyro_z,temp
        values = [float(x) for x in line.strip().split(',')]
        data['accel_x'].append(values[0])
        data['accel_y'].append(values[1])
        data['accel_z'].append(values[2])
        data['gyro_x'].append(values[3])
        data['gyro_y'].append(values[4])
        data['gyro_z'].append(values[5])
        data['temp'].append(values[6])
        
    except (ValueError, IndexError):
        pass  # Skip invalid lines
    
    # Update acceleration plot
    x = list(range(len(data['accel_x'])))
    line_accel_x.set_data(x, list(data['accel_x']))
    line_accel_y.set_data(x, list(data['accel_y']))
    line_accel_z.set_data(x, list(data['accel_z']))
    axes[0].set_xlim(0, max(len(x), 10))
    
    # Update gyroscope plot
    line_gyro_x.set_data(x, list(data['gyro_x']))
    line_gyro_y.set_data(x, list(data['gyro_y']))
    line_gyro_z.set_data(x, list(data['gyro_z']))
    axes[1].set_xlim(0, max(len(x), 10))
    
    # Update temperature plot
    line_temp.set_data(x, list(data['temp']))
    axes[2].set_xlim(0, max(len(x), 10))
    
    return [line_accel_x, line_accel_y, line_accel_z, 
            line_gyro_x, line_gyro_y, line_gyro_z, line_temp]

# Create animation
# blit=True is used to optimize drawing performance by only 
# redrawing the parts of the figure that have changed each frame, 
# rather than the entire figure
anim = animation.FuncAnimation(fig, update, interval=100, blit=True)

plt.tight_layout()
plt.show()

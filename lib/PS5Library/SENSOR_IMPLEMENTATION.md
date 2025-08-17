# PS5 Controller IMU Sensor Implementation

## Overview
This implementation adds full support for the PS5 DualSense controller's 6-axis IMU (Inertial Measurement Unit) sensors, including:
- **3-axis Accelerometer** - measures linear acceleration
- **3-axis Gyroscope** - measures angular velocity

## What Was Implemented

### 1. Core Sensor Parsing (`ps5_parser.c`)
- Added proper packet indices for IMU sensor data
- Implemented `parsePacketSensor()` function with correct 16-bit little-endian parsing
- Enabled sensor parsing in the main packet processing loop
- Added sensor event detection for motion tracking

### 2. Enhanced Event System (`ps5.h`)
- Extended `ps5_event_t` structure to include sensor movement events
- Added threshold-based motion detection to distinguish significant sensor changes

### 3. Convenient Access Methods (`ps5Controller.h`)
The existing controller class already had sensor accessor methods:
- `AccelX()`, `AccelY()`, `AccelZ()` - Accelerometer readings
- `GyroX()`, `GyroY()`, `GyroZ()` - Gyroscope readings

These now return real sensor data instead of zeros.

## Usage Examples

### Basic Sensor Reading
```cpp
#include <ps5Controller.h>

void setup() {
  Serial.begin(115200);
  ps5.begin("1a:2b:3c:01:01:01"); // Your controller's MAC
}

void loop() {
  if (ps5.isConnected()) {
    // Read accelerometer (tilt/orientation)
    int16_t accel_x = ps5.AccelX();
    int16_t accel_y = ps5.AccelY();
    int16_t accel_z = ps5.AccelZ();
    
    // Read gyroscope (rotation rate)
    int16_t gyro_x = ps5.GyroX();
    int16_t gyro_y = ps5.GyroY();
    int16_t gyro_z = ps5.GyroZ();
    
    // Simple tilt detection
    if (abs(accel_x) > 8000) {
      Serial.println("Controller tilted!");
    }
  }
}
```

### Motion Gaming Applications
```cpp
// Flight simulator control
int16_t roll = ps5.AccelY();    // Roll axis
int16_t pitch = ps5.AccelX();   // Pitch axis
int16_t yaw = ps5.GyroZ();      // Yaw rate

// Racing game steering
float steering = ps5.GyroZ() / 1000.0;  // Convert to degrees/sec

// Motion-controlled menu navigation
if (abs(ps5.AccelX()) > 5000) {
  // Tilt left/right to navigate menu
}
```

## Sensor Specifications

### Accelerometer
- **Range**: ±4g (approximately ±32,768 raw units)
- **Resolution**: 16-bit signed integer
- **Units**: Raw accelerometer units (can be converted to m/s²)
- **Use Cases**: Tilt detection, orientation, gravity sensing, impact detection

### Gyroscope  
- **Range**: ±2000 degrees/second (approximately ±32,768 raw units)
- **Resolution**: 16-bit signed integer  
- **Units**: Raw gyroscope units (can be converted to degrees/second)
- **Use Cases**: Rotation detection, angular velocity, gesture recognition

## Calibration and Filtering

For best results, implement calibration and filtering:

```cpp
// Simple calibration (controller held level)
int16_t accel_offset_x = 0;
int16_t accel_offset_y = 0;
// ... calibrate by averaging readings when stationary

// Apply calibration
int16_t calibrated_x = ps5.AccelX() - accel_offset_x;

// Simple low-pass filter
float filtered_accel = filtered_accel * 0.9 + calibrated_x * 0.1;
```

## Provided Examples

1. **`ps5_sensors_example.ino`** - Basic sensor usage with motion detection
2. **`ps5_advanced_sensors.ino`** - Calibration, filtering, and advanced motion detection  
3. **`ps5_packet_debug.ino`** - Debug utility to analyze raw packet data

## Packet Structure Notes

The PS5 DualSense controller uses HID reports over Bluetooth. The implemented packet indices are:

```c
// Sensor data locations in HID packet
packet_index_sensor_gyroscope_x = 16,      // Gyroscope X (low byte)
packet_index_sensor_gyroscope_x_hi = 17,   // Gyroscope X (high byte)  
// ... similar for Y and Z axes
packet_index_sensor_accelerometer_x = 22,  // Accelerometer X (low byte)
// ... etc
```

**Note**: If sensors don't work correctly, use `ps5_packet_debug.ino` to identify the correct packet indices for your specific controller firmware.

## Troubleshooting

### Sensors Return Zero
1. Check that `parsePacketSensor()` is being called (uncommented in parser)
2. Verify packet indices are correct for your controller firmware
3. Use the debug example to examine raw packet data

### Noisy/Jumpy Readings
1. Implement calibration (see advanced example)
2. Apply low-pass filtering  
3. Use motion detection thresholds appropriate for your application

### Controller Won't Connect
1. Verify MAC address is correct and unicast
2. Ensure controller is in pairing mode
3. Check Bluetooth initialization

## Applications

With working IMU sensors, you can create:
- **Motion-controlled games** (racing, flight simulation)
- **Gesture recognition systems**
- **Tilt-based interfaces**  
- **Motion-activated triggers**
- **Stabilization systems**
- **Activity monitors**

## Future Enhancements

Potential improvements:
- Sensor fusion algorithms (complementary/Kalman filters)
- Touchpad coordinate parsing
- Advanced gesture recognition
- Sensor data timestamping
- Adaptive filtering based on motion state

The core sensor implementation is now complete and functional!

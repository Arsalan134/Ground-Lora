# 🎮 ps5-esp32
Use a PS5 controller with an ESP32! 🚀

This is heavily based on the work of Jeffery Pernis to connect a PS3 controller to an ESP32, and the PS4-esp32 library. 

PS3 controller: https://github.com/jvpernis/esp32-ps3 🎮

PS4 controller: https://github.com/aed3/PS4-esp32 🎮

[Here's a video](https://youtu.be/BmlKBs27pgE) 📹 about how the ps4 library was made.

This repo can be downloaded as a zip file and imported into the Arduino IDE as a library. 📚

## 🛠️ Installation
The instructions on how to do this are base off what can be found [here](https://github.com/jvpernis/esp32-ps3/issues/3#issuecomment-517141523)
1. 📲 You can add the ESP32 boards to your Arduino IDE by adding them to the Boards Manager:
    1. Open `File -> Preferences` ⚙️
    1. Paste the following URL in the `Additional Boards Manager URLs` field:
    `https://dl.espressif.com/dl/package_esp32_index.json`
    1. Open the Boards Manager with `Tools -> Board: "xxx" -> Boards Manager` 🛠️
    1. Look for `esp32` (probably the last one in the list), and click `install` ⬇️
    1. Select the ESP32 board you have with `Tools -> Board: "xxx"` under the section `ESP32 Arduino` 🎯
1. 📦 To install this library into your Arduino IDE:
    1. Click on the "Code" button in the top right of this page 💻
    1. Select "Download Zip" (It's always a good idea to look through the code on this page first to make sure you know what you're downloading) 📥
    1. In the Arduino IDE, navigate to `Sketch -> Include Library -> Add .ZIP Library`, then select the file you just downloaded 📂


## 🎮 Pairing the PS5 Controller:
When a PS5 controller is 'paired' to a PS5 console, it just means that it has stored the console's Bluetooth MAC address, which is the only device the controller will connect to. 📡 Usually, this pairing happens when you connect the controller to the PS5 console using a USB cable, and press the PS button. This initiates writing the console's (or in this case an ESP32's) MAC address to the controller. 🔗

To do this, you'll need to include the controller's IP address in the ```ps5.begin()``` function during within the ```setup()``` Arduino function like below where ```1a:2b:3c:01:01:01``` is the MAC address (**note that MAC address must be unicast**): 📱
```cpp
void setup()
{
    ps5.begin("1a:2b:3c:01:01:01"); // 🎮 Connect to PS5 controller
    Serial.println("Ready."); // ✅ System ready
}
```

**TLDR:** 📱 Connect PS5 controller with phone through bluetooth. Get the controller's bluetooth MAC address (look in About settings of phone). Replace '1a:2b:3c:01:01:01' with your controller's bluetooth MAC. Same can be done with your PS5 console if more convenient. 🎮
##
**📝 Note: This library now supports full PS5 DualSense controller functionality including:**
- ✅ **Buttons and analog inputs** - Fully tested and working 🎮
- ✅ **IMU sensors (Accelerometer & Gyroscope)** - Now implemented and working 📊
- ⚠️ **Rumble/haptic feedback and lights** - Implemented but may need testing 🎆
- ⚠️ **Touchpad** - Basic structure in place, needs implementation 📱

## 📊 Sensor Data Usage

### Accelerometer
The accelerometer measures linear acceleration in 3 axes (X, Y, Z). Values are 16-bit signed integers.
- **Range**: Approximately ±4g (where 1g = 9.81 m/s²)
- **Use cases**: Tilt detection, motion sensing, orientation

```cpp
int16_t accel_x = ps5.data.sensor.accelerometer.x;
int16_t accel_y = ps5.data.sensor.accelerometer.y;
int16_t accel_z = ps5.data.sensor.accelerometer.z;
```

### Gyroscope
The gyroscope measures angular velocity in 3 axes (X, Y, Z). Values are 16-bit signed integers.
- **Range**: Approximately ±2000 degrees per second
- **Use cases**: Rotation detection, gesture recognition, stabilization

```cpp
int16_t gyro_x = ps5.data.sensor.gyroscope.x;
int16_t gyro_y = ps5.data.sensor.gyroscope.y;
int16_t gyro_z = ps5.data.sensor.gyroscope.z;
```

### Example Usage
```cpp
// Check for significant controller movement
if (abs(ps5.data.sensor.accelerometer.x) > 8000) {
    Serial.println("Controller tilted left/right");
}

// Check for rotation
if (abs(ps5.data.sensor.gyroscope.z) > 5000) {
    Serial.println("Controller spinning around Z-axis");
}
```

See `examples/ps5_sensors_example.ino` for a complete working example.

# 📡 Ground-Lora

[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange?style=for-the-badge)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-blue?style=for-the-badge)](https://arduino.cc/)

🎮 **ESP32-based Dual-Core LoRa Ground Control Station with PS5 Controller**

A sophisticated dual-core ground control station for remote aircraft control using LoRa communication and PS5 DualSense controller input. This project provides real-time flight control with OLED display feedback, robust wireless communication, and optimized dual-core performance for enhanced responsiveness and safety.

## 🌟 Features

### ⚡ Dual-Core Architecture
- **Core 1 (High Priority)**: Real-time PS5 controller processing and LoRa transmission at 20Hz
- **Core 0 (Lower Priority)**: OLED display updates and LoRa reception at 10Hz  
- **Thread Safety**: Mutex protection for shared data between cores
- **Performance Monitoring**: Real-time system health and performance statistics
- **Watchdog Protection**: Robust error handling to prevent system crashes

### 🎮 PS5 Controller Integration
- **Full DualSense Support**: Buttons, analog sticks, triggers, and IMU sensors
- **Flight Controls**: 
  - ↔️ Left stick X-axis → Aileron control
  - ↔️ Right stick X-axis → Rudder control  
  - ↕️ Right stick Y-axis → Elevator control
  - 🎚️ R2 trigger → Engine throttle
- **Auxiliary Controls**:
  - 🪶 L1/R1 → Flaps control (decrease/increase)
  - 🔄 L3/R3 → Reset aileron/elevator trim
  - 🛑 PS Button → Enable airbrake
  - ❌ Cross Button → Disable emergency stop
  - ⭕ Circle Button → Enable emergency stop
- **Safety Features**: Emergency stop override and safety interlocks

### 📡 LoRa Communication
- **915MHz ISM Band**: Long-range, low-power wireless communication
- **Packet Protocol**: Custom message format with checksum validation
- **Real-time Transmission**: 60ms update rate for responsive control
- **Data Optimization**: Smart packet filtering to reduce redundant transmissions

### 🖥️ OLED Display Interface
- **128x64 SSD1306 Display**: Real-time status and control feedback
- **Multi-frame UI**: Switchable display screens
- **Status Overlays**: 
  - 🔵 Bluetooth connection status
  - 🔋 Battery percentage display
  - ⚡ Charging indicator
  - 📶 WiFi status (when available)
- **Flight Data Display**: Live control surface positions and engine status

### 🛡️ Safety Systems
- **Emergency Stop**: Immediate control override capability
- **Connection Monitoring**: Automatic safety engagement on signal loss
- **Input Validation**: Checksum verification for all transmitted data
- **Redundant Safety**: Multiple layers of safety interlocks

## 🛠️ Hardware Requirements

### 📱 Main Components
- **ESP32 Development Board**: TTGO LoRa32 v2.1 (recommended)
- **LoRa Module**: 915MHz SX1276/SX1278
- **OLED Display**: 128x64 SSD1306 (I2C)
- **PS5 DualSense Controller**: Bluetooth connectivity

### 🔌 Pin Configuration
- **LoRa Module**: Uses board default pins (automatically configured)
- **OLED Display**: I2C (uses board default SDA/SCL pins)
- **Analog Input**: Pin 34 for manual throttle slider (optional)
- **Built-in LED**: Status indication

### 🏗️ System Architecture
- **FreeRTOS Tasks**: Dual-core task management with priority scheduling
- **Memory Management**: Optimized 8000-word stack size per task
- **Error Handling**: Graceful degradation and automatic recovery
- **Performance Optimization**: Load balancing across both ESP32 cores

## 📦 Dependencies

```ini
lib_deps = 
    thingpulse/ESP8266 and ESP32 OLED driver for SSD1306 displays@^4.5.0  ; 🖥️
    sandeepmistry/LoRa@^0.8.0   ; 📡
    ; PS5 Controller library included in lib/ folder 🎮
```

## 🚀 Quick Start

### 1. 🔧 Hardware Setup
1. Connect LoRa module to ESP32 (refer to pin definitions in `include/common.h`)
2. Connect OLED display via I2C
3. Power up the system

### 2. 📱 PS5 Controller Pairing
1. Find your PS5 controller's Bluetooth MAC address:
   - Connect controller to your phone via Bluetooth
   - Check "About" settings for the MAC address
2. Update the MAC address in `include/common.h`:
   ```cpp
   #define PS5_MAC_ADDRESS "xx:xx:xx:xx:xx:xx"  // 🎮 Your controller's MAC
   ```
3. Upload the code and the controller will automatically pair

### 3. 📤 Build and Upload
```bash
# Using PlatformIO
pio run --target upload

# Or use VS Code PlatformIO extension
# Click "Upload" button in PlatformIO toolbar
```

## 🎮 Control Mapping

| Control | Function | Description |
|---------|----------|-------------|
| 🕹️ **Left Stick X** | Aileron | ↔️ Roll control |
| 🕹️ **Right Stick X** | Rudder | ↔️ Yaw control |
| 🕹️ **Right Stick Y** | Elevator | ↕️ Pitch control |
| 🎚️ **R2 Trigger** | Engine | 🚀 Throttle control |
| ⬆️ **D-Pad Up** | Elevator Trim + | ↕️ Nose up trim |
| ⬇️ **D-Pad Down** | Elevator Trim - | ↕️ Nose down trim |
| ➡️ **D-Pad Right** | Aileron Trim + | ↔️ Right roll trim |
| ⬅️ **D-Pad Left** | Aileron Trim - | ↔️ Left roll trim |
| 🔴 **L1** | Flaps Down | 🪶 Decrease flaps |
| 🔴 **R1** | Flaps Up | 🪶 Increase flaps |
| 🔘 **L3** | Reset Aileron Trim | 🔄 Center aileron |
| 🔘 **R3** | Reset Elevator Trim | 🔄 Center elevator |
| ❌ **Cross** | Disable Emergency | 🔓 Enable flight |
| ⭕ **Circle** | Emergency Stop | 🚨 Immediate stop |
| 🏠 **PS Button** | Airbrake | 🛑 Enable airbrake |

## 📡 Communication Protocol

The system uses a custom LoRa packet format:
```
e[engine]a[aileron]r[rudder]l[elevator]t[trim]i[aileron_trim]f[flaps]z[reset_a]y[reset_e]b[airbrake]#[checksum]
```

### 📊 Data Ranges
- **Engine**: 0-180 (mapped from 0-4095 analog input)
- **Control Surfaces**: 0-180 (mapped from controller input)
- **Trim**: ±values for fine adjustment
- **Flaps**: 0-4 discrete positions
- **Checksum**: XOR validation byte

## 🖥️ Display Interface

### 📺 Main Screen (Frame 1)
- **Control Positions**: Live aileron, elevator, rudder values
- **Engine Status**: Throttle percentage with 🚀 indicator
- **Emergency Status**: 🚨 STOP indicator when active
- **Flaps Position**: Current flap setting 🪶
- **Developer Info**: 👨‍💻 Arsalan Iravani

### 📱 Status Overlays
- **🔵 Bluetooth**: Connection status indicator
- **🔋 Battery**: Controller battery percentage
- **⚡ Charging**: Active charging indicator
- **📶 WiFi**: Network status (when applicable)

## 🛡️ Safety Features

### 🚨 Emergency Systems
- **Automatic Stop**: Engine cuts to zero on emergency activation
- **Connection Loss**: Safety engagement if controller disconnects
- **Input Validation**: All commands verified with checksums
- **Redundant Controls**: Multiple ways to activate emergency stop

### ⚖️ Control Limits
- **Stick Deadzone**: Idle deviation threshold prevents accidental inputs
- **Rate Limiting**: Flap changes limited to prevent servo damage
- **Range Mapping**: All inputs mapped to safe servo ranges

## 📁 Project Structure

```
Ground Lora/
├── 📄 LICENSE                          # 📝 MIT License file
├── 📄 platformio.ini                   # 🔧 Build configuration with dual-core flags
├── 📄 README.md                        # 📚 This file
├── � include/                         # 📂 Header files directory
│   ├── 📄 common.h                     # � Shared definitions and pin config
│   ├── 📄 Display.h                    # 🖥️ Display interface
│   ├── 📄 images.h                     # �️ Display graphics and icons
│   ├── 📄 main.h                       # 🚀 Main function declarations
│   ├── 📄 PS5Joystick.h               # 🎮 Controller interface
│   └── 📄 SD-Card.h                   # 💾 SD card interface
├── 📂 src/                            # 📂 Source code directory
│   ├── 📄 main.cpp                     # 🚀 Main program with dual-core tasks
│   ├── 📄 Display.cpp                  # 🖥️ OLED display management
│   ├── 📄 Lora.cpp                     # 📡 LoRa communication
│   ├── 📄 PS5Joystick.cpp             # 🎮 Controller handling (no mutex)
│   └── 📄 SD-Card.cpp                 # 💾 SD card functionality
├── 📂 lib/                            # 📂 Libraries directory
│   └── 📂 PS5Library/                 # 🎮 PS5 controller library
│       ├── 📄 component.mk             # 📋 Component makefile
│       ├── 📄 Kconfig                  # ⚙️ Configuration file
│       ├── 📄 keywords.txt             # 🔤 Arduino IDE keywords
│       ├── 📄 library.properties       # 📚 Library properties
│       ├── 📄 PACKET_ANALYSIS.md       # 📡 Packet analysis documentation
│       ├── 📄 README.md                # � Library documentation
│       ├── 📄 SENSOR_IMPLEMENTATION.md # 📊 Sensor implementation details
│       ├── 📂 examples/                # 📝 Example implementations
│       │   ├── 📄 ps5_advanced_sensors.ino # 🧪 Advanced sensors example
│       │   ├── 📄 ps5_packet_debug.ino     # 🔍 Packet debugging example
│       │   └── 📄 ps5_sensors_example.ino  # � Basic sensors example
│       └── 📂 src/                     # 📂 Library source code
│           ├── 📄 ps5_int.h            # 🔗 Internal PS5 definitions
│           ├── 📄 ps5_l2cap.c          # 📡 L2CAP protocol implementation
│           ├── 📄 ps5_parser.c         # � Data parser implementation
│           ├── 📄 ps5_spp.c            # 📶 SPP protocol implementation
│           ├── 📄 ps5.c                # 🎮 Core PS5 functionality
│           ├── 📄 ps5.h                # 🎮 PS5 header file
│           ├── 📄 ps5Controller.cpp    # 🎮 Controller class implementation
│           ├── 📄 ps5Controller.h      # 🎮 Controller class header
│           ├── 📂 osi/                 # 📂 OS interface
│           │   └── 📄 allocator.h      # 💾 Memory allocator
│           └── 📂 stack/               # � Bluetooth stack
│               ├── 📄 bt_types.h       # 🔵 Bluetooth type definitions
│               ├── 📄 btm_api.h        # 🔵 BTM API definitions
│               ├── 📄 gap_api.h        # 🔵 GAP API definitions
│               ├── 📄 hcidefs.h        # 🔵 HCI definitions
│               ├── 📄 l2c_api.h        # 🔵 L2C API definitions
│               └── � l2cdefs.h        # 🔵 L2C definitions
├── �📂 Resources/                       # 📂 Resource files directory
│   ├── 📄 blIcon.png                   # 🔵 Bluetooth icon
│   ├── 📄 charging.png                 # ⚡ Charging indicator icon
│   ├── 📄 ps5 icon.png                 # 🎮 PS5 controller icon
│   ├── 📄 PS5-Controller-PNG-Image.png # 🎮 Controller image
│   └── 📄 wifiIcon.png                 # 📶 WiFi status icon
└── 📂 test/                           # 📂 Unit tests directory
    ├── 📄 README                       # 📚 Test documentation
    ├── 📄 test_config.py              # ⚙️ Test configuration file
    ├── 📂 test_display/                # 🖥️ Display tests
    │   └── 📄 test_display.cpp         # 🧪 Display unit tests
    ├── 📂 test_integration/            # 🔗 Integration tests
    │   └── 📄 test_integration.cpp     # 🧪 Integration unit tests
    ├── 📂 test_lora/                   # 📡 LoRa tests
    │   └── 📄 test_lora.cpp            # 🧪 LoRa unit tests
    ├── 📂 test_main/                   # 🚀 Main tests
    │   └── 📄 test_main.cpp            # 🧪 Main functionality tests
    ├── 📂 test_ps5/                    # 🎮 PS5 controller tests
    │   └── � test_ps5.cpp             # 🧪 PS5 controller unit tests
    ├── 📂 test_safety/                 # �️ Safety system tests
    │   └── 📄 test_safety.cpp          # 🧪 Safety feature tests
    └── 📂 test_utilities/              # 🔧 Utility tests
        └── 📄 test_utilities.cpp       # 🧪 Utility function tests
```

### 📋 Key Files Description

#### 🚀 **Core System Files**
- **`main.cpp`**: Dual-core task implementation with FreeRTOS
- **`common.h`**: Shared definitions, pin configurations, and extern declarations
- **`main.h`**: Function declarations for dual-core tasks and system functions

#### 🎮 **Controller Integration**
- **`PS5Joystick.cpp/.h`**: PS5 controller handling with optimized callback functions
- **`PS5Library/`**: Complete PS5 DualSense library with advanced features
  - **`ps5Controller.cpp/.h`**: Main controller class implementation
  - **`ps5.c/.h`**: Core PS5 functionality and protocol handling
  - **`examples/`**: Sample implementations for various PS5 features

#### 🖥️ **Display System**
- **`Display.cpp/.h`**: OLED display management with frame-based UI
- **`images.h`**: Graphics definitions for icons and symbols
- **`Resources/`**: Icon and image assets for the display interface

#### 📡 **Communication**
- **`Lora.cpp`**: LoRa communication with packet protocol and checksums
- **`SD-Card.cpp/.h`**: SD card functionality for data logging

#### 🧪 **Testing Framework**
- **`test/`**: Comprehensive unit testing suite
  - **`test_config.py`**: Python configuration for test automation
  - **`test_display/`**: Display functionality tests
  - **`test_integration/`**: System integration tests
  - **`test_lora/`**: LoRa communication tests
  - **`test_main/`**: Core functionality tests
  - **`test_ps5/`**: PS5 controller tests
  - **`test_safety/`**: Safety system validation tests
  - **`test_utilities/`**: Utility function tests

#### 📚 **Project Configuration**
- **`platformio.ini`**: Build configuration with dual-core optimization flags
- **`LICENSE`**: MIT License file
- **`README.md`**: This comprehensive documentation

## 🔧 Development Setup

### Prerequisites
- **PlatformIO**: VS Code extension or CLI
- **ESP32 Toolchain**: Automatically installed by PlatformIO (dual-core enabled)
- **PS5 Controller**: DualSense controller with Bluetooth
- **Hardware**: TTGO LoRa32 or compatible ESP32 board with LoRa module

### 🛠️ Building
```bash
# Clone the repository
git clone https://github.com/Arsalan134/Ground-Lora.git
cd Ground-Lora

# Build the project (with dual-core support)
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output (view dual-core performance stats)
pio device monitor
```

### 📊 Build Configuration
The project is configured for dual-core operation with optimized flags:
```ini
build_flags = 
    -std=gnu++17
    -DCONFIG_FREERTOS_NUMBER_OF_CORES=2
```

## 📊 Performance

### ⚡ Dual-Core Performance
- **Core 1 (Controller)**: 20Hz update rate for responsive control input
- **Core 0 (Display)**: 10Hz update rate for smooth UI without blocking control
- **LoRa Transmission**: ~16.7Hz with smart packet optimization
- **System Health**: Monitored every 30 seconds with performance statistics

### 📈 System Metrics
- **RAM Usage**: ~12% of 320KB (highly efficient)
- **Flash Usage**: ~87% with all features enabled
- **Stack Usage**: 8000 words per task with monitoring
- **Heap Monitoring**: Real-time memory leak detection
- **Range**: Several kilometers (LoRa 915MHz)
- **Latency**: <50ms typical (improved with dual-core)

## 🐛 Troubleshooting

### 🎮 Controller Issues
- **Pairing Failed**: Check MAC address in `src/Common/common.h`
- **No Response**: Ensure controller is charged and in pairing mode
- **Input Lag**: Check for Bluetooth interference, dual-core should minimize lag

### 📡 LoRa Issues
- **Init Failed**: Check antenna connections, system will continue without LoRa after 10 retries
- **Poor Range**: Verify antenna positioning and frequency settings
- **Packet Loss**: Check for interference on 915MHz band

### 🖥️ Display Issues
- **Blank Screen**: Verify I2C connections (uses board default pins)
- **Corrupted Display**: Check power supply stability
- **Slow Updates**: Normal with dual-core - Core 0 handles display at 10Hz

### ⚡ System Stability
- **Watchdog Timeout**: See `WATCHDOG_TIMEOUT_FIX.md` for detailed solutions
- **Core Crashes**: Check serial output for mutex warnings and stack usage
- **Memory Issues**: Monitor heap usage in performance statistics
- **Task Failures**: Verify stack high-water marks in health checks

### 🔧 Performance Optimization
- **Check Serial Output**: Look for dual-core performance statistics every 5 seconds
- **Monitor System Health**: Health checks appear every 30 seconds
- **Stack Monitoring**: Ensure >1000 words free space per task
- **Heap Memory**: Should stay >100KB for stable operation

## 🤝 Contributing

1. 🍴 Fork the repository
2. 🌿 Create a feature branch (`git checkout -b feature/amazing-feature`)
3. 💾 Commit your changes (`git commit -m '✨ Add amazing feature'`)
4. 📤 Push to the branch (`git push origin feature/amazing-feature`)
5. 🔀 Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

**MIT License Summary:**
- ✅ Commercial use allowed
- ✅ Modification allowed  
- ✅ Distribution allowed
- ✅ Private use allowed
- ⚖️ Liability and warranty disclaimed

## 👨‍💻 Author

**Arsalan Iravani**
- 🌐 GitHub: [@Arsalan134](https://github.com/Arsalan134)

## 🔗 Related Projects

- **Airplane**: Companion dual-core receiver project for aircraft-side control
- **PS5-ESP32**: Enhanced PS5 controller library with dual-core support (included in lib/)

## 📚 Additional Documentation

- 📋 **[Dual-Core Implementation Guide](README_DUAL_CORE.md)**: Complete architecture documentation
- 🛡️ **[Watchdog Timeout Fix](WATCHDOG_TIMEOUT_FIX.md)**: System stability troubleshooting  
- ✅ **[Implementation Summary](DUAL_CORE_IMPLEMENTATION_SUMMARY.md)**: Development status and metrics

## ⚠️ Disclaimer

This is a dual-core remote control system for model aircraft. Always follow local regulations and safety guidelines when operating remote-controlled aircraft. The dual-core architecture provides enhanced safety features, but proper operation is still the responsibility of the user. The author is not responsible for any accidents or damages resulting from the use of this system.

---

🚀 **Happy Flying!** ✈️

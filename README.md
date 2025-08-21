# 📡 Ground-Lora

[![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)
[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange?style=for-the-badge)](https://platformio.org/)
[![Arduino](https://img.shields.io/badge/Framework-Arduino-blue?style=for-the-badge)](https://arduino.cc/)

🎮 **ESP32-based LoRa Ground Control Station with PS5 Controller**

A sophisticated ground control station for remote aircraft control using LoRa communication and PS5 DualSense controller input. This project provides real-time flight control with OLED display feedback and robust wireless communication.

## 🌟 Features

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
- **LoRa Module**: CS, RST, IRQ pins (defined in common.h)
- **Display**: I2C (SDA/SCL)
- **Analog Input**: Pin 34 for manual throttle slider (optional)

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
Ground-Lora/
├── 📄 platformio.ini          # 🔧 Build configuration
├── 📂 include/
│   ├── 📄 common.h            # 📌 Shared definitions
│   ├── 📄 main.h              # 🚀 Main function declarations
│   ├── 📄 Display.h           # 🖥️ Display interface
│   └── 📄 PS5Joystick.h       # 🎮 Controller interface
├── 📂 src/
│   ├── 📄 main.cpp            # 🚀 Main program entry
│   ├── 📄 PS5Joystick.cpp     # 🎮 Controller handling
│   ├── 📄 Display.cpp         # 🖥️ OLED display management
│   ├── 📄 Lora.cpp            # 📡 LoRa communication
│   └── 📄 SD-Card.cpp         # 💾 SD card (unused)
├── 📂 lib/
│   └── 📂 PS5Library/         # 🎮 PS5 controller library
└── 📂 test/                   # 🧪 Unit tests
```

## 🔧 Development Setup

### Prerequisites
- **PlatformIO**: VS Code extension or CLI
- **ESP32 Toolchain**: Automatically installed by PlatformIO
- **PS5 Controller**: DualSense controller with Bluetooth

### 🛠️ Building
```bash
# Clone the repository
git clone https://github.com/Arsalan134/Ground-Lora.git
cd Ground-Lora

# Build the project
pio run

# Upload to ESP32
pio run --target upload

# Monitor serial output
pio device monitor
```

## 📊 Performance

- **Update Rate**: 60ms control loop (16.7 Hz)
- **Range**: Several kilometers (LoRa 915MHz)
- **Latency**: <100ms typical
- **Battery Life**: Depends on PS5 controller (8-12 hours typical)

## 🐛 Troubleshooting

### 🎮 Controller Issues
- **Pairing Failed**: Check MAC address in `common.h`
- **No Response**: Ensure controller is charged and in pairing mode
- **Lag**: Check for Bluetooth interference

### 📡 LoRa Issues
- **Init Failed**: Check wiring and antenna connections
- **Poor Range**: Verify antenna positioning and frequency settings
- **Packet Loss**: Check for interference on 915MHz band

### 🖥️ Display Issues
- **Blank Screen**: Verify I2C connections (SDA/SCL)
- **Corrupted Display**: Check power supply stability

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

- **Airplane**: Companion receiver project for aircraft-side control
- **PS5-ESP32**: Base PS5 controller library (included in lib/)

## ⚠️ Disclaimer

This is a remote control system for model aircraft. Always follow local regulations and safety guidelines when operating remote-controlled aircraft. The author is not responsible for any accidents or damages resulting from the use of this system.

---

🚀 **Happy Flying!** ✈️

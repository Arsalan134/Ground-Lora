#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

// pins 📌
#define SDA 18
#define SCL 17

#define sliderPin 12

// SX1262 LoRa Module Pins 📡
#define LORA_CS 7     // Chip Select (NSS)
#define LORA_SCK 5    // SPI Clock
#define LORA_MISO 3   // SPI MISO
#define LORA_MOSI 6   // SPI MOSI
#define LORA_D1 33    // DIO1 (IRQ)
#define LORA_BUSY 34  // BUSY pin (required for SX1262)
#define LORA_RST 8    // Reset pin

// PS5 Controller 🎮
#define PS5_MAC_ADDRESS "ac:36:1b:41:ac:ed"

extern OLEDDisplayUi display;  // 🖥️ Display UI

extern OverlayCallback allOverlays[];  // 📱 Display overlays

// 🎮 Controller input variables
extern int sendingEngineMessage;        // 🚀 Engine throttle
extern byte sendingAileronMessage;      // ↔️ Aileron control
extern byte sendingRudderMessage;       // ↔️ Rudder control
extern byte sendingElevatorsMessage;    // ↕️ Elevator control
extern int sendingElevatorTrimMessage;  // ⚖️ Elevator trim
extern int sendingAileronTrimMessage;   // ⚖️ Aileron trim
extern int sendingFlapsMessage;         // 🪶 Flaps position
extern bool resetAileronTrim;           // 🔄 Reset aileron trim
extern bool resetElevatorTrim;          // 🔄 Reset elevator trim

extern bool isEmergencyStopEnabled;  // 🚨 Emergency stop status
extern bool airbrakeEnabled;         // 🛑 Airbrake status

extern uint8_t batteryPercentage;  // 🔋 Battery level

#define idleDeviationThreshold 10  // 🎚️ Joystick idle threshold

// enum class FlightMode { MANUAL = 0,
//                         TAKEOFF = 1,
//                         STABILITY = 2,
//                         AUTOPILOT = 3,
//                         LANDING = 4 };  // 🛩️

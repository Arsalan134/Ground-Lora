#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

// pins 📌
#define SDA_PIN 21
#define SCL_PIN 22

#define sliderPin 34

// TTGO LoRa32 V2.1 Built-in SX1276 Pins 📡
#define LORA_CS 18    // Chip Select (NSS)
#define LORA_RST 23   // Reset pin
#define LORA_DIO0 26  // DIO0 (IRQ - RX/TX done)
// SPI uses default VSPI: SCK=5, MISO=19, MOSI=27

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

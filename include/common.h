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
extern bool acsEngageEnabled;        // 🤖 ACS autopilot engage
extern uint8_t stabilityAssistValue;  // 🛡️ Stability assist L2 analog 0-255
extern bool ecoModeEnabled;           // 🌿 ECO mode: suppress duplicate packets (default ON)

extern uint8_t batteryPercentage;  // 🔋 Battery level

// ⏱️ Flight timer (armed time)
extern unsigned long flightTimerStartMs;  // millis() when armed (0 = not armed)
extern bool flightTimerRunning;           // true while armed

// 🎮 Expo/Rates Configuration
// Expo: 0.0 = linear, 1.0 = maximum curve (fine near center, sharp at edges)
// Rate: 0.0–1.0 = fraction of full servo travel (1.0 = full 0-180)
// Expo values are runtime-variable — cycle presets with Square 🟥
extern float expoAileron;   // Aileron expo factor
extern float expoElevator;  // Elevator expo factor
extern float expoRudder;    // Rudder expo factor
extern uint8_t expoPresetIndex;         // 0=LOW 1=MED 2=HIGH
extern const char* const expoPresetNames[];  // preset label strings
void cycleExpoPreset();  // 🟥 Square: advance to next expo preset
#define RATE_AILERON  1.0f    // Aileron rate (1.0 = full throw)
#define RATE_ELEVATOR 1.0f    // Elevator rate (1.0 = full throw)
#define RATE_RUDDER   0.8f    // Rudder rate (0.8 = 80% throw)

// idleDeviationThreshold is now in protocol.h as PROTO_IDLE_THRESHOLD

enum class FlightMode : uint8_t {
  MANUAL = 0,
  TAKEOFF = 1,
  STABILITY = 2,
  AUTOPILOT = 3,
  LANDING = 4
};  // 🛩️

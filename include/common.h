#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

// pins 📌
#define sliderPin 34

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
#include <Arduino.h>
#include "common.h"
#include "protocol.h"

// PS5 Controller 🎮
#include <ps5Controller.h>
void setupPS5();             // 🎮 Initialize PS5 controller
void onConnect();            // ✅ PS5 connect callback
void notify();               // 📡 PS5 input handler
void onDisconnect();         // ❌ PS5 disconnect callback
void removePairedDevices();  // 🧹 Clear Bluetooth pairings
void printDeviceAddress();   // 📱 Print device MAC

// Display 🖥️
#include "Display.h"
int frameCount = 1;     // 🖼️ Number of display frames
int overlaysCount = 1;  // 📱 Number of display overlays
void setupDisplay();    // 🖥️ Initialize OLED display

// LoRa Communication 📡 (parameters from protocol.h)
#include <LoRa.h>
boolean runEvery(unsigned long interval);                  // ⏰ Timer function
void setupRadio();                                         // 📡 Initialize LoRa radio
void loraLoop();                                           // 📡 Main LoRa communication loop
void LoRa_sendMessage(const char* message);                // 📡 Send LoRa message (zero-alloc)

extern bool lora_initialized;  // 📡 LoRa init status

// 📊 Telemetry from flight board (read-only, updated by LoRa RX)
extern float tlm_altitude;
extern float tlm_pressure;
extern int   tlm_rssi;
extern float tlm_gforce;
extern float tlm_temperature;
extern float tlm_verticalSpeed;
extern bool  tlm_valid;
extern unsigned long tlm_lastReceived;
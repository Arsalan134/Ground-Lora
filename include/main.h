#include <Arduino.h>
#include "common.h"

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

// LoRa Communication 📡
#include <LoRa.h>
const long LORA_FREQUENCY_HZ = 915E6;                     // 📡 LoRa Frequency (915MHz)
const long LORA_BANDWIDTH_HZ = 125E3;                     // 📡 Bandwidth (125kHz)
const int LORA_SF = 7;                                     // 📡 Spreading Factor
const int LORA_CR = 5;                                     // 📡 Coding Rate (4/5)
const int LORA_SW = 0x12;                                  // 📡 Sync Word
const int LORA_POWER = 17;                                 // 📡 TX Power (dBm, max 17 for SX1276)
const int LORA_PREAMBLE = 8;                               // 📡 Preamble Length
boolean runEvery(unsigned long interval);                  // ⏰ Timer function
void setupRadio();                                         // 📡 Initialize LoRa radio
void loraLoop();                                           // 📡 Main LoRa communication loop
uint8_t simple_checksum(const uint8_t* data, size_t len);  // 🔐 Checksum calculator
void LoRa_sendMessage(String message);                     // 📡 Send LoRa message

extern bool lora_initialized;  // 📡 LoRa init status
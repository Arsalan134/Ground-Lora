#include <LoRa.h>
#include <SPI.h>
#include "common.h"

bool lora_initialized = false;  // 📡 Track init status

// 📡 LoRa Communication Variables
int sendingEngineMessage = 1;
byte sendingAileronMessage = 127;    // ↔️ Aileron control
byte sendingRudderMessage = 127;     // ↔️ Rudder control
byte sendingElevatorsMessage = 127;  // ↕️ Elevator control
int sendingElevatorTrimMessage = 0;
int sendingAileronTrimMessage = 0;
int sendingFlapsMessage = 0;  // 🪶 Flaps: 0, 1, 2, 3, 4
bool resetAileronTrim = false;
bool resetElevatorTrim = false;
bool airbrakeEnabled = false;  // 🛑 Airbrake status

void LoRa_sendMessage(String message) {
  if (!lora_initialized)
    return;  // ⚠️ Skip if LoRa not initialized

  digitalWrite(BUILTIN_LED, 1);  // 💡 Turn on LED during transmission

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();  // 📡 Blocking mode - wait for TX to complete

  digitalWrite(BUILTIN_LED, 0);  // 💡 Turn off LED after transmission
  delay(5);                      // Small delay to ensure clean packet separation
}

boolean runEvery(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;  // ⏰ Time to execute
  }

  return false;  // ⏳ Wait more
}

String message = "";
byte previousChecksum = 0;

int samePacketCount = 0;

byte simple_checksum(const byte* data, size_t len) {
  byte sum = 0;  // 🧮 Checksum calculation
  for (size_t i = 0; i < len; i++) {
    sum ^= data[i];
  }
  return sum;
}

void constructMessage() {
  message = "e" + String(isEmergencyStopEnabled ? 0 : map(sendingEngineMessage, 0, 4095, 0, 180));  // 🚀 "e" is used for engine
  message += "a" + String(map(sendingAileronMessage, 0, 255, 0, 180));                              // ↔️ "a" is used for ailerons
  message += "r" + String(map(sendingRudderMessage, 0, 255, 0, 180));                               // ↔️ "r" is used for rudder
  message += "l" + String(map(sendingElevatorsMessage, 0, 255, 0, 180));                            // ↕️ "l" is used for elevators
  message += "t" + String(sendingElevatorTrimMessage);                                              // ⚖️ "t" is used for trim
  message += "i" + String(sendingAileronTrimMessage);                                               // ⚖️ "i" is used for aileron trim
  message += "f" + String(sendingFlapsMessage);                                                     // 🪶 "f" is used for flaps
  message += "z" + String(resetAileronTrim ? 1 : 0);                                                // 🔄 "z" is used for reset aileron trim
  message += "y" + String(resetElevatorTrim ? 1 : 0);                                               // 🔄 "y" is used for reset elevator trim
  message += "b" + String(airbrakeEnabled ? 1 : 0);                                                 // 🛑 "b" is used for airbrake
  message += "#";                                                                                   // 📌 End delimiter (hardware CRC validates integrity)
}

void loraLoop() {
  if (!lora_initialized)
    return;  // ⚠️ Skip if LoRa not initialized

  if (runEvery(120)) {  // 📡 Send every 120ms (more time for RX processing)
    constructMessage();

    int aileronDeviation = abs(sendingAileronMessage - 127);      // ↔️ Aileron deviation from center
    int rudderDeviation = abs(sendingRudderMessage - 127);        // ↔️ Rudder deviation from center
    int elevatorsDeviation = abs(sendingElevatorsMessage - 127);  // ↕️ Elevator deviation from center
    int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

    // Use simple hash for duplicate detection (hardware CRC validates integrity)
    byte currentHash = message.length() ^ sendingEngineMessage ^ sendingAileronMessage;

    // Skip sending if the same packet is sent multiple times 📦
    if (currentHash == previousChecksum && samePacketCount >= 10 &&
        totalDeviation < idleDeviationThreshold) {  // only if joysticks are in neutral position 🕹️
      return;
    }

    LoRa_sendMessage(message);  // 📡 send a message (hardware CRC auto-calculated)

    // Reduced serial output - print every 10th packet
    static int printCount = 0;
    if (++printCount >= 10) {
      Serial.println("📡 TX: " + message);
      printCount = 0;
    }

    if (currentHash == previousChecksum)
      samePacketCount++;  // 📈 Increment duplicate count
    else
      samePacketCount = 0;  // 🔄 Reset duplicate count

    previousChecksum = currentHash;  // 💾 Store for comparison

    // Reset messages
    message = "";                    // Clear message buffer
    sendingElevatorTrimMessage = 0;  // 🔄 Reset trim messages
    sendingAileronTrimMessage = 0;   // 🔄 Reset trim messages
    resetAileronTrim = false;
    resetElevatorTrim = false;
  }
}
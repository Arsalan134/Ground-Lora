#include <LoRa.h>
#include "common.h"

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

void LoRa_rxMode() {
  LoRa.enableInvertIQ();  // 🔄 active invert I and Q signals
  LoRa.receive();         // 📥 set receive mode
}

void LoRa_txMode() {
  LoRa.idle();             // ⏸️ set standby mode
  LoRa.disableInvertIQ();  // ⚡ normal mode
}

void LoRa_sendMessage(String message) {
  digitalWrite(BUILTIN_LED, 1);  // 💡 Turn on LED during transmission

  LoRa.beginPacket();    // 📦 start packet
  LoRa.print(message);   // 📝 add payload
  LoRa.endPacket(true);  // 🚀 finish packet and send it (blocking mode)

  digitalWrite(BUILTIN_LED, 0);  // 💡 Turn off LED after transmission
}

void onReceive(int packetSize) {
  String message = "";  // 📥 Received message buffer

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
}

void onTxDone() {
  // Transmission complete - nothing to do for TX-only mode
  digitalWrite(BUILTIN_LED, 0);  // 💡 Turn off LED after transmission
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
  message += "y" + String(resetElevatorTrim ? 1 : 0);  // 🔄 "y" is used for reset elevator trim
  message += "b" + String(airbrakeEnabled ? 1 : 0);    // 🛑 "b" is used for airbrake
}

void loraLoop() {
  if (runEvery(100)) {  // 📡 Send every 100ms
    constructMessage();

    int aileronDeviation = abs(sendingAileronMessage - 127);      // ↔️ Aileron deviation from center
    int rudderDeviation = abs(sendingRudderMessage - 127);        // ↔️ Rudder deviation from center
    int elevatorsDeviation = abs(sendingElevatorsMessage - 127);  // ↕️ Elevator deviation from center
    int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

    byte checksum = simple_checksum((const byte*)message.c_str(), message.length());  // 🔐 Calculate checksum

    // Add message delimiter and checksum
    message += "#";       // 📌 Message delimiter
    message += checksum;  // 🔐 Add checksum

    // Skip sending if the same packet is sent multiple times 📦
    if (checksum == previousChecksum && samePacketCount >= 10 &&
        totalDeviation < idleDeviationThreshold) {  // only if joysticks are in neutral position 🕹️
      return;
    }

    // DEBUG: Print what we're sending
    Serial.print("📤 TX [len=");
    Serial.print(message.length());
    Serial.print("]: ");
    Serial.println(message);

    LoRa_sendMessage(message);  // 📡 send a message

    if (checksum == previousChecksum)
      samePacketCount++;  // 📈 Increment duplicate count
    else
      samePacketCount = 0;  // 🔄 Reset duplicate count

    previousChecksum = checksum;  // 💾 Store for comparison

    // Reset messages
    message = "";                    // Clear message buffer
    sendingElevatorTrimMessage = 0;  // 🔄 Reset trim messages
    sendingAileronTrimMessage = 0;   // 🔄 Reset trim messages
    resetAileronTrim = false;
    resetElevatorTrim = false;
  }
}
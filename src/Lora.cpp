#include <RadioLib.h>
#include "common.h"

// 📡 SX1262 Radio Instance with pin configuration
SX1262 radio = new Module(LORA_CS, LORA_D1, LORA_RST, LORA_BUSY);

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
  radio.startReceive();  // 📥 Start receive mode
}

void LoRa_txMode() {
  radio.standby();  // ⏸️ Set standby mode
}

void LoRa_sendMessage(String message) {
  digitalWrite(BUILTIN_LED, 1);  // 💡 Turn on LED during transmission

  // Transmit packet using RadioLib (BLOCKING mode - no interrupts)
  int state = radio.transmit(message);

  digitalWrite(BUILTIN_LED, 0);  // 💡 Turn off LED after transmission

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("📡 TX failed, code: ");
    Serial.println(state);
  }
}

// Not used anymore - disabled interrupts to prevent watchdog timeout
#if defined(ESP8266) || defined(ESP32)
ICACHE_RAM_ATTR
#endif
void onTxDone(void) {
  // Serial.println("📡 TxDone");
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
  message += "y" + String(resetElevatorTrim ? 1 : 0);                                               // 🔄 "y" is used for reset elevator trim
  message += "b" + String(airbrakeEnabled ? 1 : 0);                                                 // 🛑 "b" is used for airbrake
}

void loraLoop() {
  if (runEvery(60)) {  // 📡 Send every 60ms
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
    // if (checksum == previousChecksum && samePacketCount >= 10 &&
    //     totalDeviation < idleDeviationThreshold) {  // only if joysticks are in neutral position 🕹️
    //   return;
    // }

    LoRa_sendMessage(message);  // 📡 send a message

    Serial.println("📡 LoRa Send: " + message);

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
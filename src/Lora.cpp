#include <LoRa.h>
#include <SPI.h>
#include "common.h"
#include "protocol.h"

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

#ifdef PROTO_BIDIRECTIONAL
// 📊 Telemetry data received from flight board (Air → Ground)
float tlm_altitude = 0.0f;
float tlm_pressure = 0.0f;
int tlm_rssi = 0;
float tlm_gforce = 0.0f;
float tlm_temperature = 0.0f;
float tlm_verticalSpeed = 0.0f;
bool tlm_valid = false;
unsigned long tlm_lastReceived = 0;
#endif

// 📡 Fixed-size message buffer (no heap allocation)
static char msgBuffer[PROTO_MSG_BUF_SIZE];

void LoRa_sendMessage(const char* message) {
  if (!lora_initialized)
    return;  // ⚠️ Skip if LoRa not initialized

  digitalWrite(BUILTIN_LED, 1);  // 💡 Turn on LED during transmission

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket(true);  // 📡 Async mode - non-blocking TX

  digitalWrite(BUILTIN_LED, 0);  // 💡 Turn off LED after transmission
  // No delay needed - async TX handles packet separation
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

uint32_t previousHash = 0;
int samePacketCount = 0;

// 🧮 FNV-1a hash — much lower collision rate than XOR
static uint32_t fnv1a_hash(const char* data, size_t len) {
  uint32_t hash = 2166136261u;  // FNV offset basis
  for (size_t i = 0; i < len; i++) {
    hash ^= (uint8_t)data[i];
    hash *= 16777619u;  // FNV prime
  }
  return hash;
}

// 📦 Build command packet into fixed buffer (zero heap allocation)
void constructMessage() {
  int engineVal = isEmergencyStopEnabled ? 0 : (int)map(sendingEngineMessage, PROTO_ENGINE_RAW_MIN, PROTO_ENGINE_RAW_MAX, PROTO_ENGINE_MIN, PROTO_ENGINE_MAX);
  snprintf(msgBuffer, sizeof(msgBuffer),
           "%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c%d%c",
           PROTO_KEY_ENGINE, engineVal,
           PROTO_KEY_AILERONS, (int)map(sendingAileronMessage, PROTO_JOYSTICK_MIN, PROTO_JOYSTICK_MAX, PROTO_SERVO_MIN, PROTO_SERVO_MAX),
           PROTO_KEY_RUDDER, (int)map(sendingRudderMessage, PROTO_JOYSTICK_MIN, PROTO_JOYSTICK_MAX, PROTO_SERVO_MIN, PROTO_SERVO_MAX),
           PROTO_KEY_ELEVATORS, (int)map(sendingElevatorsMessage, PROTO_JOYSTICK_MIN, PROTO_JOYSTICK_MAX, PROTO_SERVO_MIN, PROTO_SERVO_MAX),
           PROTO_KEY_ELEV_TRIM, sendingElevatorTrimMessage,
           PROTO_KEY_AIL_TRIM, sendingAileronTrimMessage,
           PROTO_KEY_FLAPS, sendingFlapsMessage,
           PROTO_KEY_RESET_AIL, resetAileronTrim ? 1 : 0,
           PROTO_KEY_RESET_ELEV, resetElevatorTrim ? 1 : 0,
           PROTO_KEY_AIRBRAKE, airbrakeEnabled ? 1 : 0,
           PROTO_CMD_END);
}

#ifdef PROTO_BIDIRECTIONAL
// 📊 Parse telemetry packet from flight board
static void parseTelemetry(const char* msg, int len) {
  if (len < PROTO_TLM_MIN_LEN || msg[0] != PROTO_KEY_TELEMETRY || msg[len - 1] != PROTO_TLM_END)
    return;

  // Simple float parser for telemetry fields
  auto findField = [&](char key) -> float {
    for (int i = 0; i < len; i++) {
      if (msg[i] == key) {
        return atof(&msg[i + 1]);
      }
    }
    return 0.0f;
  };

  tlm_altitude = findField(PROTO_KEY_TELEMETRY);
  tlm_pressure = findField(PROTO_KEY_PRESSURE);
  tlm_rssi = (int)findField(PROTO_KEY_RSSI);
  tlm_gforce = findField(PROTO_KEY_GFORCE);
  tlm_temperature = findField(PROTO_KEY_TEMP);
  tlm_verticalSpeed = findField(PROTO_KEY_SPEED);
  tlm_valid = true;
  tlm_lastReceived = millis();
}

// 📡 Check for incoming telemetry from flight board
static void checkTelemetry() {
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0)
    return;

  char tlmBuf[PROTO_TLM_BUF_SIZE];
  int idx = 0;
  while (LoRa.available() && idx < (int)sizeof(tlmBuf) - 1) {
    tlmBuf[idx++] = (char)LoRa.read();
  }
  tlmBuf[idx] = '\0';

  parseTelemetry(tlmBuf, idx);

  static int tlmCount = 0;
  if (++tlmCount % 5 == 0) {
    Serial.printf("📊 TLM: Alt=%.1fm T=%.1f°C RSSI=%d G=%.1f\n",
                  tlm_altitude, tlm_temperature, tlm_rssi, tlm_gforce);
  }
}
#endif

void loraLoop() {
  if (!lora_initialized)
    return;  // ⚠️ Skip if LoRa not initialized

#ifdef PROTO_BIDIRECTIONAL
  // 📊 Check for incoming telemetry from flight board
  checkTelemetry();
#endif

  if (runEvery(PROTO_CMD_INTERVAL_MS)) {  // 📡 Send every 60ms
    constructMessage();

    int aileronDeviation = abs(sendingAileronMessage - PROTO_JOYSTICK_CENTER);
    int rudderDeviation = abs(sendingRudderMessage - PROTO_JOYSTICK_CENTER);
    int elevatorsDeviation = abs(sendingElevatorsMessage - PROTO_JOYSTICK_CENTER);
    int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

    // 🧮 FNV-1a hash on full message for accurate duplicate detection
    size_t msgLen = strlen(msgBuffer);
    uint32_t currentHash = fnv1a_hash(msgBuffer, msgLen);

    // Skip sending if the same packet is sent multiple times 📦
    if (currentHash == previousHash && samePacketCount >= PROTO_DUPLICATE_LIMIT &&
        totalDeviation < PROTO_IDLE_THRESHOLD) {
      return;
    }

    LoRa_sendMessage(msgBuffer);  // 📡 Send command (async, non-blocking)

    // Reduced serial output - print every 10th packet
    static int printCount = 0;
    if (++printCount >= 10) {
      Serial.printf("📡 TX: %s\n", msgBuffer);
      printCount = 0;
    }

    if (currentHash == previousHash)
      samePacketCount++;  // 📈 Increment duplicate count
    else
      samePacketCount = 0;  // 🔄 Reset duplicate count

    previousHash = currentHash;  // 💾 Store for comparison

    // Reset one-shot messages
    sendingElevatorTrimMessage = 0;
    sendingAileronTrimMessage = 0;
    resetAileronTrim = false;
    resetElevatorTrim = false;
  }
}
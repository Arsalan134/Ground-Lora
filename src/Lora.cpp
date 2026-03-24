#include <LoRa.h>
#include <SPI.h>
#include <math.h>
#include "common.h"
#include "protocol.h"

bool lora_initialized = false;  // 📡 Track init status
bool ecoModeEnabled = true;     // 🌿 ECO mode: suppress duplicate packets (default ON)

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
bool acsEngageEnabled = false; // 🤖 ACS autopilot engage
uint8_t stabilityAssistValue = 0; // 🛡️ Stability assist L2 analog

// ⏱️ Flight timer
unsigned long flightTimerStartMs = 0;
bool flightTimerRunning = false;

// 🟥 Expo presets — cycled via Square button
// Columns: {aileron, elevator, rudder}
static const float expoPresets[3][3] = {
  {0.10f, 0.10f, 0.05f},  // 0: LOW  — nearly linear
  {0.35f, 0.30f, 0.20f},  // 1: MED  — default
  {0.55f, 0.50f, 0.35f},  // 2: HIGH — strong curve
};
const char* const expoPresetNames[] = {"LOW", "MED", "HIGH"};
uint8_t expoPresetIndex = 1;  // default: MED
float expoAileron  = 0.35f;
float expoElevator = 0.30f;
float expoRudder   = 0.20f;

void cycleExpoPreset() {
  expoPresetIndex = (expoPresetIndex + 1) % 3;
  expoAileron  = expoPresets[expoPresetIndex][0];
  expoElevator = expoPresets[expoPresetIndex][1];
  expoRudder   = expoPresets[expoPresetIndex][2];
}

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

// � Binary command packet buffer (10 bytes — was 48 byte ASCII buffer)
static ProtoCmdPacket cmdPacket;
// 🎮 Expo/Rates: apply exponential curve to joystick input.
// Input:  raw joystick byte 0–255 (127/128 = center)
// Output: servo value 0–180 (90 = center) with expo + rate applied.
// Formula: out = (1-expo)*x + expo*x³  where x ∈ [-1,+1]
static uint8_t applyExpoRate(byte rawJoystick, float expo, float rate) {
  // Normalize to [-1, +1]
  float x = ((float)rawJoystick - 127.5f) / 127.5f;
  x = constrain(x, -1.0f, 1.0f);

  // Expo curve: blend linear and cubic
  float curved = (1.0f - expo) * x + expo * x * x * x;

  // Apply rate (scales the throw)
  curved *= rate;
  curved = constrain(curved, -1.0f, 1.0f);

  // Map back to servo range 0–180 (90 = center)
  float servo = 90.0f + curved * 90.0f;
  return (uint8_t)constrain((int)roundf(servo), 0, 180);
}
void LoRa_sendPacket(const uint8_t* data, size_t len) {
  if (!lora_initialized)
    return;  // ⚠️ Skip if LoRa not initialized

  digitalWrite(BUILTIN_LED, 1);  // 💡 Turn on LED during transmission

  LoRa.beginPacket();
  LoRa.write(data, len);
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
static uint32_t fnv1a_hash(const uint8_t* data, size_t len) {
  uint32_t hash = 2166136261u;  // FNV offset basis
  for (size_t i = 0; i < len; i++) {
    hash ^= data[i];
    hash *= 16777619u;  // FNV prime
  }
  return hash;
}

// 📦 Build binary command packet (10 bytes, zero heap allocation)
void constructMessage() {
  cmdPacket.magic = PROTO_CMD_MAGIC;
  cmdPacket.engine = isEmergencyStopEnabled ? 0 : (uint8_t)map(sendingEngineMessage, PROTO_ENGINE_RAW_MIN, PROTO_ENGINE_RAW_MAX, PROTO_ENGINE_MIN, PROTO_ENGINE_MAX);
  cmdPacket.ailerons = applyExpoRate(sendingAileronMessage, expoAileron, RATE_AILERON);
  cmdPacket.rudder = applyExpoRate(sendingRudderMessage, expoRudder, RATE_RUDDER);
  cmdPacket.elevators = applyExpoRate(sendingElevatorsMessage, expoElevator, RATE_ELEVATOR);
  cmdPacket.elevatorTrim = (int8_t)sendingElevatorTrimMessage;
  cmdPacket.aileronTrim = (int8_t)sendingAileronTrimMessage;
  cmdPacket.flaps = (uint8_t)sendingFlapsMessage;
  cmdPacket.flags = 0;
  if (resetAileronTrim)  cmdPacket.flags |= PROTO_FLAG_RESET_AIL;
  if (resetElevatorTrim) cmdPacket.flags |= PROTO_FLAG_RESET_ELEV;
  if (airbrakeEnabled)   cmdPacket.flags |= PROTO_FLAG_AIRBRAKE;
  if (acsEngageEnabled)  cmdPacket.flags |= PROTO_FLAG_ACS;
  cmdPacket.stabilityAssist = stabilityAssistValue;
  cmdPacket.checksum = proto_checksum((const uint8_t*)&cmdPacket, PROTO_CMD_PACKET_SIZE - 1);
}

#ifdef PROTO_BIDIRECTIONAL
// 📊 Parse binary telemetry packet from flight board (14 bytes)
static void parseTelemetry(const uint8_t* data, int len) {
  if (len != PROTO_TLM_PACKET_SIZE) return;

  const ProtoTlmPacket* pkt = (const ProtoTlmPacket*)data;
  if (pkt->magic != PROTO_TLM_MAGIC) return;

  // Validate software checksum
  uint8_t expected = proto_checksum(data, PROTO_TLM_PACKET_SIZE - 1);
  if (pkt->checksum != expected) return;

  // Decode fixed-point → float
  tlm_altitude      = pkt->altitude_dm    / 10.0f;
  tlm_pressure      = pkt->pressure_dPa   / 10.0f;
  tlm_rssi          = (int)pkt->rssi;
  tlm_gforce        = pkt->gforce_centi   / 100.0f;
  tlm_temperature   = pkt->temperature_dC / 10.0f;
  tlm_verticalSpeed = pkt->vspeed_dms     / 10.0f;
  tlm_valid = true;
  tlm_lastReceived = millis();
}

// 📡 Check for incoming telemetry from flight board
static void checkTelemetry() {
  int packetSize = LoRa.parsePacket();
  if (packetSize == 0)
    return;

  uint8_t rxBuf[PROTO_RX_BUF_SIZE];
  int idx = 0;
  while (LoRa.available() && idx < (int)sizeof(rxBuf)) {
    rxBuf[idx++] = (uint8_t)LoRa.read();
  }
  // Drain any extra bytes
  while (LoRa.available()) LoRa.read();

  parseTelemetry(rxBuf, idx);

  static int tlmCount = 0;
  if (++tlmCount % 5 == 0) {
    Serial.printf("📊 TLM: Alt=%.1fm T=%.1f°C RSSI=%d G=%.2f\n",
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

  if (runEvery(PROTO_CMD_INTERVAL_MS)) {  // 📡 Send every 50ms
    constructMessage();

    int aileronDeviation = abs(sendingAileronMessage - PROTO_JOYSTICK_CENTER);
    int rudderDeviation = abs(sendingRudderMessage - PROTO_JOYSTICK_CENTER);
    int elevatorsDeviation = abs(sendingElevatorsMessage - PROTO_JOYSTICK_CENTER);
    int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

    // 🧮 FNV-1a hash on binary packet for accurate duplicate detection
    uint32_t currentHash = fnv1a_hash((const uint8_t*)&cmdPacket, PROTO_CMD_PACKET_SIZE);

    // 🌿 ECO mode: skip sending duplicate packets when idle (saves bandwidth)
    if (ecoModeEnabled && currentHash == previousHash &&
        samePacketCount >= PROTO_DUPLICATE_LIMIT &&
        totalDeviation < PROTO_IDLE_THRESHOLD) {
      return;
    }

    LoRa_sendPacket((const uint8_t*)&cmdPacket, PROTO_CMD_PACKET_SIZE);  // 📡 Send binary (10 bytes)

    // Reduced serial output - print every 10th packet
    static int printCount = 0;
    if (++printCount >= 10) {
      Serial.printf("📡 TX [%dB]: E=%d A=%d R=%d L=%d F=%d flags=0x%02X\n",
                    PROTO_CMD_PACKET_SIZE, cmdPacket.engine, cmdPacket.ailerons,
                    cmdPacket.rudder, cmdPacket.elevators, cmdPacket.flaps, cmdPacket.flags);
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
// ============================================================================
// 📡 Shared LoRa Protocol Definition
// ============================================================================
// This file defines the communication protocol between Ground Station
// (Ground-Lora) and Flight Computer (Fatima_Arsalan_Board).
// ⚠️ KEEP IDENTICAL IN BOTH PROJECTS — any change here must be mirrored.
// ============================================================================

#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdint.h>

// ============================================================================
// 📻 LoRa Radio Parameters (MUST MATCH on both sides)
// ============================================================================
#define PROTO_LORA_FREQUENCY_MHZ 915.0f
#define PROTO_LORA_FREQUENCY_HZ 915000000L
#define PROTO_LORA_BANDWIDTH_KHZ 125.0f
#define PROTO_LORA_BANDWIDTH_HZ 125000L
#define PROTO_LORA_SF 7
#define PROTO_LORA_CR 5  // Coding rate 4/5
#define PROTO_LORA_SYNC_WORD 0x12
#define PROTO_LORA_PREAMBLE 8
#define PROTO_LORA_TX_POWER 17  // dBm (aligned: both sides use 17)

// ============================================================================
// 📦 Command Packet Format (Ground → Air)
// ============================================================================
// Format: e[engine]a[ailerons]r[rudder]l[elevators]t[trim]i[atrim]f[flaps]z[rz]y[ry]b[brake]#
// Example: "e0a90r90l90t0i0f0z0y0b0#"
//
// Field keys:
#define PROTO_KEY_ENGINE 'e'      // Engine throttle       (0-180)
#define PROTO_KEY_AILERONS 'a'    // Ailerons              (0-180, 90=center)
#define PROTO_KEY_RUDDER 'r'      // Rudder                (0-180, 90=center)
#define PROTO_KEY_ELEVATORS 'l'   // Elevators             (0-180, 90=center)
#define PROTO_KEY_ELEV_TRIM 't'   // Elevator trim         (-1, 0, 1)
#define PROTO_KEY_AIL_TRIM 'i'    // Aileron trim          (-1, 0, 1)
#define PROTO_KEY_FLAPS 'f'       // Flaps position        (0-4)
#define PROTO_KEY_RESET_AIL 'z'   // Reset aileron trim    (0 or 1)
#define PROTO_KEY_RESET_ELEV 'y'  // Reset elevator trim   (0 or 1)
#define PROTO_KEY_AIRBRAKE 'b'    // Airbrake              (0 or 1)
#define PROTO_CMD_END '#'         // End delimiter

// Command packet size constraints
#define PROTO_CMD_MIN_LEN 22   // Minimum valid command length
#define PROTO_CMD_MAX_LEN 35   // Maximum valid command length
#define PROTO_MSG_BUF_SIZE 48  // Buffer size for message construction/parsing

// ============================================================================
// 📊 Telemetry Packet Format (Air → Ground)  [NEW]
// ============================================================================
// Format: T[alt]P[press]R[rssi]G[gforce]K[temp]S[speed]#
// Example: "T150.5P1013.2R-45G1.2K25.3S0.0#"
//
#define PROTO_KEY_TELEMETRY 'T'  // Telemetry packet start / Altitude (m)
#define PROTO_KEY_PRESSURE 'P'   // Pressure (hPa)
#define PROTO_KEY_RSSI 'R'       // RSSI of last received command (dBm)
#define PROTO_KEY_GFORCE 'G'     // G-force magnitude
#define PROTO_KEY_TEMP 'K'       // Temperature (°C) — 'K' to avoid clash with 't'
#define PROTO_KEY_SPEED 'S'      // Vertical speed (m/s)
#define PROTO_TLM_END '#'        // End delimiter

#define PROTO_TLM_BUF_SIZE 80  // Buffer size for telemetry message
#define PROTO_TLM_MIN_LEN 12   // Minimum valid telemetry length
#define PROTO_TLM_MAX_LEN 75   // Maximum valid telemetry length

// Telemetry send interval (ms) — flight board sends telemetry at this rate
#define PROTO_TLM_INTERVAL_MS 500

// ============================================================================
// 🎮 Control Value Ranges
// ============================================================================
#define PROTO_SERVO_MIN 0
#define PROTO_SERVO_MAX 180
#define PROTO_SERVO_CENTER 90
#define PROTO_ENGINE_MIN 0
#define PROTO_ENGINE_MAX 180
#define PROTO_FLAPS_MIN 0
#define PROTO_FLAPS_MAX 4
#define PROTO_TRIM_MIN -1
#define PROTO_TRIM_MAX 1

// Ground station joystick raw ranges
#define PROTO_JOYSTICK_MIN 0
#define PROTO_JOYSTICK_MAX 255
#define PROTO_JOYSTICK_CENTER 127
#define PROTO_ENGINE_RAW_MIN 0
#define PROTO_ENGINE_RAW_MAX 4095

// ============================================================================
// ⏱️ Timing Constants
// ============================================================================
#define PROTO_CMD_INTERVAL_MS 60          // Ground TX interval (ms)
#define PROTO_CONNECTION_TIMEOUT_MS 2000  // Flight board connection timeout (ms)
#define PROTO_IDLE_THRESHOLD 10           // Joystick idle deviation threshold
#define PROTO_DUPLICATE_LIMIT 10          // Max duplicate packets before suppression

// ============================================================================
// 🔁 Communication Mode
// ============================================================================
// PROTO_BIDIRECTIONAL — enables two-way communication (commands + telemetry).
//   Defined  → Bidirectional: Ground → Air commands  AND  Air → Ground telemetry
//   Undefined → Unidirectional: Ground → Air commands ONLY (no telemetry TX/RX)
//
// Override from platformio.ini with:  build_flags = -DPROTO_BIDIRECTIONAL
// Or define/comment out the line below:
#define PROTO_BIDIRECTIONAL  // Comment this line out for unidirectional mode

#endif  // PROTOCOL_H

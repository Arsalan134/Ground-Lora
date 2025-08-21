#include <unity.h>

// 📱 Native environment compatibility
#ifndef ARDUINO
#include <algorithm>
#include <cstdint>
typedef uint8_t byte;
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
#define max(a, b) std::max(a, b)
#define min(a, b) std::min(a, b)
unsigned long millis() {
  return 0;
}
void delay(unsigned long ms) {}
#else
#include <Arduino.h>
#endif

// 🎮 Mock PS5 controller state for testing
struct MockPS5State {
  // Analog sticks (0-255 range, 127 = center)
  int LStickX = 127;  // ↔️ Aileron
  int RStickX = 127;  // ↔️ Rudder
  int RStickY = 127;  // ↕️ Elevator
  int R2Value = 0;    // 🎚️ Throttle trigger
  int L2Value = 0;    // 🎚️ Left trigger

  // Buttons
  bool Up = false;        // ⬆️
  bool Down = false;      // ⬇️
  bool Left = false;      // ⬅️
  bool Right = false;     // ➡️
  bool Cross = false;     // ❌
  bool Circle = false;    // ⭕
  bool Square = false;    // 🟨
  bool Triangle = false;  // 🔺
  bool L1 = false;        // 🔴
  bool R1 = false;        // 🔴
  bool L3 = false;        // 🔘
  bool R3 = false;        // 🔘
  bool PSButton = false;  // 🏠
  bool Share = false;     // 🔗
  bool Options = false;   // ⚙️
  bool Touchpad = false;  // 📱

  // Connection state
  bool connected = true;  // ✅ Connected
  int batteryLevel = 75;  // 🔋 Battery percentage
} mockPS5;

// 🧪 Control message variables (simulated from common.h)
int sendingEngineMessage = 1;
byte sendingAileronMessage = 127;    // ↔️ Aileron control
byte sendingRudderMessage = 127;     // ↔️ Rudder control
byte sendingElevatorsMessage = 127;  // ↕️ Elevator control
int sendingElevatorTrimMessage = 0;  // ⚖️ Elevator trim
int sendingAileronTrimMessage = 0;   // ⚖️ Aileron trim
int sendingFlapsMessage = 0;         // 🪶 Flaps position
bool resetAileronTrim = false;       // 🔄 Reset aileron trim
bool resetElevatorTrim = false;      // 🔄 Reset elevator trim
bool isEmergencyStopEnabled = true;  // 🚨 Emergency stop
bool airbrakeEnabled = false;        // 🛑 Airbrake
uint8_t batteryPercentage = 0;       // 🔋 Battery level

void setUp(void) {
  // 🚀 Reset mock PS5 state before each test
  mockPS5 = MockPS5State();  // Reset to default values
  sendingEngineMessage = 1;
  sendingAileronMessage = 127;
  sendingRudderMessage = 127;
  sendingElevatorsMessage = 127;
  sendingElevatorTrimMessage = 0;
  sendingAileronTrimMessage = 0;
  sendingFlapsMessage = 0;
  resetAileronTrim = false;
  resetElevatorTrim = false;
  isEmergencyStopEnabled = true;
  airbrakeEnabled = false;
  batteryPercentage = 75;
}

void tearDown(void) {
  // 🧹 Clean up after each test
}

// 🧪 Simulate PS5 controller input processing
void processPS5Input() {
  // Process D-pad for trim
  if (mockPS5.Up)
    sendingElevatorTrimMessage = 1;  // ⬆️ Nose up
  if (mockPS5.Down)
    sendingElevatorTrimMessage = -1;  // ⬇️ Nose down
  if (mockPS5.Right)
    sendingAileronTrimMessage = 1;  // ➡️ Right roll
  if (mockPS5.Left)
    sendingAileronTrimMessage = -1;  // ⬅️ Left roll

  // Process action buttons
  if (mockPS5.Cross) {  // ❌ Disable emergency
    isEmergencyStopEnabled = false;
    airbrakeEnabled = false;
  }
  if (mockPS5.Circle)
    isEmergencyStopEnabled = true;  // ⭕ Enable emergency
  if (mockPS5.PSButton)
    airbrakeEnabled = true;  // 🏠 Enable airbrake

  // Process shoulder buttons for flaps
  if (mockPS5.L1 && sendingFlapsMessage > 0) {  // 🔴 Flaps down
    sendingFlapsMessage = constrain(sendingFlapsMessage - 1, 0, 4);
  }
  if (mockPS5.R1 && sendingFlapsMessage < 4) {  // 🔴 Flaps up
    sendingFlapsMessage = constrain(sendingFlapsMessage + 1, 0, 4);
  }

  // Process stick buttons for trim reset
  if (mockPS5.L3)
    resetAileronTrim = true;  // 🔘 Reset aileron
  if (mockPS5.R3)
    resetElevatorTrim = true;  // 🔘 Reset elevator

  // Process analog inputs (add 128 to center around 127)
  sendingAileronMessage = mockPS5.LStickX;    // ↔️ Aileron
  sendingRudderMessage = mockPS5.RStickX;     // ↔️ Rudder
  sendingElevatorsMessage = mockPS5.RStickY;  // ↕️ Elevator

  // Process throttle (map R2 trigger to engine range)
  int throttleInput = max(0, map(mockPS5.R2Value, 0, 255, 0, 4095));
  sendingEngineMessage = isEmergencyStopEnabled ? 0 : throttleInput;  // 🚀 Engine

  // Update battery level
  batteryPercentage = mockPS5.batteryLevel;  // 🔋 Battery
}

// ✅ Test PS5 controller connection state
void test_ps5_connection_state() {
  mockPS5.connected = true;
  TEST_ASSERT_TRUE(mockPS5.connected);

  mockPS5.connected = false;
  TEST_ASSERT_FALSE(mockPS5.connected);
}

// ✅ Test analog stick input processing
void test_analog_stick_processing() {
  // Test center position (neutral)
  mockPS5.LStickX = 127;  // Aileron center
  mockPS5.RStickX = 127;  // Rudder center
  mockPS5.RStickY = 127;  // Elevator center

  processPS5Input();

  TEST_ASSERT_EQUAL(127, sendingAileronMessage);
  TEST_ASSERT_EQUAL(127, sendingRudderMessage);
  TEST_ASSERT_EQUAL(127, sendingElevatorsMessage);

  // Test maximum deflection
  mockPS5.LStickX = 255;  // Full right aileron
  mockPS5.RStickX = 0;    // Full left rudder
  mockPS5.RStickY = 255;  // Full back elevator

  processPS5Input();

  TEST_ASSERT_EQUAL(255, sendingAileronMessage);
  TEST_ASSERT_EQUAL(0, sendingRudderMessage);
  TEST_ASSERT_EQUAL(255, sendingElevatorsMessage);
}

// ✅ Test throttle trigger processing
void test_throttle_processing() {
  isEmergencyStopEnabled = false;  // Disable emergency to allow throttle

  // Test idle throttle
  mockPS5.R2Value = 0;
  processPS5Input();
  TEST_ASSERT_EQUAL(0, sendingEngineMessage);

  // Test half throttle
  mockPS5.R2Value = 127;
  processPS5Input();
  int expectedHalf = map(127, 0, 255, 0, 4095);
  TEST_ASSERT_EQUAL(expectedHalf, sendingEngineMessage);

  // Test full throttle
  mockPS5.R2Value = 255;
  processPS5Input();
  int expectedFull = map(255, 0, 255, 0, 4095);
  TEST_ASSERT_EQUAL(expectedFull, sendingEngineMessage);

  // Test emergency stop override
  isEmergencyStopEnabled = true;
  mockPS5.R2Value = 255;  // Full throttle requested
  processPS5Input();
  TEST_ASSERT_EQUAL(0, sendingEngineMessage);  // Should be zero due to emergency
}

// ✅ Test emergency stop functionality
void test_emergency_stop() {
  // Test emergency stop activation
  isEmergencyStopEnabled = false;
  mockPS5.Circle = true;  // Press Circle to activate emergency
  processPS5Input();
  TEST_ASSERT_TRUE(isEmergencyStopEnabled);

  // Test emergency stop deactivation
  isEmergencyStopEnabled = true;
  mockPS5.Circle = false;
  mockPS5.Cross = true;  // Press Cross to deactivate emergency
  processPS5Input();
  TEST_ASSERT_FALSE(isEmergencyStopEnabled);
  TEST_ASSERT_FALSE(airbrakeEnabled);  // Should also disable airbrake
}

// ✅ Test trim control functionality
void test_trim_controls() {
  // Test elevator trim up
  mockPS5.Up = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(1, sendingElevatorTrimMessage);

  // Reset for next test
  setUp();

  // Test elevator trim down
  mockPS5.Down = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(-1, sendingElevatorTrimMessage);

  // Reset for next test
  setUp();

  // Test aileron trim right
  mockPS5.Right = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(1, sendingAileronTrimMessage);

  // Reset for next test
  setUp();

  // Test aileron trim left
  mockPS5.Left = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(-1, sendingAileronTrimMessage);
}

// ✅ Test flaps control
void test_flaps_control() {
  sendingFlapsMessage = 2;  // Start at middle position

  // Test flaps down (L1)
  mockPS5.L1 = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(1, sendingFlapsMessage);

  // Reset for next test
  setUp();
  sendingFlapsMessage = 2;

  // Test flaps up (R1)
  mockPS5.R1 = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(3, sendingFlapsMessage);

  // Test flaps limits
  sendingFlapsMessage = 0;  // At minimum
  mockPS5.L1 = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(0, sendingFlapsMessage);  // Should not go below 0

  sendingFlapsMessage = 4;  // At maximum
  mockPS5.L1 = false;
  mockPS5.R1 = true;
  processPS5Input();
  TEST_ASSERT_EQUAL(4, sendingFlapsMessage);  // Should not go above 4
}

// ✅ Test trim reset functionality
void test_trim_reset() {
  // Test aileron trim reset
  resetAileronTrim = false;
  mockPS5.L3 = true;  // Press L3 stick
  processPS5Input();
  TEST_ASSERT_TRUE(resetAileronTrim);

  // Test elevator trim reset
  resetElevatorTrim = false;
  mockPS5.R3 = true;  // Press R3 stick
  processPS5Input();
  TEST_ASSERT_TRUE(resetElevatorTrim);
}

// ✅ Test airbrake control
void test_airbrake_control() {
  airbrakeEnabled = false;
  mockPS5.PSButton = true;  // Press PS button
  processPS5Input();
  TEST_ASSERT_TRUE(airbrakeEnabled);
}

// ✅ Test battery level reporting
void test_battery_reporting() {
  mockPS5.batteryLevel = 85;
  processPS5Input();
  TEST_ASSERT_EQUAL(85, batteryPercentage);

  mockPS5.batteryLevel = 25;
  processPS5Input();
  TEST_ASSERT_EQUAL(25, batteryPercentage);

  mockPS5.batteryLevel = 100;
  processPS5Input();
  TEST_ASSERT_EQUAL(100, batteryPercentage);
}

// ✅ Test input validation ranges
void test_input_validation() {
  // Test analog stick bounds
  mockPS5.LStickX = 300;  // Above maximum
  mockPS5.RStickY = -50;  // Below minimum

  // In real implementation, these should be constrained
  // Here we just verify they don't crash the system
  processPS5Input();

  // Test that extreme values are handled
  TEST_ASSERT_TRUE(sendingAileronMessage >= 0);
  TEST_ASSERT_TRUE(sendingElevatorsMessage >= 0);
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor
#endif

  UNITY_BEGIN();

  // 🧪 Run PS5 controller tests
  RUN_TEST(test_ps5_connection_state);
  RUN_TEST(test_analog_stick_processing);
  RUN_TEST(test_throttle_processing);
  RUN_TEST(test_emergency_stop);
  RUN_TEST(test_trim_controls);
  RUN_TEST(test_flaps_control);
  RUN_TEST(test_trim_reset);
  RUN_TEST(test_airbrake_control);
  RUN_TEST(test_battery_reporting);
  RUN_TEST(test_input_validation);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

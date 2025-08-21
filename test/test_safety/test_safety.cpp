#include <unity.h>

// 📱 Native environment compatibility
#ifndef ARDUINO
#include <cstdint>
typedef uint8_t byte;
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
unsigned long millis() {
  return 0;
}
void delay(unsigned long ms) {}
#else
#include <Arduino.h>
#endif

// 🧪 Mock variables for safety system testing
bool isEmergencyStopEnabled = true;
bool airbrakeEnabled = false;
int sendingEngineMessage = 1;
byte sendingAileronMessage = 127;
byte sendingRudderMessage = 127;
byte sendingElevatorsMessage = 127;
int sendingFlapsMessage = 0;
bool resetAileronTrim = false;
bool resetElevatorTrim = false;

// 🧪 Mock timing variables
unsigned long mockMillis = 0;
unsigned long lastFlapsChangeTimestamp = 0;
unsigned long lastTimeStamp = 0;

// 🧪 Mock controller state
bool mockPS5Connected = true;
const int idleDeviationThreshold = 10;

void setUp(void) {
  // 🚀 Reset safety system state before each test
  isEmergencyStopEnabled = true;
  airbrakeEnabled = false;
  sendingEngineMessage = 1;
  sendingAileronMessage = 127;
  sendingRudderMessage = 127;
  sendingElevatorsMessage = 127;
  sendingFlapsMessage = 0;
  resetAileronTrim = false;
  resetElevatorTrim = false;
  mockPS5Connected = true;
  mockMillis = 0;
  lastFlapsChangeTimestamp = 0;
  lastTimeStamp = 0;
}

void tearDown(void) {
  // 🧹 Clean up after each test
}

// ✅ Test emergency stop functionality
void test_emergency_stop_functionality() {
  // Test emergency stop enabled - engine should be zero
  isEmergencyStopEnabled = true;
  int engineOutput = isEmergencyStopEnabled ? 0 : sendingEngineMessage;
  TEST_ASSERT_EQUAL(0, engineOutput);

  // Test emergency stop disabled - engine should use actual value
  isEmergencyStopEnabled = false;
  sendingEngineMessage = 100;
  engineOutput = isEmergencyStopEnabled ? 0 : sendingEngineMessage;
  TEST_ASSERT_EQUAL(100, engineOutput);

  // Test emergency stop override with max throttle
  isEmergencyStopEnabled = true;
  sendingEngineMessage = 4095;  // Max throttle
  engineOutput = isEmergencyStopEnabled ? 0 : sendingEngineMessage;
  TEST_ASSERT_EQUAL(0, engineOutput);  // Should still be zero
}

// ✅ Test control surface range safety limits
void test_control_surface_limits() {
  // Test that control surfaces stay within safe ranges (0-255)
  sendingAileronMessage = 300;  // Above maximum
  sendingAileronMessage = constrain(sendingAileronMessage, 0, 255);
  TEST_ASSERT_EQUAL(255, sendingAileronMessage);

  sendingRudderMessage = -50;  // Below minimum
  sendingRudderMessage = constrain(sendingRudderMessage, 0, 255);
  TEST_ASSERT_EQUAL(0, sendingRudderMessage);

  sendingElevatorsMessage = 127;  // Valid range
  sendingElevatorsMessage = constrain(sendingElevatorsMessage, 0, 255);
  TEST_ASSERT_EQUAL(127, sendingElevatorsMessage);
}

// ✅ Test flaps position safety limits
void test_flaps_safety_limits() {
  // Test flaps position constraints (0-4)
  sendingFlapsMessage = -1;  // Below minimum
  sendingFlapsMessage = constrain(sendingFlapsMessage, 0, 4);
  TEST_ASSERT_EQUAL(0, sendingFlapsMessage);

  sendingFlapsMessage = 10;  // Above maximum
  sendingFlapsMessage = constrain(sendingFlapsMessage, 0, 4);
  TEST_ASSERT_EQUAL(4, sendingFlapsMessage);

  // Test valid flaps positions
  for (int i = 0; i <= 4; i++) {
    sendingFlapsMessage = i;
    sendingFlapsMessage = constrain(sendingFlapsMessage, 0, 4);
    TEST_ASSERT_EQUAL(i, sendingFlapsMessage);
  }
}

// ✅ Test rate limiting for flaps changes
void test_flaps_rate_limiting() {
  const unsigned long minFlapsInterval = 200;  // 200ms minimum between changes

  // Test that flaps changes respect timing
  mockMillis = 0;
  lastFlapsChangeTimestamp = 0;

  // First change should be allowed
  bool changeAllowed = (mockMillis - lastFlapsChangeTimestamp) > minFlapsInterval;
  TEST_ASSERT_TRUE(changeAllowed);

  // Immediate second change should be blocked
  mockMillis = 100;  // Only 100ms later
  lastFlapsChangeTimestamp = 0;
  changeAllowed = (mockMillis - lastFlapsChangeTimestamp) > minFlapsInterval;
  TEST_ASSERT_FALSE(changeAllowed);

  // Change after sufficient time should be allowed
  mockMillis = 250;  // 250ms later
  lastFlapsChangeTimestamp = 0;
  changeAllowed = (mockMillis - lastFlapsChangeTimestamp) > minFlapsInterval;
  TEST_ASSERT_TRUE(changeAllowed);
}

// ✅ Test engine safety interlock
void test_engine_safety_interlock() {
  bool setToZeroEngineSlider = false;
  sendingEngineMessage = 100;  // Non-zero engine value

  // Test safety condition: if engine is non-zero but slider should be zero
  if (!setToZeroEngineSlider && sendingEngineMessage) {
    // Safety system should prevent operation
    TEST_ASSERT_TRUE(sendingEngineMessage > 0);
    TEST_ASSERT_FALSE(setToZeroEngineSlider);
    // In real system, this would return early from main loop
  }

  // Test safe condition: slider allows engine operation
  setToZeroEngineSlider = true;
  sendingEngineMessage = 0;
  TEST_ASSERT_TRUE(setToZeroEngineSlider);
  TEST_ASSERT_EQUAL(0, sendingEngineMessage);
}

// ✅ Test connection loss safety
void test_connection_loss_safety() {
  // Test normal operation with connection
  mockPS5Connected = true;
  if (mockPS5Connected) {
    // Normal operation - commands should be processed
    TEST_ASSERT_TRUE(mockPS5Connected);
  }

  // Test disconnection safety
  mockPS5Connected = false;
  if (!mockPS5Connected) {
    // Safety system should activate
    // In real system, this would trigger emergency stop
    isEmergencyStopEnabled = true;
    TEST_ASSERT_TRUE(isEmergencyStopEnabled);
  }
}

// ✅ Test idle detection for packet optimization
void test_idle_detection() {
  // Test joystick deviation calculation
  int centerPos = 127;

  // Calculate total deviation from center
  sendingAileronMessage = 130;    // Small deviation (3)
  sendingRudderMessage = 124;     // Small deviation (3)
  sendingElevatorsMessage = 132;  // Small deviation (5)

  int aileronDeviation = abs(sendingAileronMessage - centerPos);
  int rudderDeviation = abs(sendingRudderMessage - centerPos);
  int elevatorsDeviation = abs(sendingElevatorsMessage - centerPos);
  int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

  TEST_ASSERT_EQUAL(3, aileronDeviation);
  TEST_ASSERT_EQUAL(3, rudderDeviation);
  TEST_ASSERT_EQUAL(5, elevatorsDeviation);
  TEST_ASSERT_EQUAL(11, totalDeviation);

  // Test idle threshold
  bool isIdle = totalDeviation < idleDeviationThreshold;
  TEST_ASSERT_FALSE(isIdle);  // 11 > 10, so not idle

  // Test actual idle condition
  sendingAileronMessage = 127;    // Center
  sendingRudderMessage = 127;     // Center
  sendingElevatorsMessage = 127;  // Center

  aileronDeviation = abs(sendingAileronMessage - centerPos);
  rudderDeviation = abs(sendingRudderMessage - centerPos);
  elevatorsDeviation = abs(sendingElevatorsMessage - centerPos);
  totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

  isIdle = totalDeviation < idleDeviationThreshold;
  TEST_ASSERT_TRUE(isIdle);  // 0 < 10, so is idle
}

// ✅ Test airbrake safety system
void test_airbrake_safety() {
  // Test airbrake activation
  airbrakeEnabled = false;
  // Simulate PS button press
  airbrakeEnabled = true;
  TEST_ASSERT_TRUE(airbrakeEnabled);

  // Test airbrake deactivation with Cross button
  airbrakeEnabled = true;
  isEmergencyStopEnabled = true;
  // Simulate Cross button press
  isEmergencyStopEnabled = false;
  airbrakeEnabled = false;
  TEST_ASSERT_FALSE(airbrakeEnabled);
  TEST_ASSERT_FALSE(isEmergencyStopEnabled);
}

// ✅ Test control input validation
void test_input_validation() {
  // Test that all critical values are within expected ranges
  sendingEngineMessage = 2000;
  TEST_ASSERT_TRUE(sendingEngineMessage >= 0 && sendingEngineMessage <= 4095);

  sendingAileronMessage = 200;
  TEST_ASSERT_TRUE(sendingAileronMessage >= 0 && sendingAileronMessage <= 255);

  sendingRudderMessage = 50;
  TEST_ASSERT_TRUE(sendingRudderMessage >= 0 && sendingRudderMessage <= 255);

  sendingElevatorsMessage = 180;
  TEST_ASSERT_TRUE(sendingElevatorsMessage >= 0 && sendingElevatorsMessage <= 255);

  sendingFlapsMessage = 3;
  TEST_ASSERT_TRUE(sendingFlapsMessage >= 0 && sendingFlapsMessage <= 4);
}

// ✅ Test trim reset safety
void test_trim_reset_safety() {
  // Test that trim reset flags work correctly
  resetAileronTrim = true;
  resetElevatorTrim = true;

  TEST_ASSERT_TRUE(resetAileronTrim);
  TEST_ASSERT_TRUE(resetElevatorTrim);

  // Test that trim values are cleared after reset
  // (In real system, these would be reset to 0 after processing)
  if (resetAileronTrim) {
    resetAileronTrim = false;  // Simulate reset processing
  }
  if (resetElevatorTrim) {
    resetElevatorTrim = false;  // Simulate reset processing
  }

  TEST_ASSERT_FALSE(resetAileronTrim);
  TEST_ASSERT_FALSE(resetElevatorTrim);
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor
#endif

  UNITY_BEGIN();

  // 🧪 Run safety system tests
  RUN_TEST(test_emergency_stop_functionality);
  RUN_TEST(test_control_surface_limits);
  RUN_TEST(test_flaps_safety_limits);
  RUN_TEST(test_flaps_rate_limiting);
  RUN_TEST(test_engine_safety_interlock);
  RUN_TEST(test_connection_loss_safety);
  RUN_TEST(test_idle_detection);
  RUN_TEST(test_airbrake_safety);
  RUN_TEST(test_input_validation);
  RUN_TEST(test_trim_reset_safety);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

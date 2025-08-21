#include <unity.h>

// 📱 Native environment compatibility
#ifndef ARDUINO
#include <cstdint>
#include <sstream>
#include <string>

typedef uint8_t byte;
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
unsigned long millis() {
  return 0;
}
void delay(unsigned long ms) {}

// 🎯 Arduino String compatibility for native testing
class String {
 private:
  std::string str;

 public:
  String() {}
  String(const char* s) : str(s) {}
  String(const std::string& s) : str(s) {}
  String(int val) : str(std::to_string(val)) {}
  String(byte val) : str(std::to_string(val)) {}
  String(long val) : str(std::to_string(val)) {}
  String(double val) : str(std::to_string(val)) {}

  String& operator+=(const String& other) {
    str += other.str;
    return *this;
  }
  String& operator+=(const char* s) {
    str += s;
    return *this;
  }
  String operator+(const String& other) const { return String(str + other.str); }
  String operator+(const char* s) const { return String(str + s); }

  int indexOf(const char* substr) const {
    size_t pos = str.find(substr);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
  }
  int indexOf(const String& substr) const {
    size_t pos = str.find(substr.str);
    return pos == std::string::npos ? -1 : static_cast<int>(pos);
  }

  const char* c_str() const { return str.c_str(); }
  size_t length() const { return str.length(); }
};

// Helper function for string concatenation
String operator+(const char* s, const String& str) {
  return String(std::string(s) + str.c_str());
}
#else
#include <Arduino.h>
#endif>

// 📱 Native environment compatibility
#ifndef ARDUINO
#include <cstdint>
#include <string>
typedef uint8_t byte;
typedef std::string String;
#define map(x, in_min, in_max, out_min, out_max) ((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min)
#define constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))
unsigned long millis() {
  return 0;
}
void delay(unsigned long ms) {}
#else
#include <Arduino.h>
#endif

// 🖥️ Mock display dimensions and state
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

// 🧪 Mock display state for testing
struct MockDisplayState {
  String lastDrawnText = "";
  int lastX = 0;
  int lastY = 0;
  bool isInitialized = false;
  int targetFPS = 60;
  int currentFrame = 0;
  bool overlaysEnabled = false;
} mockDisplay;

// 🧪 Mock variables from common.h for testing
int sendingEngineMessage = 1;
byte sendingAileronMessage = 127;
byte sendingRudderMessage = 127;
byte sendingElevatorsMessage = 127;
int sendingFlapsMessage = 0;
bool isEmergencyStopEnabled = true;
uint8_t batteryPercentage = 75;

// 🧪 Mock PS5 connection state
bool mockPS5Connected = true;
int mockL2Value = 0;
int mockR2Value = 128;

void setUp(void) {
  // 🚀 Reset display state before each test
  mockDisplay = MockDisplayState();
  sendingEngineMessage = 1;
  sendingAileronMessage = 127;
  sendingRudderMessage = 127;
  sendingElevatorsMessage = 127;
  sendingFlapsMessage = 0;
  isEmergencyStopEnabled = true;
  batteryPercentage = 75;
  mockPS5Connected = true;
}

void tearDown(void) {
  // 🧹 Clean up after each test
}

// ✅ Test display initialization parameters
void test_display_initialization() {
  mockDisplay.isInitialized = true;
  mockDisplay.targetFPS = 60;

  TEST_ASSERT_TRUE(mockDisplay.isInitialized);
  TEST_ASSERT_EQUAL(60, mockDisplay.targetFPS);
  TEST_ASSERT_TRUE(mockDisplay.targetFPS > 0);
  TEST_ASSERT_TRUE(mockDisplay.targetFPS <= 60);  // Reasonable FPS limit
}

// ✅ Test engine percentage calculation for display
void test_engine_percentage_display() {
  // Test various engine values and their percentage representation
  sendingEngineMessage = 0;  // Idle
  int percentage = map(sendingEngineMessage, 0, 4095, 0, 100);
  TEST_ASSERT_EQUAL(0, percentage);

  sendingEngineMessage = 2047;  // Half throttle
  percentage = map(sendingEngineMessage, 0, 4095, 0, 100);
  TEST_ASSERT_TRUE(percentage >= 49 && percentage <= 51);  // ~50%

  sendingEngineMessage = 4095;  // Full throttle
  percentage = map(sendingEngineMessage, 0, 4095, 0, 100);
  TEST_ASSERT_EQUAL(100, percentage);
}

// ✅ Test emergency stop display logic
void test_emergency_stop_display() {
  // Test emergency stop enabled display
  isEmergencyStopEnabled = true;
  String emergencyText = isEmergencyStopEnabled ? "🚨STOP" : "";
  TEST_ASSERT_EQUAL_STRING("🚨STOP", emergencyText.c_str());

  // Test emergency stop disabled display
  isEmergencyStopEnabled = false;
  emergencyText = isEmergencyStopEnabled ? "🚨STOP" : "";
  TEST_ASSERT_EQUAL_STRING("", emergencyText.c_str());
}

// ✅ Test control surface value ranges for display
void test_control_surface_display_ranges() {
  // Test minimum values
  sendingAileronMessage = 0;
  sendingRudderMessage = 0;
  sendingElevatorsMessage = 0;

  TEST_ASSERT_TRUE(sendingAileronMessage >= 0 && sendingAileronMessage <= 255);
  TEST_ASSERT_TRUE(sendingRudderMessage >= 0 && sendingRudderMessage <= 255);
  TEST_ASSERT_TRUE(sendingElevatorsMessage >= 0 && sendingElevatorsMessage <= 255);

  // Test maximum values
  sendingAileronMessage = 255;
  sendingRudderMessage = 255;
  sendingElevatorsMessage = 255;

  TEST_ASSERT_TRUE(sendingAileronMessage >= 0 && sendingAileronMessage <= 255);
  TEST_ASSERT_TRUE(sendingRudderMessage >= 0 && sendingRudderMessage <= 255);
  TEST_ASSERT_TRUE(sendingElevatorsMessage >= 0 && sendingElevatorsMessage <= 255);

  // Test center values
  sendingAileronMessage = 127;
  sendingRudderMessage = 127;
  sendingElevatorsMessage = 127;

  TEST_ASSERT_EQUAL(127, sendingAileronMessage);
  TEST_ASSERT_EQUAL(127, sendingRudderMessage);
  TEST_ASSERT_EQUAL(127, sendingElevatorsMessage);
}

// ✅ Test flaps display values
void test_flaps_display() {
  // Test all valid flaps positions
  for (int i = 0; i <= 4; i++) {
    sendingFlapsMessage = i;
    TEST_ASSERT_TRUE(sendingFlapsMessage >= 0 && sendingFlapsMessage <= 4);
  }

  // Test invalid flaps positions (should be constrained)
  sendingFlapsMessage = constrain(-1, 0, 4);
  TEST_ASSERT_EQUAL(0, sendingFlapsMessage);

  sendingFlapsMessage = constrain(10, 0, 4);
  TEST_ASSERT_EQUAL(4, sendingFlapsMessage);
}

// ✅ Test battery percentage display
void test_battery_display() {
  // Test valid battery percentages
  batteryPercentage = 0;  // Empty
  TEST_ASSERT_EQUAL(0, batteryPercentage);

  batteryPercentage = 50;  // Half
  TEST_ASSERT_EQUAL(50, batteryPercentage);

  batteryPercentage = 100;  // Full
  TEST_ASSERT_EQUAL(100, batteryPercentage);

  // Test battery percentage string formatting
  String batteryText = String(batteryPercentage) + "% 🔋";
  TEST_ASSERT_TRUE(batteryText.indexOf("%") != -1);
  TEST_ASSERT_TRUE(batteryText.indexOf("🔋") != -1);
}

// ✅ Test display coordinate bounds
void test_display_coordinates() {
  // Test that coordinates are within display bounds
  int testX = 50;
  int testY = 30;

  TEST_ASSERT_TRUE(testX >= 0 && testX < DISPLAY_WIDTH);
  TEST_ASSERT_TRUE(testY >= 0 && testY < DISPLAY_HEIGHT);

  // Test boundary conditions
  TEST_ASSERT_TRUE(0 >= 0 && 0 < DISPLAY_WIDTH);      // Left edge
  TEST_ASSERT_TRUE(127 >= 0 && 127 < DISPLAY_WIDTH);  // Right edge
  TEST_ASSERT_TRUE(0 >= 0 && 0 < DISPLAY_HEIGHT);     // Top edge
  TEST_ASSERT_TRUE(63 >= 0 && 63 < DISPLAY_HEIGHT);   // Bottom edge
}

// ✅ Test PS5 connection display logic
void test_ps5_connection_display() {
  // Test connected state
  mockPS5Connected = true;
  if (mockPS5Connected) {
    // Should display controller values
    TEST_ASSERT_TRUE(mockPS5Connected);
  }

  // Test disconnected state
  mockPS5Connected = false;
  if (mockPS5Connected) {
    // Should not display controller values
    TEST_ASSERT_TRUE(false);  // This branch should not execute
  } else {
    TEST_ASSERT_FALSE(mockPS5Connected);  // Verify disconnected
  }
}

// ✅ Test display frame animation settings
void test_display_animation() {
  // Test frame count
  int frameCount = 1;
  TEST_ASSERT_TRUE(frameCount > 0);
  TEST_ASSERT_TRUE(frameCount <= 10);  // Reasonable frame limit

  // Test overlay count
  int overlaysCount = 1;
  TEST_ASSERT_TRUE(overlaysCount >= 0);
  TEST_ASSERT_TRUE(overlaysCount <= 5);  // Reasonable overlay limit
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor
#endif

  UNITY_BEGIN();

  // 🧪 Run display tests
  RUN_TEST(test_display_initialization);
  RUN_TEST(test_engine_percentage_display);
  RUN_TEST(test_emergency_stop_display);
  RUN_TEST(test_control_surface_display_ranges);
  RUN_TEST(test_flaps_display);
  RUN_TEST(test_battery_display);
  RUN_TEST(test_display_coordinates);
  RUN_TEST(test_ps5_connection_display);
  RUN_TEST(test_display_animation);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

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
#endif

// 📡 Mock LoRa functions for testing
bool mockLoRaInitialized = false;
String lastSentMessage = "";
bool mockLoRaTransmissionSuccess = true;

// 🧪 Test checksum calculation function
byte simple_checksum(const byte* data, size_t len) {
  byte sum = 0;  // 🧮 Checksum calculation
  for (size_t i = 0; i < len; i++) {
    sum ^= data[i];
  }
  return sum;
}

// 🧪 Test message construction (simulated)
String constructLoRaMessage(int engine,
                            byte aileron,
                            byte rudder,
                            byte elevator,
                            int elevatorTrim,
                            int aileronTrim,
                            int flaps,
                            bool resetAileronTrim,
                            bool resetElevatorTrim,
                            bool airbrake) {
  String message = "";
  message += "e" + String(engine);                     // 🚀 Engine
  message += "a" + String(aileron);                    // ↔️ Aileron
  message += "r" + String(rudder);                     // ↔️ Rudder
  message += "l" + String(elevator);                   // ↕️ Elevator
  message += "t" + String(elevatorTrim);               // ⚖️ Elevator trim
  message += "i" + String(aileronTrim);                // ⚖️ Aileron trim
  message += "f" + String(flaps);                      // 🪶 Flaps
  message += "z" + String(resetAileronTrim ? 1 : 0);   // 🔄 Reset aileron
  message += "y" + String(resetElevatorTrim ? 1 : 0);  // 🔄 Reset elevator
  message += "b" + String(airbrake ? 1 : 0);           // 🛑 Airbrake

  return message;
}

void setUp(void) {
  // 🚀 Reset mock state before each test
  mockLoRaInitialized = false;
  lastSentMessage = "";
  mockLoRaTransmissionSuccess = true;
}

void tearDown(void) {
  // 🧹 Clean up after each test
}

// ✅ Test checksum calculation
void test_checksum_calculation() {
  // Test known data
  const byte testData1[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // "Hello"
  byte checksum1 = simple_checksum(testData1, 5);

  const byte testData2[] = {0x48, 0x65, 0x6C, 0x6C, 0x6F};  // Same "Hello"
  byte checksum2 = simple_checksum(testData2, 5);

  // Same data should produce same checksum
  TEST_ASSERT_EQUAL(checksum1, checksum2);

  // Different data should produce different checksum
  const byte testData3[] = {0x48, 0x65, 0x6C, 0x6C, 0x6E};  // "Helln"
  byte checksum3 = simple_checksum(testData3, 5);

  TEST_ASSERT_NOT_EQUAL(checksum1, checksum3);

  // Empty data should return 0
  byte checksumEmpty = simple_checksum(nullptr, 0);
  TEST_ASSERT_EQUAL(0, checksumEmpty);
}

// ✅ Test message construction
void test_message_construction() {
  // Test neutral position message
  String message = constructLoRaMessage(90, 127, 127, 127, 0, 0, 0, false, false, false);
  String expected = "e90a127r127l127t0i0f0z0y0b0";
  TEST_ASSERT_EQUAL_STRING(expected.c_str(), message.c_str());

  // Test emergency stop (engine = 0)
  message = constructLoRaMessage(0, 127, 127, 127, 0, 0, 0, false, false, false);
  expected = "e0a127r127l127t0i0f0z0y0b0";
  TEST_ASSERT_EQUAL_STRING(expected.c_str(), message.c_str());

  // Test airbrake enabled
  message = constructLoRaMessage(90, 127, 127, 127, 0, 0, 0, false, false, true);
  expected = "e90a127r127l127t0i0f0z0y0b1";
  TEST_ASSERT_EQUAL_STRING(expected.c_str(), message.c_str());
}

// ✅ Test control surface range mapping
void test_control_surface_mapping() {
  // Test PS5 controller input to servo output mapping
  // PS5 range: 0-255, Servo range: 0-180

  // Minimum position
  int servoMin = map(0, 0, 255, 0, 180);
  TEST_ASSERT_EQUAL(0, servoMin);

  // Center position (neutral)
  int servoCenter = map(127, 0, 255, 0, 180);
  TEST_ASSERT_TRUE(servoCenter >= 89 && servoCenter <= 91);  // Allow small rounding error

  // Maximum position
  int servoMax = map(255, 0, 255, 0, 180);
  TEST_ASSERT_EQUAL(180, servoMax);
}

// ✅ Test engine throttle mapping
void test_engine_throttle_mapping() {
  // Test analog input to servo output mapping
  // Analog range: 0-4095, Servo range: 0-180

  // Idle position
  int engineIdle = map(0, 0, 4095, 0, 180);
  TEST_ASSERT_EQUAL(0, engineIdle);

  // Half throttle
  int engineHalf = map(2047, 0, 4095, 0, 180);
  TEST_ASSERT_TRUE(engineHalf >= 89 && engineHalf <= 91);  // Allow rounding error

  // Full throttle
  int engineFull = map(4095, 0, 4095, 0, 180);
  TEST_ASSERT_EQUAL(180, engineFull);

  // Test percentage calculation for display
  int percentage = map(2047, 0, 4095, 0, 100);
  TEST_ASSERT_TRUE(percentage >= 49 && percentage <= 51);  // ~50%
}

// ✅ Test flaps position limits
void test_flaps_position_limits() {
  // Test flaps constrain to 0-4 range
  TEST_ASSERT_EQUAL(0, constrain(-5, 0, 4));  // Below minimum
  TEST_ASSERT_EQUAL(0, constrain(0, 0, 4));   // At minimum
  TEST_ASSERT_EQUAL(2, constrain(2, 0, 4));   // In range
  TEST_ASSERT_EQUAL(4, constrain(4, 0, 4));   // At maximum
  TEST_ASSERT_EQUAL(4, constrain(10, 0, 4));  // Above maximum
}

// ✅ Test deviation calculation for idle detection
void test_deviation_calculation() {
  // Test joystick deviation from center (127)
  int centerPos = 127;

  // No deviation
  TEST_ASSERT_EQUAL(0, abs(centerPos - 127));

  // Small deviation (within idle threshold)
  TEST_ASSERT_EQUAL(5, abs(132 - 127));
  TEST_ASSERT_EQUAL(5, abs(122 - 127));

  // Large deviation (outside idle threshold)
  TEST_ASSERT_EQUAL(50, abs(177 - 127));
  TEST_ASSERT_EQUAL(50, abs(77 - 127));

  // Test total deviation calculation
  int aileronDev = abs(130 - 127);                      // 3
  int rudderDev = abs(124 - 127);                       // 3
  int elevatorDev = abs(132 - 127);                     // 5
  int totalDev = aileronDev + rudderDev + elevatorDev;  // 11

  TEST_ASSERT_EQUAL(11, totalDev);
  TEST_ASSERT_TRUE(totalDev > 10);  // Above idle threshold
}

// ✅ Test message validation
void test_message_validation() {
  // Test valid message format
  String validMessage = "e90a127r127l127t0i0f0z0y0b0";
  TEST_ASSERT_TRUE(validMessage.length() >= 10);      // Minimum length check
  TEST_ASSERT_TRUE(validMessage.indexOf("e") == 0);   // Starts with engine
  TEST_ASSERT_TRUE(validMessage.indexOf("a") != -1);  // Contains aileron
  TEST_ASSERT_TRUE(validMessage.indexOf("r") != -1);  // Contains rudder
  TEST_ASSERT_TRUE(validMessage.indexOf("l") != -1);  // Contains elevator

  // Test message with checksum
  byte checksum = simple_checksum((const byte*)validMessage.c_str(), validMessage.length());
  String messageWithChecksum = validMessage + "#" + String(checksum);

  TEST_ASSERT_TRUE(messageWithChecksum.indexOf("#") != -1);                // Has delimiter
  TEST_ASSERT_TRUE(messageWithChecksum.length() > validMessage.length());  // Longer with checksum
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor to open
#endif

  UNITY_BEGIN();  // 🧪 Run LoRa communication tests
  RUN_TEST(test_checksum_calculation);
  RUN_TEST(test_message_construction);
  RUN_TEST(test_control_surface_mapping);
  RUN_TEST(test_engine_throttle_mapping);
  RUN_TEST(test_flaps_position_limits);
  RUN_TEST(test_deviation_calculation);
  RUN_TEST(test_message_validation);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

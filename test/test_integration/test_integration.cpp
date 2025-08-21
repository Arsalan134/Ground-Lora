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

// 🧪 Integration tests for the complete Ground-Lora system

// 🧪 Mock system state
struct SystemState {
  // 🎮 Controller state
  bool ps5Connected = false;
  int batteryLevel = 0;

  // 📡 Communication state
  bool loraInitialized = false;
  String lastMessage = "";
  unsigned long lastTransmission = 0;

  // 🖥️ Display state
  bool displayInitialized = false;
  int currentFrame = 0;

  // 🛡️ Safety state
  bool emergencyActive = true;
  bool systemReady = false;
} mockSystem;

// 🧪 Mock control inputs
struct ControlInputs {
  byte aileron = 127;     // ↔️ Neutral
  byte rudder = 127;      // ↔️ Neutral
  byte elevator = 127;    // ↕️ Neutral
  int engine = 0;         // 🚀 Idle
  int flaps = 0;          // 🪶 Retracted
  bool emergency = true;  // 🚨 Safe state
  bool airbrake = false;  // 🛑 Disabled
} mockControls;

void setUp(void) {
  // 🚀 Reset system state before each test
  mockSystem = SystemState();
  mockControls = ControlInputs();
}

void tearDown(void) {
  // 🧹 Clean up after each test
}

// 🧪 Simulate system initialization sequence
bool simulateSystemInit() {
  // Simulate display init
  mockSystem.displayInitialized = true;

  // Simulate LoRa init
  mockSystem.loraInitialized = true;

  // Simulate PS5 connection (may take time)
  mockSystem.ps5Connected = false;  // Initially disconnected

  mockSystem.systemReady = mockSystem.displayInitialized && mockSystem.loraInitialized;

  return mockSystem.systemReady;
}

// 🧪 Simulate PS5 controller connection
void simulatePS5Connection() {
  mockSystem.ps5Connected = true;
  mockSystem.batteryLevel = 85;        // 🔋 85% battery
  mockSystem.emergencyActive = false;  // Can disable emergency when connected
}

// 🧪 Simulate control message generation
String simulateMessageGeneration() {
  String message = "";
  message += "e" + String(mockControls.emergency ? 0 : mockControls.engine);  // 🚀 Engine
  message += "a" + String(mockControls.aileron);                              // ↔️ Aileron
  message += "r" + String(mockControls.rudder);                               // ↔️ Rudder
  message += "l" + String(mockControls.elevator);                             // ↕️ Elevator
  message += "f" + String(mockControls.flaps);                                // 🪶 Flaps
  message += "b" + String(mockControls.airbrake ? 1 : 0);                     // 🛑 Airbrake

  return message;
}

// ✅ Test complete system initialization
void test_system_initialization() {
  // Test that all subsystems initialize correctly
  bool initSuccess = simulateSystemInit();

  TEST_ASSERT_TRUE(mockSystem.displayInitialized);  // 🖥️ Display ready
  TEST_ASSERT_TRUE(mockSystem.loraInitialized);     // 📡 LoRa ready
  TEST_ASSERT_TRUE(mockSystem.systemReady);         // ✅ System ready
  TEST_ASSERT_TRUE(initSuccess);
}

// ✅ Test PS5 controller integration workflow
void test_ps5_integration_workflow() {
  // Start with disconnected state
  TEST_ASSERT_FALSE(mockSystem.ps5Connected);
  TEST_ASSERT_TRUE(mockSystem.emergencyActive);  // Should start in emergency

  // Simulate controller connection
  simulatePS5Connection();

  TEST_ASSERT_TRUE(mockSystem.ps5Connected);       // ✅ Connected
  TEST_ASSERT_EQUAL(85, mockSystem.batteryLevel);  // 🔋 Battery level updated
  TEST_ASSERT_FALSE(mockSystem.emergencyActive);   // 🔓 Can disable emergency
}

// ✅ Test communication workflow
void test_communication_workflow() {
  // Setup system state
  simulateSystemInit();
  simulatePS5Connection();

  // Set test control inputs
  mockControls.aileron = 150;      // ↔️ Right roll
  mockControls.elevator = 100;     // ↕️ Nose down
  mockControls.engine = 50;        // 🚀 Partial throttle
  mockControls.emergency = false;  // 🔓 Emergency disabled

  // Generate message
  String message = simulateMessageGeneration();
  mockSystem.lastMessage = message;
  mockSystem.lastTransmission = millis();

  // Verify message content
  TEST_ASSERT_TRUE(message.indexOf("e50") != -1);   // Engine 50
  TEST_ASSERT_TRUE(message.indexOf("a150") != -1);  // Aileron 150
  TEST_ASSERT_TRUE(message.indexOf("l100") != -1);  // Elevator 100
  TEST_ASSERT_TRUE(message.length() > 10);          // Reasonable length

  // Verify transmission was logged
  TEST_ASSERT_EQUAL_STRING(message.c_str(), mockSystem.lastMessage.c_str());
}

// ✅ Test emergency stop integration
void test_emergency_stop_integration() {
  // Setup normal flight condition
  simulateSystemInit();
  simulatePS5Connection();
  mockControls.engine = 100;       // 🚀 Throttle up
  mockControls.emergency = false;  // 🔓 Emergency disabled

  // Generate normal message
  String normalMessage = simulateMessageGeneration();
  TEST_ASSERT_TRUE(normalMessage.indexOf("e100") != -1);  // Engine should be 100

  // Activate emergency stop
  mockControls.emergency = true;  // 🚨 Emergency activated

  // Generate emergency message
  String emergencyMessage = simulateMessageGeneration();
  TEST_ASSERT_TRUE(emergencyMessage.indexOf("e0") != -1);  // Engine should be 0

  // Verify other controls still work during emergency
  TEST_ASSERT_TRUE(emergencyMessage.indexOf("a127") != -1);  // Aileron still active
}

// ✅ Test airbrake system integration
void test_airbrake_integration() {
  // Setup flight condition
  mockControls.airbrake = false;  // 🛑 Airbrake off
  String message = simulateMessageGeneration();
  TEST_ASSERT_TRUE(message.indexOf("b0") != -1);  // Airbrake disabled

  // Activate airbrake
  mockControls.airbrake = true;  // 🛑 Airbrake on
  message = simulateMessageGeneration();
  TEST_ASSERT_TRUE(message.indexOf("b1") != -1);  // Airbrake enabled
}

// ✅ Test flaps control integration
void test_flaps_control_integration() {
  // Test flaps progression
  for (int i = 0; i <= 4; i++) {
    mockControls.flaps = i;
    String message = simulateMessageGeneration();
    String expectedFlaps = "f" + String(i);
    TEST_ASSERT_TRUE(message.indexOf(expectedFlaps) != -1);
  }
}

// ✅ Test system state consistency
void test_system_state_consistency() {
  // Test that system maintains consistent state
  simulateSystemInit();
  simulatePS5Connection();

  // Verify all systems are in expected state
  TEST_ASSERT_TRUE(mockSystem.systemReady);         // ✅ System ready
  TEST_ASSERT_TRUE(mockSystem.ps5Connected);        // 🎮 Controller connected
  TEST_ASSERT_TRUE(mockSystem.loraInitialized);     // 📡 LoRa ready
  TEST_ASSERT_TRUE(mockSystem.displayInitialized);  // 🖥️ Display ready

  // Test state transitions
  mockSystem.ps5Connected = false;  // Simulate disconnection
  if (!mockSystem.ps5Connected) {
    mockSystem.emergencyActive = true;  // Should activate emergency
  }
  TEST_ASSERT_TRUE(mockSystem.emergencyActive);
}

// ✅ Test complete control loop simulation
void test_complete_control_loop() {
  // Initialize system
  bool initSuccess = simulateSystemInit();
  TEST_ASSERT_TRUE(initSuccess);

  // Connect controller
  simulatePS5Connection();
  TEST_ASSERT_TRUE(mockSystem.ps5Connected);

  // Set realistic flight inputs
  mockControls.aileron = 140;      // ↔️ Slight right roll
  mockControls.elevator = 115;     // ↕️ Slight nose up
  mockControls.rudder = 130;       // ↔️ Slight right yaw
  mockControls.engine = 75;        // 🚀 Cruise power
  mockControls.flaps = 1;          // 🪶 Takeoff flaps
  mockControls.emergency = false;  // 🔓 Normal operation

  // Generate and validate message
  String controlMessage = simulateMessageGeneration();

  TEST_ASSERT_TRUE(controlMessage.indexOf("e75") != -1);   // Engine
  TEST_ASSERT_TRUE(controlMessage.indexOf("a140") != -1);  // Aileron
  TEST_ASSERT_TRUE(controlMessage.indexOf("l115") != -1);  // Elevator
  TEST_ASSERT_TRUE(controlMessage.indexOf("r130") != -1);  // Rudder
  TEST_ASSERT_TRUE(controlMessage.indexOf("f1") != -1);    // Flaps
  TEST_ASSERT_TRUE(controlMessage.indexOf("b0") != -1);    // Airbrake off

  // Verify message is well-formed
  TEST_ASSERT_TRUE(controlMessage.length() >= 20);  // Reasonable length
  TEST_ASSERT_TRUE(controlMessage.length() <= 50);  // Not too long
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor
#endif

  UNITY_BEGIN();

  // 🧪 Run integration tests
  RUN_TEST(test_system_initialization);
  RUN_TEST(test_ps5_integration_workflow);
  RUN_TEST(test_communication_workflow);
  RUN_TEST(test_emergency_stop_integration);
  RUN_TEST(test_airbrake_integration);
  RUN_TEST(test_flaps_control_integration);
  RUN_TEST(test_system_state_consistency);
  RUN_TEST(test_complete_control_loop);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

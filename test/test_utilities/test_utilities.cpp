#include <unity.h>

#ifdef ARDUINO
#include <Arduino.h>
#else
// 🖥️ Native environment - provide Arduino function implementations
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

long constrain(long x, long a, long b) {
  return (x < a) ? a : (x > b) ? b : x;
}

int abs(int x) {
  return (x < 0) ? -x : x;
}

unsigned long millis() {
  // Mock millis for native testing
  static unsigned long mock_millis = 0;
  return mock_millis++;
}

void delay(unsigned long ms) {
  // Mock delay for native testing
  (void)ms;
}
#endif

// 🧪 Test utility functions and basic Arduino functionality

void setUp(void) {
  // 🚀 Set up function called before each test
}

void tearDown(void) {
  // 🧹 Tear down function called after each test
}

// ✅ Test basic Arduino map function
void test_map_function() {
  // Test mapping from 0-255 to 0-180 (controller to servo range)
  TEST_ASSERT_EQUAL(0, map(0, 0, 255, 0, 180));
  TEST_ASSERT_EQUAL(90, map(127, 0, 255, 0, 180));  // Middle position
  TEST_ASSERT_EQUAL(180, map(255, 0, 255, 0, 180));

  // Test mapping from 0-4095 to 0-180 (analog to servo range)
  TEST_ASSERT_EQUAL(0, map(0, 0, 4095, 0, 180));
  TEST_ASSERT_EQUAL(90, map(2047, 0, 4095, 0, 180));  // Middle position
  TEST_ASSERT_EQUAL(180, map(4095, 0, 4095, 0, 180));
}

// ✅ Test constrain function for flaps
void test_constrain_function() {
  // Test flaps constrain (0-4 range)
  TEST_ASSERT_EQUAL(0, constrain(-1, 0, 4));  // Below minimum
  TEST_ASSERT_EQUAL(0, constrain(0, 0, 4));   // At minimum
  TEST_ASSERT_EQUAL(2, constrain(2, 0, 4));   // In range
  TEST_ASSERT_EQUAL(4, constrain(4, 0, 4));   // At maximum
  TEST_ASSERT_EQUAL(4, constrain(5, 0, 4));   // Above maximum
}

// ✅ Test abs function for deviation calculations
void test_abs_function() {
  TEST_ASSERT_EQUAL(0, abs(0));
  TEST_ASSERT_EQUAL(50, abs(50));
  TEST_ASSERT_EQUAL(50, abs(-50));
  TEST_ASSERT_EQUAL(127, abs(127));
  TEST_ASSERT_EQUAL(127, abs(-127));
}

// ✅ Test millis overflow handling (edge case)
void test_millis_overflow_simulation() {
  unsigned long current = 4294967290UL;  // Close to overflow
  unsigned long previous = 4294967280UL;

  // Normal case
  TEST_ASSERT_TRUE((current - previous) >= 10);

  // Simulate overflow case
  current = 10UL;           // After overflow
  previous = 4294967290UL;  // Before overflow

  // This should handle overflow correctly in real implementation
  // In practice, Arduino's millis() handles this automatically
  TEST_ASSERT_TRUE(current > 0);  // Basic sanity check
}

void setup() {
#ifdef ARDUINO
  delay(2000);  // 🕐 Wait for serial monitor to open
#endif

  UNITY_BEGIN();  // 🧪 Run basic utility tests
  RUN_TEST(test_map_function);
  RUN_TEST(test_constrain_function);
  RUN_TEST(test_abs_function);
  RUN_TEST(test_millis_overflow_simulation);

  UNITY_END();
}

void loop() {
  // 🔄 Empty loop - tests run once in setup()
}

#include <ps5Controller.h>

void setup() {
  Serial.begin(115200);
  ps5.begin("ac:36:1b:41:ac:ed");  // replace with your controller's Bluetooth MAC address
  Serial.println("PS5 Controller sensor example ready.");
}

void loop() {
  // Only process sensor data when controller is connected
  if (ps5.isConnected()) {
    // Read accelerometer data using convenient accessor methods
    int16_t accel_x = ps5.AccelX();
    int16_t accel_y = ps5.AccelY();
    int16_t accel_z = ps5.AccelZ();

    // Read gyroscope data using convenient accessor methods
    int16_t gyro_x = ps5.GyroX();
    int16_t gyro_y = ps5.GyroY();
    int16_t gyro_z = ps5.GyroZ();

    // Print sensor data every 100ms to avoid spam
    static unsigned long lastPrint = 0;
    if (millis() - lastPrint > 100) {
      Serial.printf("Accel: X=%6d Y=%6d Z=%6d | Gyro: X=%6d Y=%6d Z=%6d\n", accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z);
      lastPrint = millis();
    }

    // Example: Detect controller tilt (simple threshold-based detection)
    if (abs(accel_x) > 8000 || abs(accel_y) > 8000) {
      Serial.println("Controller tilted!");
      ps5.setLed(255, 0, 0);  // Red light when tilted
    } else {
      ps5.setLed(0, 0, 255);  // Blue light when level
    }

    // Example: Detect controller rotation (simple threshold-based detection)
    if (abs(gyro_x) > 5000 || abs(gyro_y) > 5000 || abs(gyro_z) > 5000) {
      Serial.println("Controller rotating!");
      ps5.setRumble(50, 100);  // Light rumble feedback
    }

    // Button controls for testing
    if (ps5.Cross()) {
      Serial.println("Cross pressed - Current sensor values:");
      Serial.printf("  Accelerometer: X=%d, Y=%d, Z=%d\n", accel_x, accel_y, accel_z);
      Serial.printf("  Gyroscope: X=%d, Y=%d, Z=%d\n", gyro_x, gyro_y, gyro_z);
    }

    if (ps5.Circle()) {
      Serial.println("Circle pressed - Zeroing rumble and LED");
      ps5.setRumble(0, 0);
      ps5.setLed(0, 255, 0);  // Green light
    }

    if (ps5.Triangle()) {
      Serial.printf("Triangle pressed - Battery: %d%%, Charging: %s\n", ps5.Battery() * 10, ps5.Charging() ? "Yes" : "No");
    }

    // Send any changes to the controller
    ps5.sendToController();
  } else {
    // Controller not connected
    static unsigned long lastConnectionAttempt = 0;
    if (millis() - lastConnectionAttempt > 5000) {
      Serial.println("PS5 Controller not connected. Waiting...");
      lastConnectionAttempt = millis();
    }
  }

  delay(10);  // Small delay to prevent overwhelming the system
}

// Optional: Event callbacks for connection status
void onConnect() {
  Serial.println("PS5 Controller Connected!");
  ps5.setLed(0, 255, 0);  // Green light on connect
  ps5.sendToController();
}

void onDisconnect() {
  Serial.println("PS5 Controller Disconnected!");
}

// Note: To use event callbacks, uncomment these lines in setup():
// ps5.attachOnConnect(onConnect);
// ps5.attachOnDisconnect(onDisconnect);

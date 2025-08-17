#include <ps5Controller.h>

// Sensor calibration and filtering
struct SensorCalibration {
  int16_t accel_offset_x = 0;
  int16_t accel_offset_y = 0;
  int16_t accel_offset_z = 0;
  int16_t gyro_offset_x = 0;
  int16_t gyro_offset_y = 0;
  int16_t gyro_offset_z = 0;
  bool calibrated = false;
};

struct FilteredSensor {
  float accel_x = 0.0;
  float accel_y = 0.0;
  float accel_z = 0.0;
  float gyro_x = 0.0;
  float gyro_y = 0.0;
  float gyro_z = 0.0;
  float alpha = 0.1;  // Low-pass filter coefficient (0-1, higher = less filtering)
};

SensorCalibration cal;
FilteredSensor filtered;

// Motion detection
struct MotionState {
  bool tilted = false;
  bool rotating = false;
  float tilt_angle = 0.0;
  unsigned long last_motion_time = 0;
};

MotionState motion;

void setup() {
  Serial.begin(115200);
  ps5.begin("ac:36:1b:41:ac:ed");  // replace with your controller's Bluetooth MAC address

  Serial.println("PS5 Advanced Sensor Example");
  Serial.println("Commands:");
  Serial.println("  Cross: Calibrate sensors (hold controller level)");
  Serial.println("  Circle: Toggle motion detection display");
  Serial.println("  Triangle: Reset calibration");
  Serial.println("  Square: Show raw sensor values");
  Serial.println("");
}

void calibrateSensors() {
  Serial.println("Calibrating sensors... Hold controller level and still.");

  // Take 100 samples for calibration
  long sum_accel_x = 0, sum_accel_y = 0, sum_accel_z = 0;
  long sum_gyro_x = 0, sum_gyro_y = 0, sum_gyro_z = 0;

  for (int i = 0; i < 100; i++) {
    sum_accel_x += ps5.AccelX();
    sum_accel_y += ps5.AccelY();
    sum_accel_z += ps5.AccelZ();
    sum_gyro_x += ps5.GyroX();
    sum_gyro_y += ps5.GyroY();
    sum_gyro_z += ps5.GyroZ();
    delay(10);
  }

  cal.accel_offset_x = sum_accel_x / 100;
  cal.accel_offset_y = sum_accel_y / 100;
  cal.accel_offset_z = sum_accel_z / 100;
  cal.gyro_offset_x = sum_gyro_x / 100;
  cal.gyro_offset_y = sum_gyro_y / 100;
  cal.gyro_offset_z = sum_gyro_z / 100;
  cal.calibrated = true;

  Serial.printf("Calibration complete!\n");
  Serial.printf("Accel offsets: X=%d, Y=%d, Z=%d\n", cal.accel_offset_x, cal.accel_offset_y, cal.accel_offset_z);
  Serial.printf("Gyro offsets: X=%d, Y=%d, Z=%d\n", cal.gyro_offset_x, cal.gyro_offset_y, cal.gyro_offset_z);

  ps5.setLed(0, 255, 0);  // Green = calibrated
  ps5.sendToController();
}

void updateFilteredSensors() {
  // Get raw sensor values
  int16_t raw_accel_x = ps5.AccelX();
  int16_t raw_accel_y = ps5.AccelY();
  int16_t raw_accel_z = ps5.AccelZ();
  int16_t raw_gyro_x = ps5.GyroX();
  int16_t raw_gyro_y = ps5.GyroY();
  int16_t raw_gyro_z = ps5.GyroZ();

  // Apply calibration if available
  if (cal.calibrated) {
    raw_accel_x -= cal.accel_offset_x;
    raw_accel_y -= cal.accel_offset_y;
    raw_accel_z -= cal.accel_offset_z;
    raw_gyro_x -= cal.gyro_offset_x;
    raw_gyro_y -= cal.gyro_offset_y;
    raw_gyro_z -= cal.gyro_offset_z;
  }

  // Apply low-pass filter (simple exponential moving average)
  filtered.accel_x = filtered.accel_x * (1.0 - filtered.alpha) + raw_accel_x * filtered.alpha;
  filtered.accel_y = filtered.accel_y * (1.0 - filtered.alpha) + raw_accel_y * filtered.alpha;
  filtered.accel_z = filtered.accel_z * (1.0 - filtered.alpha) + raw_accel_z * filtered.alpha;
  filtered.gyro_x = filtered.gyro_x * (1.0 - filtered.alpha) + raw_gyro_x * filtered.alpha;
  filtered.gyro_y = filtered.gyro_y * (1.0 - filtered.alpha) + raw_gyro_y * filtered.alpha;
  filtered.gyro_z = filtered.gyro_z * (1.0 - filtered.alpha) + raw_gyro_z * filtered.alpha;
}

void detectMotion() {
  // Calculate tilt angle (simplified - actual calculation would use all 3 axes)
  motion.tilt_angle = sqrt(filtered.accel_x * filtered.accel_x + filtered.accel_y * filtered.accel_y) / 1000.0;

  // Detect significant tilt (threshold tuned for PS5 controller)
  motion.tilted = (abs(filtered.accel_x) > 3000 || abs(filtered.accel_y) > 3000);

  // Detect rotation (any axis)
  motion.rotating = (abs(filtered.gyro_x) > 2000 || abs(filtered.gyro_y) > 2000 || abs(filtered.gyro_z) > 2000);

  if (motion.tilted || motion.rotating) {
    motion.last_motion_time = millis();
  }
}

void loop() {
  if (ps5.isConnected()) {
    updateFilteredSensors();
    detectMotion();

    // Handle button presses
    static bool cross_pressed = false;
    static bool circle_pressed = false;
    static bool triangle_pressed = false;
    static bool square_pressed = false;
    static bool show_motion = true;

    if (ps5.Cross() && !cross_pressed) {
      cross_pressed = true;
      calibrateSensors();
    } else if (!ps5.Cross()) {
      cross_pressed = false;
    }

    if (ps5.Circle() && !circle_pressed) {
      circle_pressed = true;
      show_motion = !show_motion;
      Serial.printf("Motion detection display: %s\n", show_motion ? "ON" : "OFF");
    } else if (!ps5.Circle()) {
      circle_pressed = false;
    }

    if (ps5.Triangle() && !triangle_pressed) {
      triangle_pressed = true;
      cal.calibrated = false;
      Serial.println("Calibration reset");
      ps5.setLed(255, 255, 0);  // Yellow = needs calibration
      ps5.sendToController();
    } else if (!ps5.Triangle()) {
      triangle_pressed = false;
    }

    if (ps5.Square() && !square_pressed) {
      square_pressed = true;
      Serial.printf("Raw: A(%d,%d,%d) G(%d,%d,%d)\n", ps5.AccelX(), ps5.AccelY(), ps5.AccelZ(), ps5.GyroX(), ps5.GyroY(), ps5.GyroZ());
    } else if (!ps5.Square()) {
      square_pressed = false;
    }

    // Display sensor data periodically
    static unsigned long lastDisplay = 0;
    if (millis() - lastDisplay > 200 && show_motion) {  // 5Hz display rate
      if (cal.calibrated) {
        Serial.printf("Filtered: A(%.1f,%.1f,%.1f) G(%.1f,%.1f,%.1f) | Tilt:%.2f° %s%s\n", filtered.accel_x, filtered.accel_y,
                      filtered.accel_z, filtered.gyro_x, filtered.gyro_y, filtered.gyro_z, motion.tilt_angle,
                      motion.tilted ? " TILTED" : "", motion.rotating ? " ROTATING" : "");
      } else {
        Serial.printf("Raw: A(%d,%d,%d) G(%d,%d,%d) [NEEDS CALIBRATION - Press Cross]\n", ps5.AccelX(), ps5.AccelY(), ps5.AccelZ(),
                      ps5.GyroX(), ps5.GyroY(), ps5.GyroZ());
      }
      lastDisplay = millis();
    }

    // Visual feedback via LED
    if (!cal.calibrated) {
      // Yellow blink = needs calibration
      static bool blink = false;
      static unsigned long lastBlink = 0;
      if (millis() - lastBlink > 500) {
        blink = !blink;
        ps5.setLed(blink ? 255 : 0, blink ? 255 : 0, 0);
        ps5.sendToController();
        lastBlink = millis();
      }
    } else if (motion.tilted) {
      ps5.setLed(255, 0, 0);  // Red = tilted
      ps5.sendToController();
    } else if (motion.rotating) {
      ps5.setLed(0, 0, 255);  // Blue = rotating
      ps5.sendToController();
    } else {
      ps5.setLed(0, 255, 0);  // Green = normal
      ps5.sendToController();
    }

    // Haptic feedback for motion
    if (motion.tilted && (millis() - motion.last_motion_time < 100)) {
      ps5.setRumble(50, 0);  // Light rumble for tilt
      ps5.sendToController();
    } else if (motion.rotating && (millis() - motion.last_motion_time < 100)) {
      ps5.setRumble(0, 80);  // Different rumble for rotation
      ps5.sendToController();
    } else {
      ps5.setRumble(0, 0);
      ps5.sendToController();
    }

  } else {
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 5000) {
      Serial.println("PS5 Controller not connected. Make sure MAC address is correct.");
      lastMsg = millis();
    }
  }

  delay(10);
}

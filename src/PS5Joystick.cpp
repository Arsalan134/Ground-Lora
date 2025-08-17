#include "Header Files\PS5Joystick.h"
#include "Common\common.h"

unsigned long lastTimeStamp = 0;

unsigned long lastFlapsChangeTimestamp = 0;

bool PS4AccelerometerEnabled = false;
uint8_t batteryPercentage = 0;

void removePairedDevices() {
  uint8_t pairedDeviceBtAddr[20][6];
  int count = esp_bt_gap_get_bond_device_num();
  esp_bt_gap_get_bond_device_list(&count, pairedDeviceBtAddr);
  for (int i = 0; i < count; i++) {
    esp_bt_gap_remove_bond_device(pairedDeviceBtAddr[i]);
  }
}

void printDeviceAddress() {
  const uint8_t* point = esp_bt_dev_get_address();

  Serial.print("This device MAC is: ");

  for (int i = 0; i < 6; i++) {
    char str[3];
    sprintf(str, "%02x", (int)point[i]);
    Serial.print(str);
    if (i < 5) {
      Serial.print(":");
    }
  }

  Serial.println();
}

// 🎮
void notify() {
  if (millis() - lastTimeStamp > 20) {
    lastTimeStamp = millis();

    if (ps5.Up())  // Up Button ⬆️
      sendingElevatorTrimMessage = 1;

    if (ps5.Down())  // Down Button ⬇️
      sendingElevatorTrimMessage = -1;

    if (ps5.Right())  // Right Button ➡️
      sendingAileronTrimMessage = 1;

    if (ps5.Left())  // Left Button ⬅️
      sendingAileronTrimMessage = -1;

    // if (ps5.Square()) // Square Button ⏹️

    if (ps5.Cross()) {  // Cross Button ❌
      digitalWrite(BUILTIN_LED, 1);
      isEmergencyStopEnabled = false;
      airbrakeEnabled = false;
    } else
      digitalWrite(BUILTIN_LED, 0);

    if (ps5.Circle())  // Circle Button ⭕
      isEmergencyStopEnabled = true;

    // if (ps5.Triangle()) // Triangle Button 🔺

    if (ps5.L1() && millis() - lastFlapsChangeTimestamp > 200) {
      sendingFlapsMessage = constrain(sendingFlapsMessage - 1, 0, 4);
      lastFlapsChangeTimestamp = millis();
    }

    if (ps5.R1() && millis() - lastFlapsChangeTimestamp > 200) {
      sendingFlapsMessage = constrain(sendingFlapsMessage + 1, 0, 4);
      lastFlapsChangeTimestamp = millis();
    }

    // if (ps5.Share()) // Share Button 🔗

    if (ps5.Options())  // Options Button ⚙️
      PS4AccelerometerEnabled = !PS4AccelerometerEnabled;

    if (ps5.L3())  // L3 Button 🔘
      resetAileronTrim = true;

    if (ps5.R3())  // R3 Button 🔘
      resetElevatorTrim = true;

    if (ps5.PSButton())  // PS Button ⏹️
      airbrakeEnabled = true;

    // if (ps5.Touchpad()) // Touch Pad Button 🖱️

    // 🕹️
    sendingAileronMessage = ps5.LStickX() + 128;
    sendingRudderMessage = ps5.RStickX() + 128;
    sendingElevatorsMessage = ps5.RStickY() + 128;

#if EVENTS
    boolean sqd = ps5.event.button_down.square, squ = ps5.event.button_up.square, trd = ps5.event.button_down.triangle,
            tru = ps5.event.button_up.triangle;
    // crossD = ps5.event.button_down.cross, crossU = ps5.event.button_up.cross;

    // if (sqd)
    //   Serial.println("SQUARE down");
    // else if (squ)
    //   Serial.println("SQUARE up");
    // else if (trd)
    //   Serial.println("TRIANGLE down");
    // else if (tru)
    //   Serial.println("TRIANGLE up");

    // if (crossD) {
    //   Serial.println("Cross Button ❌");
    //   digitalWrite(BUILTIN_LED, 1);
    // } else if (crossU)
    //   digitalWrite(BUILTIN_LED, 0);

#endif

#if BUTTONS
    boolean sq = ps5.Square(), tr = ps5.Triangle();
    if (sq)
      Serial.print("SQUARE pressed");
    if (tr)
      Serial.print("TRIANGLE pressed");
    if (sq | tr)
      Serial.println();
#endif

    // Only needed to print the message properly on serial monitor. Else we dont need it.
    //   if (millis() - lastTimeStamp > 50) {
    // #if JOYSTICKS
    //     Serial.printf("lx:%4d,ly:%4d,rx:%4d,ry:%4d\n", ps5.LStickX(), ps5.LStickY(),
    //     ps5.RStickX(),
    //                   ps5.RStickY());

    // #endif
    // #if SENSORS
    //     Serial.printf("gx:%5d,gy:%5d,gz:%5d,ax:%5d,ay:%5d,az:%5d\n", ps5.GyrX(), ps5.GyrY(),
    //     ps5.GyrZ(),
    //                   ps5.AccX(), ps5.AccY(), ps5.AccZ());
    // #endif
    //     lastTimeStamp = millis();
    //   }
  }
}

void onConnect() {
  Serial.println("Connected!");
  display.setOverlays(allOverlays, 1);
}

void onDisconnect() {
  Serial.println("Disconnected!");
}
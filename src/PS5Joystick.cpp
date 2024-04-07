#include "PS5Joystick.h"

unsigned long lastTimeStamp = 0;

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
  for (int i = 0; i < 6; i++) {
    char str[3];
    sprintf(str, "%02x", (int)point[i]);
    Serial.print(str);
    if (i < 5) {
      Serial.print(":");
    }
  }
}

void notify() {
  // if (ps5.Right())
  //   Serial.println("Right Button");

  // if (ps5.Down())
  //   Serial.println("Down Button");

  // if (ps5.Left())
  //   Serial.println("Left Button");

  // if (ps5.Up())
  //   Serial.println("Up Button");

  // if (ps5.Square())
  //   Serial.println("Square Button");

  if (ps5.Cross()) {
    // Serial.println("Cross Button");
    digitalWrite(BUILTIN_LED, 1);
  } else {
    digitalWrite(BUILTIN_LED, 0);
  }

  // if (ps5.Circle())
  //   Serial.println("Circle Button");

  // if (ps5.Triangle())
  //   Serial.println("Triangle Button");

  // if (ps5.UpRight())
  //   Serial.println("Up Right");

  // if (ps5.DownRight())
  //   Serial.println("Down Right");

  // if (ps5.UpLeft())
  //   Serial.println("Up Left");

  // if (ps5.DownLeft())
  //   Serial.println("Down Left");

  // if (ps5.L1())
  //   Serial.println("L1 Button");

  // if (ps5.R1())
  //   Serial.println("R1 Button");

  // if (ps5.Share())
  //   Serial.println("Share Button");

  // if (ps5.Options())
  //   Serial.println("Options Button");

  // if (ps5.L3())
  //   Serial.println("L3 Button");

  // if (ps5.R3())
  //   Serial.println("R3 Button");

  // if (ps5.PSButton())
  //   Serial.println("PS Button");

  // if (ps5.Touchpad())
  //   Serial.println("Touch Pad Button");

  // if (ps5.L2())
  //   Serial.printf("L2 button at %d\n", ps5.L2Value());

  // if (ps5.R2())
  //   Serial.printf("R2 button at %d\n", ps5.R2Value());

  // if (abs(ps5.LStickX()) > 20)
  //   Serial.printf("Left Stick x at %d\n", ps5.LStickX());

  // if (abs(ps5.LStickY()) > 20)
  //   Serial.printf("Left Stick y at %d\n", ps5.LStickY());

  // if (abs(ps5.RStickX()) > 20)
  //   Serial.printf("Right Stick x at %d\n", ps5.RStickX());

  // if (abs(ps5.RStickY()) > 20)
  //   Serial.printf("Right Stick y at %d\n", ps5.RStickY());

#if EVENTS
  boolean sqd = ps5.event.button_down.square, squ = ps5.event.button_up.square,
          trd = ps5.event.button_down.triangle, tru = ps5.event.button_up.triangle;

  if (sqd)
    Serial.println("SQUARE down");
  else if (squ)
    Serial.println("SQUARE up");
  else if (trd)
    Serial.println("TRIANGLE down");
  else if (tru)
    Serial.println("TRIANGLE up");
#endif

#if BUTTONS
  boolean sq = ps5.Square(), tr = ps5.Triangle();
  if (sq)
    Serial.print(" SQUARE pressed");
  if (tr)
    Serial.print(" TRIANGLE pressed");
  if (sq | tr)
    Serial.println();
#endif

  // Only needed to print the message properly on serial monitor. Else we dont need it.
  //   if (millis() - lastTimeStamp > 50) {
  // #if JOYSTICKS
  //     Serial.printf("lx:%4d,ly:%4d,rx:%4d,ry:%4d\n", ps5.LStickX(), ps5.LStickY(), ps5.RStickX(),
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

void onConnect() {
  Serial.println("Connected!");
}

void onDisconnect() {
  Serial.println("Disconnected!");
}
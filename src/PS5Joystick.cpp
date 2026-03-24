#include "PS5Joystick.h"
#include "common.h"

unsigned long lastTimeStamp = 0;

unsigned long lastFlapsChangeTimestamp = 0;
unsigned long lastExpoChangeTimestamp = 0;
unsigned long lastShareChangeTimestamp = 0;

uint8_t batteryPercentage = 0;

// 🎮 Software disconnect detection (library callback is unreliable)
static bool wasConnected = false;

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

  Serial.print("📱 This device MAC is: ");

  if (point == NULL) {
    Serial.println("⚠️  Bluetooth not ready");
    return;
  }

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

    if (ps5.Square() && millis() - lastExpoChangeTimestamp > 300) {  // 🟥 Square: cycle expo preset
      cycleExpoPreset();
      lastExpoChangeTimestamp = millis();
    }

    if (ps5.Cross()) {  // Cross Button ❌
      digitalWrite(BUILTIN_LED, 1);
      isEmergencyStopEnabled = false;  // 🔓 Disable emergency stop
      airbrakeEnabled = false;         // 🚀 Disable airbrake
      acsEngageEnabled = false;        // 🤖 Disable ACS
      // ⏱️ Start flight timer on arm
      if (!flightTimerRunning) {
        flightTimerStartMs = millis();
        flightTimerRunning = true;
      }
    } else
      digitalWrite(BUILTIN_LED, 0);

    if (ps5.Circle()) {               // Circle Button ⭕
      isEmergencyStopEnabled = true;  // 🚨 Enable emergency stop
      flightTimerRunning = false;     // ⏱️ Stop flight timer on disarm
    }

    if (ps5.Triangle())         // Triangle Button 🔺
      acsEngageEnabled = true;  // 🤖 Engage ACS autopilot

    if (ps5.L1() && millis() - lastFlapsChangeTimestamp > 200) {
      sendingFlapsMessage = constrain(sendingFlapsMessage - 1, 0, 4);  // ⬇️ Decrease flaps
      lastFlapsChangeTimestamp = millis();
    }

    if (ps5.R1() && millis() - lastFlapsChangeTimestamp > 200) {
      sendingFlapsMessage = constrain(sendingFlapsMessage + 1, 0, 4);  // ⬆️ Increase flaps
      lastFlapsChangeTimestamp = millis();
    }

    if (ps5.Share() && millis() - lastShareChangeTimestamp > 300) {  // 🌿 Share: toggle ECO mode
      ecoModeEnabled = !ecoModeEnabled;
      lastShareChangeTimestamp = millis();
    }

    // if (ps5.Options())  // Options Button ⚙️

    if (ps5.L3())               // L3 Button 🔘
      resetAileronTrim = true;  // 🔄 Reset aileron trim

    if (ps5.R3())                // R3 Button 🔘
      resetElevatorTrim = true;  // 🔄 Reset elevator trim

    if (ps5.PSButton())        // PS Button ⏹️
      airbrakeEnabled = true;  // 🛑 Enable airbrake

    // if (ps5.Touchpad()) // Touch Pad Button 🖱️

    // 🕹️ Joystick inputs
    sendingAileronMessage = ps5.LStickX() + 128;    // ↔️ Aileron
    sendingRudderMessage = ps5.RStickX() + 128;     // ↔️ Rudder
    sendingElevatorsMessage = ps5.RStickY() + 128;  // ↕️ Elevators

    // 🛡️ Stability Assist — L2 analog trigger (0-255)
    stabilityAssistValue = ps5.L2Value();

#if EVENTS
    boolean sqd = ps5.event.button_down.square, squ = ps5.event.button_up.square, trd = ps5.event.button_down.triangle,
            tru = ps5.event.button_up.triangle;
    // crossD = ps5.event.button_down.cross, crossU = ps5.event.button_up.cross;

    // if (sqd)
    //   Serial.println("🟨 SQUARE down");
    // else if (squ)
    //   Serial.println("🟨 SQUARE up");
    // else if (trd)
    //   Serial.println("🔺 TRIANGLE down");
    // else if (tru)
    //   Serial.println("🔺 TRIANGLE up");

    // if (crossD) {
    //   Serial.println("❌ Cross Button");
    //   digitalWrite(BUILTIN_LED, 1);
    // } else if (crossU)
    //   digitalWrite(BUILTIN_LED, 0);

#endif
  }
}

void onConnect() {
  Serial.println("✅ Connected!");
  wasConnected = true;
  display.setOverlays(allOverlays, 1);  // Enable all overlays: BT
}

void onDisconnect() {
  Serial.println("❌ Disconnected!");
  wasConnected = false;
  display.setOverlays(allOverlays, 0);  // Disable all overlays
}

// 🎮 Poll-based disconnect detection — call from loop/task
// The ps5Controller onDisconnect callback often doesn't fire,
// so we detect the transition ourselves.
void checkPS5Connection() {
  bool connected = ps5.isConnected();
  if (wasConnected && !connected) {
    onDisconnect();
  }
}
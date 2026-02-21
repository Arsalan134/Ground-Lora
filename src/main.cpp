#include "main.h"

// Display (TTGO LoRa32 V2.1 built-in OLED)
SSD1306Wire ui(0x3c, SDA_PIN, SCL_PIN);
OLEDDisplayUi display(&ui);

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = {drawFrame1};

bool setToZeroEngineSlider = false;
bool isEmergencyStopEnabled = true;

// 🖥️ Display task handle (runs on Core 0)
static TaskHandle_t displayTaskHandle = NULL;

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);

  // Set LoRa pins (built-in SX1276 on TTGO LoRa32 V2.1)
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);

  // pins_arduino.h 📱
  setupDisplay();  // 🖥️
  // setupSD();      // 💾
  setupPS5();    // 🎮
  setupRadio();  // 📡

  // 🖥️ Create display task on Core 0 (frees Core 1 for realtime control)
  xTaskCreatePinnedToCore(
    [](void* pvParameters) {
      while (true) {
        display.update();
        vTaskDelay(pdMS_TO_TICKS(16));  // ~60 FPS
      }
    },
    "DisplayTask",
    4096,
    NULL,
    1,     // Low priority
    &displayTaskHandle,
    0      // Core 0
  );
  Serial.println(F("✅ DisplayTask created on Core 0"));
}

void loop() {
  // Feed the watchdog
  yield();

  // Display rendering moved to Core 0 DisplayTask

  // Get engine value from slider
  sendingEngineMessage = max((int)analogRead(sliderPin), (int)map(ps5.R2Value(), 0, 255, 0, PROTO_ENGINE_RAW_MAX));

  if (!sendingEngineMessage)       // If engine message is not being sent
    setToZeroEngineSlider = true;  // Set the slider to zero. For safety measures

  if (!setToZeroEngineSlider && sendingEngineMessage) {
    // For safety measures, if engine value is non zero, return 🚨
    Serial.println("⚠️  Engine value is non zero, returning.");
    delay(100);
    return;
  }

  if (ps5.isConnected())  // 🎮✅
    loraLoop();           // 📡

  vTaskDelay(pdMS_TO_TICKS(1));  // Minimal yield instead of delay(10)
}

void setupRadio() {
  Serial.print("📡 Initializing LoRa1276 (SX1276)... ");

  // Initialize LoRa with frequency (from protocol.h)
  if (!LoRa.begin(PROTO_LORA_FREQUENCY_HZ)) {
    Serial.println("❌ LoRa init failed! Check wiring.");
    Serial.printf("   CS:   Pin %d\n", LORA_CS);
    Serial.printf("   RST:  Pin %d\n", LORA_RST);
    Serial.printf("   DIO0: Pin %d\n", LORA_DIO0);
    lora_initialized = false;
    return;  // ⚠️ Continue without LoRa instead of halting
  }

  lora_initialized = true;

  Serial.println("✅ LoRa init succeeded.");

  // Configure radio settings (parameters from shared protocol.h)
  LoRa.setSpreadingFactor(PROTO_LORA_SF);
  LoRa.setSignalBandwidth(PROTO_LORA_BANDWIDTH_HZ);
  LoRa.setCodingRate4(PROTO_LORA_CR);
  LoRa.setSyncWord(PROTO_LORA_SYNC_WORD);
  LoRa.setTxPower(PROTO_LORA_TX_POWER);
  LoRa.setPreambleLength(PROTO_LORA_PREAMBLE);
  LoRa.enableCrc();

  Serial.println();
  Serial.println("📡 LoRa Ground Station");
  Serial.println("📡 Using LoRa1276 (SX1276) at 915MHz");
  Serial.printf("   Frequency:  %ld Hz\n", PROTO_LORA_FREQUENCY_HZ);
  Serial.printf("   Bandwidth:  %ld Hz\n", PROTO_LORA_BANDWIDTH_HZ);
  Serial.printf("   SF:         %d\n", PROTO_LORA_SF);
  Serial.printf("   CR:         4/%d\n", PROTO_LORA_CR);
  Serial.printf("   Sync Word:  0x%02X\n", PROTO_LORA_SYNC_WORD);
  Serial.printf("   TX Power:   %d dBm\n", PROTO_LORA_TX_POWER);
  Serial.println();

  // Send initial test packet 🚀
  ProtoCmdPacket initPkt = {0};
  initPkt.magic = PROTO_CMD_MAGIC;
  initPkt.ailerons = 90;
  initPkt.rudder = 90;
  initPkt.elevators = 90;
  initPkt.checksum = proto_checksum((const uint8_t*)&initPkt, PROTO_CMD_PACKET_SIZE - 1);
  LoRa_sendPacket((const uint8_t*)&initPkt, PROTO_CMD_PACKET_SIZE);
  Serial.println("✅ Initial packet sent");
  Serial.println("📡 Radio configuration complete");
}

void setupPS5() {
  // removePairedDevices();  // 🧹 Clear previous pairings

  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);

  ps5.begin(PS5_MAC_ADDRESS);  // 🎮 MAC addresss of Joystick

  printDeviceAddress();  // 📱 Print device MAC
}

// The ESP is capable of rendering 60fps in 80Mhz mode
// but that won't give you much time for anything else
// run it in 160Mhz mode or just set it to 30 fps 🖥️
void setupDisplay() {
  display.setTargetFPS(60);  // 🎯 Target 60 FPS

  // Customize the active and inactive symbol ⭕❌
  display.setActiveSymbol(activeSymbol);
  display.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT 🧭
  display.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar. ⬅️➡️
  display.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used 🔄
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  display.setFrameAnimation(SLIDE_LEFT);

  // Add frames 🖼️
  display.setFrames(frames, frameCount);
  display.disableAutoTransition();
  display.disableAllIndicators();

  // Add overlays 📱
  // display.setOverlays(all, 2);

  // Initialising the UI will init the display too. 🚀
  ui.init();

  ui.flipScreenVertically();  // 🔄 Flip display

  ui.setTextAlignment(TEXT_ALIGN_LEFT);  // ⬅️ Left align
  ui.setFont(ArialMT_Plain_10);          // 🔤 Set font
}

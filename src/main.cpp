#include "main.h"

// Display
SSD1306Wire ui(0x3c, SDA, SCL);
OLEDDisplayUi display(&ui);

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = {drawFrame1};

bool setToZeroEngineSlider = false;
bool isEmergencyStopEnabled = true;

static unsigned long lastDisplayUpdate = 0;

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);

  // Initialize SPI with custom pins 📡
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

  // pins_arduino.h 📱
  setupDisplay();  // 🖥️
  // setupSD();      // 💾
  // setupPS5();    // 🎮
  setupRadio();  // 📡
}

void loop() {
  // Feed the watchdog
  yield();

  // DISPLAY DISABLED - Causing watchdog timeout
  // TODO: Move display to separate RTOS task on Core 0
  display.update();

  // Get engine value
  sendingEngineMessage = max((int)analogRead(sliderPin), (int)map(rand() % 255, 0, 255, 0, 4095));

  // For testing: Simple fixed value to avoid safety checks
  // sendingEngineMessage = 100;  // Fixed value for testing LoRa

  // if (ps5.isConnected())  // 🎮✅
  loraLoop();  // 📡

  delay(10);  // Small delay to prevent tight loop
}

void setupRadio() {
  Serial.print("📡 Initializing SX1262... ");

  // Initialize SX1262 with pin configuration
  int state = radio.begin(frequency);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("❌ LoRa init failed, code: ");
    Serial.println(state);
    while (true) {
      delay(100);
    }  // Halt on error
  }

  Serial.println("✅ LoRa init succeeded.");
  Serial.println();
  Serial.println("📡 LoRa Ground Station");
  Serial.println("📡 Using SX1262 at 915MHz");
  Serial.println();

  // Configure radio settings
  radio.setOutputPower(22);     // 📡 Set TX power to 22 dBm (max for SX1262)
  radio.setSpreadingFactor(7);  // 📡 SF7 for faster data rate
  radio.setBandwidth(125.0);    // 📡 125 kHz bandwidth
  radio.setCodingRate(5);       // 📡 CR 4/5
  radio.setPreambleLength(8);   // 📡 8 symbol preamble
  // DON'T use interrupt - causes watchdog timeout in ISR
  // radio.setDio1Action(onTxDone);

  // Send initial packet 🚀 (blocking mode)
  String msg = "🎯 Setup Completed!";
  int txState = radio.transmit(msg);
  if (txState == RADIOLIB_ERR_NONE) {
    Serial.println("✅ Initial packet sent");
  }

  Serial.println("📡 Radio configuration complete");
}

void setupPS5() {
  // removePairedDevices();  // 🧹 Clear previous pairings

  // ps5.attach(notify);
  // ps5.attachOnConnect(onConnect);
  // ps5.attachOnDisconnect(onDisconnect);

  // ps5.begin(PS5_MAC_ADDRESS);  // 🎮 MAC addresss of Joystick

  // printDeviceAddress();  // 📱 Print device MAC
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

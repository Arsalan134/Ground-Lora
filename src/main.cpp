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

  // pins_arduino.h 📱
  setupDisplay();  // 🖥️
  // setupSD();    // 💾
  setupPS5();    // 🎮
  setupRadio();  // 📡
}

void loop() {
  // Display
  int remainingTimeBudget = display.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your time budget.

    sendingEngineMessage = max((int)analogRead(sliderPin), (int)map(ps5.R2Value(), 0, 255, 0, 4095));

    delay(remainingTimeBudget);
  }

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
}

void setupRadio() {
  Serial.println("📡 Initializing LoRa radio...");

  // Configure TTGO LoRa32 v2.1 pins
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  while (!LoRa.begin(frequency)) {
    Serial.println("❌ LoRa init failed. Check your connections.");
    delay(200);
  }

  Serial.println("✅ LoRa init succeeded.");
  Serial.println();
  Serial.println("📡 LoRa Ground Station (TX-only mode)");
  Serial.println();

  // Configure LoRa parameters to match receiver (Fatima Board)
  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);  // 📡 Max power (20 dBm) - Full power!
  LoRa.setSpreadingFactor(10);                  // 📡 SF10 (MUST match receiver!)
  LoRa.setSignalBandwidth(125E3);               // 📡 125 kHz bandwidth
  LoRa.setCodingRate4(5);                       // 📡 CR 4/5
  LoRa.setSyncWord(0x12);                       // 📡 Sync word 0x12 (MUST match receiver!)
  LoRa.setPreambleLength(8);                    // 📡 Preamble 8 symbols (MUST match receiver!)
  LoRa.disableCrc();                            // 📡 Disable CRC (match receiver)
  LoRa.disableInvertIQ();                       // 📡 Normal IQ (MUST match receiver!)
  LoRa.setOCP(240);                             // 📡 Over Current Protection: 240mA (max for high power)

  Serial.println("📡 LoRa Configuration:");
  Serial.printf("   Frequency: %ld Hz\n", frequency);
  Serial.println("   TX Power: 20 dBm (100mW) - FULL POWER");
  Serial.println("   Spreading Factor: 10");
  Serial.println("   Bandwidth: 125 kHz");
  Serial.println("   Coding Rate: 4/5");
  Serial.println("   Sync Word: 0x12");
  Serial.println("   Preamble: 8 symbols");
  Serial.println("   IQ Inversion: Disabled");
  Serial.println("   OCP: 240mA");
  Serial.println();

  // TX-only mode - no need for callbacks or mode switching
  LoRa.onTxDone(onTxDone);

  // Stay in idle/TX mode (no RX mode switching)
  LoRa.idle();

  delay(100);  // Give it time to stabilize

  // Send initial packet to confirm setup
  Serial.println("📤 Sending initialization packet...");
  LoRa.beginPacket();
  LoRa.print("🎯 Ground Station Ready!");
  LoRa.endPacket(true);  // Blocking send

  Serial.println("✅ LoRa radio ready!");
}

void setupPS5() {
  removePairedDevices();  // 🧹 Clear previous pairings

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

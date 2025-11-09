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
  // setupSD();      // 💾
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

void setupSD() {
  // 💾 SD Card Setup (Currently disabled)
  // SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // if (!SD.begin(SD_CS)) {
  //   Serial.println("❌ Card Mount Failed");
  //   return;
  // }

  // uint8_t cardType = SD.cardType();

  // if (cardType == CARD_NONE) {
  //   Serial.println("💾❌ No SD card attached");
  //   return;
  // }

  // Serial.print("💾 SD Card Type: ");
  // if (cardType == CARD_MMC)
  //   Serial.println("MMC");
  // else if (cardType == CARD_SD)
  //   Serial.println("SDSC");
  // else if (cardType == CARD_SDHC)
  //   Serial.println("SDHC");
  // else
  //   Serial.println("UNKNOWN");

  // uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  // Serial.printf("💾 SD Card Size: %lluMB\n", cardSize);

  // listDir(SD, "/", 0);
  // createDir(SD, "/mydir");
  // listDir(SD, "/", 0);
  // removeDir(SD, "/mydir");
  // listDir(SD, "/", 2);
  // writeFile(SD, "/hello.txt", "Hello ");
  // appendFile(SD, "/hello.txt", "World!\n");
  // readFile(SD, "/hello.txt");
  // deleteFile(SD, "/foo.txt");
  // renameFile(SD, "/hello.txt", "/foo.txt");
  // readFile(SD, "/foo.txt");
  // testFileIO(SD, "/test.txt");
  // Serial.printf("💾 Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  // Serial.printf("💾 Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void setupRadio() {
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);  // 📡 Configure LoRa pins

  while (!LoRa.begin(frequency)) {
    Serial.println("❌ LoRa init failed. Check your connections.");
    delay(200);
  }

  Serial.println("✅ LoRa init succeeded.");
  Serial.println();
  Serial.println("📡 LoRa Simple Node");
  Serial.println("📥 Only receive messages from gateways");
  Serial.println("📤 Tx: invertIQ disable");
  Serial.println("📥 Rx: invertIQ enable");
  Serial.println();

  LoRa.setTxPower(20, PA_OUTPUT_PA_BOOST_PIN);  // 📡 Max power

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_txMode();
  LoRa_rxMode();

  // Send LoRa packet to receiver 🚀
  LoRa.beginPacket();
  LoRa.print("🎯 Setup is Completed!");
  LoRa.endPacket();
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

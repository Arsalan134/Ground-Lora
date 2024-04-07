#include "main.h"

// Display
SSD1306Wire ui(0x3c, SDA, SCL);
OLEDDisplayUi display(&ui);

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback overlays[] = {msOverlay};

// This array keeps function pointers to all frames
// frames are the single views that slide in
FrameCallback frames[] = {drawFrame1, drawFrame2, drawFrame3, drawFrame4, drawFrame5};

void setup() {
  Serial.begin(115200);

  pinMode(BUILTIN_LED, OUTPUT);

  // pins_arduino.h
  setupDisplay();
  // setupSD();
  setupPS5();
}

void loop() {
  int remainingTimeBudget = display.update();

  if (remainingTimeBudget > 0) {
    // You can do some work here
    // Don't do stuff if you are below your time budget.

    delay(remainingTimeBudget);
  }
}

void setupSD() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  if (!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC)
    Serial.println("MMC");
  else if (cardType == CARD_SD)
    Serial.println("SDSC");
  else if (cardType == CARD_SDHC)
    Serial.println("SDHC");
  else
    Serial.println("UNKNOWN");

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  listDir(SD, "/", 0);
  createDir(SD, "/mydir");
  listDir(SD, "/", 0);
  removeDir(SD, "/mydir");
  listDir(SD, "/", 2);
  writeFile(SD, "/hello.txt", "Hello ");
  appendFile(SD, "/hello.txt", "World!\n");
  readFile(SD, "/hello.txt");
  deleteFile(SD, "/foo.txt");
  renameFile(SD, "/hello.txt", "/foo.txt");
  readFile(SD, "/foo.txt");
  testFileIO(SD, "/test.txt");
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void setupPS5() {
  ps5.attach(notify);
  ps5.attachOnConnect(onConnect);
  ps5.attachOnDisconnect(onDisconnect);
  // removePairedDevices();
  ps5.begin("ac:36:1b:41:ac:ed");  // MAC addresss of Joystick
  // PS4.begin();
  Serial.print("This device MAC is: ");
  // printDeviceAddress();
}

// The ESP is capable of rendering 60fps in 80Mhz mode
// but that won't give you much time for anything else
// run it in 160Mhz mode or just set it to 30 fps
void setupDisplay() {
  display.setTargetFPS(60);

  // Customize the active and inactive symbol
  display.setActiveSymbol(activeSymbol);
  display.setInactiveSymbol(inactiveSymbol);

  // You can change this to
  // TOP, LEFT, BOTTOM, RIGHT
  display.setIndicatorPosition(BOTTOM);

  // Defines where the first frame is located in the bar.
  display.setIndicatorDirection(LEFT_RIGHT);

  // You can change the transition that is used
  // SLIDE_LEFT, SLIDE_RIGHT, SLIDE_UP, SLIDE_DOWN
  display.setFrameAnimation(SLIDE_LEFT);

  // Add frames
  display.setFrames(frames, frameCount);
  display.disableAutoTransition();

  // Add overlays
  display.setOverlays(overlays, overlaysCount);

  // Initialising the UI will init the display too.
  ui.init();

  ui.flipScreenVertically();

  ui.setTextAlignment(TEXT_ALIGN_LEFT);
  ui.setFont(ArialMT_Plain_10);
}

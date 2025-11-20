#include "Display.h"
#include "PS5Joystick.h"
#include "common.h"

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback allOverlays[] = {/*wifiOverlay,*/ bluetoothOverlay, batteryOverlay, chargingOverlay};

// draw an xbm image.
// Please note that everything that should be transitioned
// needs to be drawn relative to x and y 🖼️
void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  // if (ps5.isConnected()) {  // 🎮 PS5 controller connected
  //   display->drawString(0 + x, 20 + y, String(ps5.L2Value()));
  //   display->drawString(25 + x, 20 + y, String(ps5.R2Value()));
  // }

  display->drawString(0 + x, 10 + y, String(sendingAileronMessage));     // ↔️ Aileron
  display->drawString(25 + x, 10 + y, String(sendingElevatorsMessage));  // ↕️ Elevator
  display->drawString(50 + x, 10 + y, String(sendingRudderMessage));     // ↔️ Rudder

  display->drawString(75 + x, 10 + y, String(isEmergencyStopEnabled ? "🚨STOP" : ""));

  // Slider 🎚️
  display->drawString(50 + x, 20 + y, String(map(sendingEngineMessage, 0, 4095, 0, 100)) + "% 🚀Engine");

  display->drawString(50 + x, 30 + y, String(sendingFlapsMessage));  // 🪶 Flaps

  display->drawXbm(x + 0, y + 36, ps5Icon::xres, ps5Icon::yres, ps5Icon::pixels);

  display->drawString(50 + x, 52 + y, "👨‍💻 Arsalan Iravani");  // 👨‍💻 Developer signature
}

// Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
// Besides the default fonts there will be a program to convert TrueType fonts into this format 🔤
void drawFrame2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "📝 Arial 10");

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "📝 Arial 16");

  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 34 + y, "📝 Arial 24");
}

void drawFrame3(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo ⬅️➡️
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text ⬅️
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 11 + y, "⬅️ Left aligned (0,10)");

  // The coordinates define the center of the text ↔️
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "↔️ Center aligned (64,22)");

  // The coordinates define the right end of the text ➡️
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 33 + y, "➡️ Right aligned (128,33)");
}

// Demo for drawStringMaxWidth:
// with the third parameter you can define the width after which words will be wrapped. 📏
// Currently only spaces and "-" are allowed for wrapping
void drawFrame4(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128, "📝 Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, ");
}

void drawFrame5(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {}

// void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
//   display->setTextAlignment(TEXT_ALIGN_RIGHT);
//   display->setFont(ArialMT_Plain_10);
//   display->drawString(128, 0, String(millis()));

//   display->setTextAlignment(TEXT_ALIGN_LEFT);
//   display->drawXbm(0, 0, bluetoothIcon::xres, bluetoothIcon::yres, bluetoothIcon::pixels);
// }

void bluetoothOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(0, 0, bluetoothIcon::xres, bluetoothIcon::yres, bluetoothIcon::pixels);  // 🔵 Bluetooth icon
}

void batteryOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(120, 0, String(batteryPercentage) + "% 🔋");  // 🔋 Battery percentage
}

void chargingOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->drawXbm(85, 0, batteryChargingIcon::xres, batteryChargingIcon::yres, batteryChargingIcon::pixels);  // ⚡ Charging icon
}

void wifiOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawXbm(10, 0, wifiIcon::xres, wifiIcon::yres, wifiIcon::pixels);  // 📶 WiFi icon
}

void salam() {
  // if (ps5.isConnected())
  //   display.setOverlays(all, 2);
  // else
  //   display.setOverlays(wifiOverlays, 1);
}
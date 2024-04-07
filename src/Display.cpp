#include "Display.h"
#include "PS5Joystick.h"

// void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//   // draw an xbm image.
//   // Please note that everything that should be transitioned
//   // needs to be drawn relative to x and y

//   display->drawXbm(x + 34, y + 14, valorantlogo::xres, valorantlogo::yres, valorantlogo::pixels);
// }

void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  // display->printf("Salam L: %d  R: %d\n", 1, 2);

  if (ps5.isConnected()) {
    display->drawString(0 + x, 0, String(ps5.LStickX()));
    display->drawString(25 + x, 0, String(ps5.RStickY()));

    display->drawString(0 + x, 10, String(ps5.L2Value()));
    display->drawString(25 + x, 10, String(ps5.R2Value()));
  }

  display->drawXbm(x + 0, y + 26, ps5Icon::xres, ps5Icon::yres, ps5Icon::pixels);
}

// Demonstrates the 3 included default sizes. The fonts come from SSD1306Fonts.h file
// Besides the default fonts there will be a program to convert TrueType fonts into this format
void drawFrame2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "Arial 10");

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "Arial 16");

  display->setFont(ArialMT_Plain_24);
  display->drawString(0 + x, 34 + y, "Arial 24");
}

void drawFrame3(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  // Text alignment demo
  display->setFont(ArialMT_Plain_10);

  // The coordinates define the left starting point of the text
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->drawString(0 + x, 11 + y, "Left aligned (0,10)");

  // The coordinates define the center of the text
  display->setTextAlignment(TEXT_ALIGN_CENTER);
  display->drawString(64 + x, 22 + y, "Center aligned (64,22)");

  // The coordinates define the right end of the text
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 33 + y, "Right aligned (128,33)");
}

// Demo for drawStringMaxWidth:
// with the third parameter you can define the width after which words will be wrapped.
// Currently only spaces and "-" are allowed for wrapping
void drawFrame4(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawStringMaxWidth(0 + x, 10 + y, 128,
                              "Lorem ipsum\n dolor sit amet, consetetur sadipscing elitr, ");
}

void drawFrame5(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {}

void msOverlay(OLEDDisplay* display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}
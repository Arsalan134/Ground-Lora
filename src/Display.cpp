#include "Display.h"
#include "PS5Joystick.h"
#include "common.h"
#include "protocol.h"

#ifdef PROTO_BIDIRECTIONAL
// Telemetry externs
extern float tlm_altitude;
extern float tlm_pressure;
extern int tlm_rssi;
extern float tlm_gforce;
extern float tlm_temperature;
extern float tlm_verticalSpeed;
extern bool tlm_valid;
extern unsigned long tlm_lastReceived;
#endif

// Overlays are statically drawn on top of a frame eg. a clock
OverlayCallback allOverlays[] = {/*wifiOverlay,*/ bluetoothOverlay, batteryOverlay, chargingOverlay};

// 🖼️ Frame 1: Primary Flight HUD
void drawFrame1(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  char buf[24];

  // ── Row 1 (y=10): Control surfaces + Arm/Stop pill ──
  snprintf(buf, sizeof(buf), "A:%d", sendingAileronMessage);
  display->drawString(0 + x, 10 + y, buf);
  snprintf(buf, sizeof(buf), "E:%d", sendingElevatorsMessage);
  display->drawString(34 + x, 10 + y, buf);
  snprintf(buf, sizeof(buf), "R:%d", sendingRudderMessage);
  display->drawString(68 + x, 10 + y, buf);
  drawPill(display, 98 + x, 10 + y, isEmergencyStopEnabled ? "STOP" : "ARM", true);

  // ── Row 2 (y=21): Engine throttle progress bar + Flaps ──
  int enginePct = map(sendingEngineMessage, 0, PROTO_ENGINE_RAW_MAX, 0, 100);
  display->drawProgressBar(0 + x, 23 + y, 76, 8, enginePct);
  snprintf(buf, sizeof(buf), "%d%%", enginePct);
  display->drawString(80 + x, 21 + y, buf);
  snprintf(buf, sizeof(buf), "F%d", sendingFlapsMessage);
  display->drawString(106 + x, 21 + y, buf);

  // ── Row 3 (y=32): Status indicator pills ──
  int16_t px = 0;
  drawPill(display, px + x, 32 + y, "ABRK", airbrakeEnabled);
  px += display->getStringWidth("ABRK") + 7;
  drawPill(display, px + x, 32 + y, "ACS", acsEngageEnabled);
  px += display->getStringWidth("ACS") + 7;
  int stabPct = map(stabilityAssistValue, 0, 255, 0, 100);
  snprintf(buf, sizeof(buf), "SA:%d%%", stabPct);
  display->drawString(px + x, 32 + y, buf);

  // ── Row 4 (y=43): Telemetry summary or Expo info ──
#ifdef PROTO_BIDIRECTIONAL
  if (tlm_valid && (millis() - tlm_lastReceived < 3000)) {
    snprintf(buf, sizeof(buf), "Alt:%.1fm", (double)tlm_altitude);
    display->drawString(0 + x, 43 + y, buf);
    snprintf(buf, sizeof(buf), "G:%.1f", (double)tlm_gforce);
    display->drawString(62 + x, 43 + y, buf);
    snprintf(buf, sizeof(buf), "%ddB", tlm_rssi);
    display->drawString(92 + x, 43 + y, buf);
  } else
#endif
  {
    char expBuf[26];
    snprintf(expBuf, sizeof(expBuf), "%s .%02d/.%02d/.%02d", expoPresetNames[expoPresetIndex],
             (int)(expoAileron * 100), (int)(expoElevator * 100), (int)(expoRudder * 100));
    display->drawString(0 + x, 43 + y, expBuf);
  }

  // ── Row 5 (y=53): Flight timer + telemetry extras ──
  if (flightTimerRunning && flightTimerStartMs > 0) {
    unsigned long elapsed = (millis() - flightTimerStartMs) / 1000;
    char timeBuf[10];
    snprintf(timeBuf, sizeof(timeBuf), "%02d:%02d", (int)(elapsed / 60), (int)(elapsed % 60));
    display->drawString(0 + x, 53 + y, timeBuf);
  } else {
    display->drawString(0 + x, 53 + y, "--:--");
  }
#ifdef PROTO_BIDIRECTIONAL
  if (tlm_valid && (millis() - tlm_lastReceived < 3000)) {
    snprintf(buf, sizeof(buf), "%.1fC", (double)tlm_temperature);
    display->drawString(38 + x, 53 + y, buf);
  }
#endif

  // ⏱️ Uptime timer (always running from boot)
  unsigned long uptimeSeconds = millis() / 1000UL;
  unsigned long uptimeMinutes = uptimeSeconds / 60UL;
  unsigned long uptimeRemainderSeconds = uptimeSeconds % 60UL;
  char uptimeBuf[12];
  snprintf(uptimeBuf, sizeof(uptimeBuf), "%lu:%02lu", uptimeMinutes, uptimeRemainderSeconds);

  // 🌿 ECO mode indicator + uptime — bottom right
  if (ecoModeEnabled) {
    int16_t ecoWidth = display->getStringWidth("ECO") + 4;
    int16_t ecoX = 128 + x - ecoWidth;
    drawPill(display, ecoX, 53 + y, "ECO", true);
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(ecoX - 3, 53 + y, uptimeBuf);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
  } else {
    display->setTextAlignment(TEXT_ALIGN_RIGHT);
    display->drawString(128 + x, 53 + y, uptimeBuf);
    display->setTextAlignment(TEXT_ALIGN_LEFT);
  }
}

// 🖼️ Frame 2: Expo/Rate Config + Extended Telemetry
void drawFrame2(OLEDDisplay* display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);

  // ── Row 1 (y=10): Expo values (A/E/R) ──
  char buf[30];
  snprintf(buf, sizeof(buf), "%s .%02d/.%02d/.%02d", expoPresetNames[expoPresetIndex],
           (int)(expoAileron * 100), (int)(expoElevator * 100), (int)(expoRudder * 100));
  display->drawString(0 + x, 10 + y, buf);

  // ── Row 2 (y=21): Rate values (A/E/R) ──
  snprintf(buf, sizeof(buf), "Rate %.1f/%.1f/%.1f",
           (double)RATE_AILERON, (double)RATE_ELEVATOR, (double)RATE_RUDDER);
  display->drawString(0 + x, 21 + y, buf);

  // ── Row 3-4 (y=32, y=43): Extended telemetry or radio config ──
#ifdef PROTO_BIDIRECTIONAL
  if (tlm_valid && (millis() - tlm_lastReceived < 3000)) {
    snprintf(buf, sizeof(buf), "P:%.1fhPa", (double)tlm_pressure);
    display->drawString(0 + x, 32 + y, buf);
    snprintf(buf, sizeof(buf), "%.1fC", (double)tlm_temperature);
    display->drawString(76 + x, 32 + y, buf);
    snprintf(buf, sizeof(buf), "VS:%.1fm/s", (double)tlm_verticalSpeed);
    display->drawString(0 + x, 43 + y, buf);
    snprintf(buf, sizeof(buf), "RSSI:%d", tlm_rssi);
    display->drawString(64 + x, 43 + y, buf);
  } else
#endif
  {
    char radioBuf[30];
    snprintf(radioBuf, sizeof(radioBuf), "LoRa %ldMHz SF%d BW%ldk",
             PROTO_LORA_FREQUENCY_HZ / 1000000L, PROTO_LORA_SF,
             PROTO_LORA_BANDWIDTH_HZ / 1000L);
    display->drawString(0 + x, 32 + y, radioBuf);
    snprintf(radioBuf, sizeof(radioBuf), "TX:%ddBm CR:4/%d Sync:0x%02X",
             PROTO_LORA_TX_POWER, PROTO_LORA_CR, PROTO_LORA_SYNC_WORD);
    display->drawString(0 + x, 43 + y, radioBuf);
  }

  // ── Row 5 (y=53): Name ──
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->drawString(128 + x, 53 + y, "Arsalan Iravani");
  display->setTextAlignment(TEXT_ALIGN_LEFT);
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

// Helper: draw an inverted "pill" label (filled when active, outlined when inactive)
static void drawPill(OLEDDisplay* display, int16_t x, int16_t y, const char* text, bool active) {
  int16_t w = display->getStringWidth(text) + 4;
  int16_t h = 11;
  if (active) {
    display->setColor(WHITE);
    display->fillRect(x, y, w, h);
    display->setColor(BLACK);
    display->drawString(x + 2, y, text);
    display->setColor(WHITE);
  } else {
    display->drawRect(x, y, w, h);
    display->drawString(x + 2, y, text);
  }
}
#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

// pins
#define sliderPin 34

// PS5
#define MAC_ADDRESS "ac:36:1b:41:ac:ed"

extern OLEDDisplayUi display;

// Overlays are statically drawn on top of a frame eg. a clock
// OverlayCallback overlays[] = {msOverlay};
extern OverlayCallback all[];
extern OverlayCallback wifiOverlays[];
extern OverlayCallback bluetoothOverlays[];

extern int sendingEngineMessage;
extern byte sendingAileronMessage;
extern byte sendingRudderMessage;
extern byte sendingElevatorsMessage;
extern int sendingTrimMessage;

extern bool isEmergencyStopEnabled;

#define idleDeviationThreshold 10
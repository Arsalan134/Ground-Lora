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
extern int sendingAileronMessage;
extern int sendingRudderMessage;
extern int sendingElevatorsMessage;

#define idleDeviationThreshold 10
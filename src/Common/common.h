#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

extern OLEDDisplayUi display;

// Overlays are statically drawn on top of a frame eg. a clock
// OverlayCallback overlays[] = {msOverlay};
extern OverlayCallback all[];
extern OverlayCallback wifiOverlays[];
extern OverlayCallback bluetoothOverlays[];
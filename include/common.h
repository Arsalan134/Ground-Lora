#include "OLEDDisplayUi.h"
#include "SSD1306Wire.h"

// pins
#define sliderPin 34

// PS5
#define PS5_MAC_ADDRESS "ac:36:1b:41:ac:ed"

extern OLEDDisplayUi display;

extern OverlayCallback allOverlays[];

extern int sendingEngineMessage;
extern byte sendingAileronMessage;
extern byte sendingRudderMessage;
extern byte sendingElevatorsMessage;
extern int sendingElevatorTrimMessage;
extern int sendingAileronTrimMessage;
extern int sendingFlapsMessage;
extern bool resetAileronTrim;
extern bool resetElevatorTrim;

extern bool isEmergencyStopEnabled;
extern bool airbrakeEnabled;

extern uint8_t batteryPercentage;

#define idleDeviationThreshold 10
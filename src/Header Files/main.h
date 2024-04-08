#include <Arduino.h>
#include "Common\common.h"

// PS5
#include <ps5Controller.h>

void setupPS5();
void setupSD();
void setupDisplay();
void removePairedDevices();
void printDeviceAddress();
void onConnect();
void notify();
void onDisconnect();

// SD Card
#include "SD-Card.h"

// Display
#include "Display.h"
int frameCount = 1;
int overlaysCount = 1;

void testdrawline();       // Draw many lines
void testdrawrect();       // Draw rectangles (outlines)
void testfillrect();       // Draw rectangles (filled)
void testdrawcircle();     // Draw circles (outlines)
void testfillcircle();     // Draw circles (filled)
void testdrawroundrect();  // Draw rounded rectangles (outlines)
void testfillroundrect();  // Draw rounded rectangles (filled)
void testdrawtriangle();   // Draw triangles (outlines)
void testfilltriangle();   // Draw triangles (filled)
void testdrawchar();       // Draw characters of the default font
void testdrawstyles();     // Draw 'stylized' characters
void testscrolltext();     // Draw scrolling text
void testdrawbitmap();     // Draw a small bitmap image
void testanimate(const uint8_t* bitmap, uint8_t w, uint8_t h);

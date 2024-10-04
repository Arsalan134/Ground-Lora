#include <Arduino.h>
#include "Common\common.h"

// PS5
#include <ps5Controller.h>
void setupPS5();
void onConnect();
void notify();
void onDisconnect();
void removePairedDevices();
void printDeviceAddress();

// SD Card
#include "SD-Card.h"
void setupSD();

// Display
#include "Display.h"
int frameCount = 1;
int overlaysCount = 1;
void setupDisplay();

// Lora
#include <LoRa.h>
const long frequency = 915E6;  // LoRa Frequency
boolean runEvery(unsigned long interval);
void setupRadio();
void loraLoop();
void LoRa_rxMode();
void LoRa_txMode();
void LoRa_sendMessage(String message);
void onReceive(int packetSize);
void onTxDone();

void readSlider();

// void testdrawline();       // Draw many lines
// void testdrawrect();       // Draw rectangles (outlines)
// void testfillrect();       // Draw rectangles (filled)
// void testdrawcircle();     // Draw circles (outlines)
// void testfillcircle();     // Draw circles (filled)
// void testdrawroundrect();  // Draw rounded rectangles (outlines)
// void testfillroundrect();  // Draw rounded rectangles (filled)
// void testdrawtriangle();   // Draw triangles (outlines)
// void testfilltriangle();   // Draw triangles (filled)
// void testdrawchar();       // Draw characters of the default font
// void testdrawstyles();     // Draw 'stylized' characters
// void testscrolltext();     // Draw scrolling text
// void testdrawbitmap();     // Draw a small bitmap image
// void testanimate(const uint8_t* bitmap, uint8_t w, uint8_t h);

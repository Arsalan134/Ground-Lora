#include <Arduino.h>
#include "common.h"

// PS5 Controller 🎮
#include <ps5Controller.h>
void setupPS5();             // 🎮 Initialize PS5 controller
void onConnect();            // ✅ PS5 connect callback
void notify();               // 📡 PS5 input handler
void onDisconnect();         // ❌ PS5 disconnect callback
void removePairedDevices();  // 🧹 Clear Bluetooth pairings
void printDeviceAddress();   // 📱 Print device MAC

// SD Card 💾
#include "SD-Card.h"
void setupSD();  // 💾 Initialize SD card (currently disabled)

// Display 🖥️
#include "Display.h"
int frameCount = 1;     // 🖼️ Number of display frames
int overlaysCount = 1;  // 📱 Number of display overlays
void setupDisplay();    // 🖥️ Initialize OLED display

// LoRa Communication 📡
#include <LoRa.h>
const long frequency = 915E6;                              // 📡 LoRa Frequency (915MHz)
boolean runEvery(unsigned long interval);                  // ⏰ Timer function
void setupRadio();                                         // 📡 Initialize LoRa radio
void loraLoop();                                           // 📡 Main LoRa communication loop
uint8_t simple_checksum(const uint8_t* data, size_t len);  // 🔐 Checksum calculator
void LoRa_rxMode();                                        // 📥 Set LoRa receive mode
void LoRa_txMode();                                        // 📤 Set LoRa transmit mode
void LoRa_sendMessage(String message);                     // 📡 Send LoRa message
void onReceive(int packetSize);                            // 📥 LoRa receive callback
void onTxDone();                                           // ✅ LoRa transmit complete callback

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

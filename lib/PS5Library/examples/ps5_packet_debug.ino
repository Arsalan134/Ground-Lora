#include <ps5Controller.h>

// This utility helps determine the correct packet indices for PS5 controller data
// Use this to identify where sensor data is located in the HID packet

void setup() {
  Serial.begin(115200);
  ps5.begin("ac:36:1b:41:ac:ed"); //replace with your controller's Bluetooth MAC address
  
  Serial.println("PS5 Controller Packet Debug Utility");
  Serial.println("This tool displays raw packet data to help identify sensor locations.");
  Serial.println("Move the controller, press buttons, and observe which bytes change.");
  Serial.println("");
}

void printPacketHex(uint8_t* packet, int length) {
  Serial.print("Raw packet [");
  Serial.print(length);
  Serial.print("]: ");
  
  for (int i = 0; i < length && i < 64; i++) { // Limit to first 64 bytes for readability
    if (packet[i] < 0x10) Serial.print("0");
    Serial.print(packet[i], HEX);
    Serial.print(" ");
    
    if ((i + 1) % 16 == 0) {
      Serial.println("");
      Serial.print("       ");
    }
  }
  Serial.println("");
}

void printPacketAnalysis(uint8_t* packet) {
  if (!packet) return;
  
  Serial.println("Packet Analysis:");
  
  // Show known working indices
  Serial.printf("  Analog sticks: LX=%d LY=%d RX=%d RY=%d\n", 
                ps5.LStickX(), ps5.LStickY(), ps5.RStickX(), ps5.RStickY());
  
  Serial.printf("  Buttons: Cross=%d Circle=%d Square=%d Triangle=%d\n",
                ps5.Cross(), ps5.Circle(), ps5.Square(), ps5.Triangle());
  
  Serial.printf("  Status: Battery=%d%% Charging=%d\n", 
                ps5.Battery() * 10, ps5.Charging());
  
  // Show current sensor readings (may be incorrect due to wrong indices)
  Serial.printf("  Sensors (may be wrong): AccelX=%d AccelY=%d AccelZ=%d\n",
                ps5.AccelX(), ps5.AccelY(), ps5.AccelZ());
  Serial.printf("                          GyroX=%d GyroY=%d GyroZ=%d\n",
                ps5.GyroX(), ps5.GyroY(), ps5.GyroZ());
  
  // Show raw bytes at various indices to help identify sensor locations
  Serial.println("  Raw bytes at key indices:");
  for (int i = 0; i < 64; i += 2) {
    int16_t value = (int16_t)((packet[i+1] << 8) | packet[i]);
    Serial.printf("    [%2d:%2d] = %6d (0x%04X)\n", i, i+1, value, value & 0xFFFF);
    
    if (i >= 30) break; // Don't spam too much
  }
  Serial.println("");
}

void loop() {
  if (ps5.isConnected()) {
    static unsigned long lastOutput = 0;
    static bool detailMode = false;
    static bool prevTriangle = false;
    
    // Toggle detail mode with Triangle button
    if (ps5.Triangle() && !prevTriangle) {
      detailMode = !detailMode;
      Serial.printf("Detail mode: %s\n", detailMode ? "ON" : "OFF");
      Serial.println("");
    }
    prevTriangle = ps5.Triangle();
    
    // Output packet data every 500ms in normal mode, 100ms in detail mode
    unsigned long interval = detailMode ? 100 : 500;
    
    if (millis() - lastOutput > interval) {
      uint8_t* packet = ps5.LatestPacket();
      
      if (packet) {
        if (detailMode) {
          printPacketHex(packet, 64);
          printPacketAnalysis(packet);
        } else {
          // Just show sensor values and some key bytes
          Serial.printf("Sensors: A(%6d,%6d,%6d) G(%6d,%6d,%6d) | Buttons: %s%s%s%s\n",
                        ps5.AccelX(), ps5.AccelY(), ps5.AccelZ(),
                        ps5.GyroX(), ps5.GyroY(), ps5.GyroZ(),
                        ps5.Cross() ? "X" : "-",
                        ps5.Circle() ? "O" : "-",
                        ps5.Square() ? "□" : "-",
                        ps5.Triangle() ? "△" : "-");
        }
      } else {
        Serial.println("No packet data available");
      }
      
      lastOutput = millis();
    }
    
    // Provide LED feedback
    if (detailMode) {
      ps5.setLed(255, 255, 0); // Yellow in detail mode
    } else {
      ps5.setLed(0, 255, 0); // Green in normal mode
    }
    ps5.sendToController();
    
  } else {
    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 3000) {
      Serial.println("PS5 Controller not connected...");
      lastMsg = millis();
    }
  }
  
  delay(10);
}

// Instructions that will be printed
void printInstructions() {
  Serial.println("INSTRUCTIONS:");
  Serial.println("1. Connect your PS5 controller");
  Serial.println("2. Press Triangle to toggle detailed packet view");
  Serial.println("3. Move the controller and observe which bytes change");
  Serial.println("4. Press buttons and note the pattern");
  Serial.println("5. Use this data to correct sensor packet indices in ps5_parser.c");
  Serial.println("");
}

#include <LoRa.h>
#include "Common\common.h"

int sendingEngineMessage = 1;
byte sendingAileronMessage = 127;
byte sendingRudderMessage = 127;
byte sendingElevatorsMessage = 127;
int sendingElevatorTrimMessage = 0;
int sendingAileronTrimMessage = 0;
int sendingFlapsMessage = 0;  // 0, 1, 2, 3, 4
bool resetAileronTrim = false;
bool resetElevatorTrim = false;

void LoRa_rxMode() {
  LoRa.enableInvertIQ();  // active invert I and Q signals
  LoRa.receive();         // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();             // set standby mode
  LoRa.disableInvertIQ();  // normal mode
}

void LoRa_sendMessage(String message) {
  digitalWrite(BUILTIN_LED, 1);
  // LoRa_txMode();         // set tx mode
  LoRa.beginPacket();    // start packet
  LoRa.print(message);   // add payload
  LoRa.endPacket(true);  // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }
}

void onTxDone() {
  // Serial.println("TxDone");
  digitalWrite(BUILTIN_LED, 0);
  // LoRa_rxMode();
}

boolean runEvery(unsigned long interval) {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    return true;
  }

  return false;
}

String message = "";
byte previousChecksum = 0;

int samePacketCount = 0;

byte simple_checksum(const byte* data, size_t len) {
  byte sum = 0;
  for (size_t i = 0; i < len; i++) {
    sum ^= data[i];
  }
  return sum;
}

void loraLoop() {
  if (runEvery(60)) {
    message = "e" + String(isEmergencyStopEnabled ? 0 : map(sendingEngineMessage, 0, 4095, 0, 180));  // "e" is used for engine
    message += "a" + String(map(sendingAileronMessage, 0, 255, 0, 180));                              // "a" is used for ailerons
    message += "r" + String(map(sendingRudderMessage, 0, 255, 0, 180));                               // "r" is used for rudder
    message += "l" + String(map(sendingElevatorsMessage, 0, 255, 0, 180));                            // "l" is used for elevators
    message += "t" + String(sendingElevatorTrimMessage);                                              // "t" is used for trim
    message += "i" + String(sendingAileronTrimMessage);                                               // "i" is used for aileron trim
    message += "f" + String(sendingFlapsMessage);                                                     // "f" is used for flaps
    message += "z" + String(resetAileronTrim ? 1 : 0);                                                // "z" is used for reset aileron trim
    message += "y" + String(resetElevatorTrim ? 1 : 0);                                               // "y" is used for reset elevator trim

    sendingElevatorTrimMessage = 0;
    sendingAileronTrimMessage = 0;
    resetAileronTrim = false;
    resetElevatorTrim = false;

    byte checksum = simple_checksum((const byte*)message.c_str(), message.length());

    message += "#";
    message += checksum;

    int aileronDeviation = abs(sendingAileronMessage - 127);
    int rudderDeviation = abs(sendingRudderMessage - 127);
    int elevatorsDeviation = abs(sendingElevatorsMessage - 127);

    int totalDeviation = aileronDeviation + rudderDeviation + elevatorsDeviation;

    // Skip sending if the same packet is sent multiple times
    if (checksum == previousChecksum && samePacketCount >= 10 &&
        totalDeviation < idleDeviationThreshold) {  // only if joysticks are in neutral position
      return;
    }

    LoRa_sendMessage(message);  // send a message

    Serial.println("LoRa Send: " + message);

    if (checksum == previousChecksum)
      samePacketCount++;
    else
      samePacketCount = 0;

    previousChecksum = checksum;
  }
}
#include <LoRa.h>
#include "Common\common.h"

void LoRa_rxMode() {
  LoRa.enableInvertIQ();  // active invert I and Q signals
  LoRa.receive();         // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();             // set standby mode
  LoRa.disableInvertIQ();  // normal mode
}

void LoRa_sendMessage(String message) {
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

  // Serial.print("Node Receive: ");
  // Serial.println(message);
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

/*
  engine
  aileron
  rudder
  elevators
  String

  min 0. max 180

  e0 a0  r0 l0
  e0a90r89l91 example payload sending
*/

String message = "";

void loraLoop() {
  if (runEvery(100)) {
    digitalWrite(BUILTIN_LED, 1);
    // delay(5);

    message = "e" + String(map(sendingEngineMessage, 0, 4095, 0, 180));  // "e" is used for engine
    message +=
        "a" + String(map(sendingAileronMessage, 0, 255, 0, 180));        // "a" is used for ailerons
    message += "r" + String(map(sendingRudderMessage, 0, 255, 0, 180));  // "r" is used for rudder
    message +=
        "l" + String(map(sendingElevatorsMessage, 0, 255, 0, 180));  // "l" is used for elevators
    LoRa_sendMessage(message);                                       // send a message

    Serial.println(message);
    // Serial.println("LORA LOOP");
  }
}

// void radioConnection() {
//   transmitData[throttleIndex] =
//       emergencyStopIsActive ? 0 : max(map(L2Value, 0, 255, 0, 90), map(R2Value, 0, 255, 0, 180));

//   trim = constrain(trim, 90 - 45, 90 + 45);

//   transmitData[trimIndex] = trim;

//   radio.stopListening();

//   if (radio.write(&transmitData, sizeof(transmitData)))
//     radio.startListening();
//   else
//     Serial.println("Failed to transmit !");

//   while (radio.available()) {
//     radio.read(&recievedData, sizeof(recievedData));
//     lastRecievedTime = millis();
//   }

//   setLEDColor();

//   // Serial.print("Elapsed: ");
//   // Serial.println(millis() - lastRecievedTime);
// }

// void reset() {
//   transmitData[rollIndex] = 90;
//   transmitData[pitchIndex] = 90;
//   transmitData[yawIndex] = 90;

//   transmitData[autopilotIsOnIndex] = false;
// }

// #define sliderPin A0

// #define rollIndex 0
// #define pitchIndex 1
// #define yawIndex 2
// #define throttleIndex 3
// #define autopilotIsOnIndex 4
// #define trimIndex 5

// #define batteryLevelIndex 0

// byte transmitData[6];
// byte recievedData[1];

// unsigned long lastRecievedTime = millis();

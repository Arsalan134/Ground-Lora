#include <ps5Controller.h>
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_err.h"
#include "esp_gap_bt_api.h"

#define EVENTS 0
#define BUTTONS 0
#define JOYSTICKS 0
#define SENSORS 0

void onDisconnect();
void removePairedDevices();
void printDeviceAddress();
void onConnect();
void notify();
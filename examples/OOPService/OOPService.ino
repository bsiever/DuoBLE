
/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#include <DuoBLE.h>
#include"BLETimerService.h"

const char *shortDeviceName = "Time";

// Disable WiFi (for now)
SYSTEM_MODE(MANUAL);

// With object's it is easy to include multiple instances of a service
BLETimerService timer1;
BLETimerService timer2;

void setup() {
  delay(2000);
  Serial.begin(9600);

  DuoBLE.addService(timer1);
  DuoBLE.addService(timer2);

  // Start stack
  DuoBLE.begin();

  // The Advertised name and "local" name should have some agreement
  DuoBLE.advertisingDataAddName(ADVERTISEMENT, shortDeviceName);
  DuoBLE.setName(shortDeviceName);

  // Start advertising.
  DuoBLE.startAdvertising();

  Serial.println("Advertising");
}

void loop() {
}

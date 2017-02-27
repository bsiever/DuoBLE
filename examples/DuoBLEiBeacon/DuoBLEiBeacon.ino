
/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

 #include <DuoBLE.h>

// Disable WiFi (for now)
SYSTEM_MODE(MANUAL);

#define APPLE_MANUFACTURER_ID 0x004C

// UUID: 11cd2933-9ea7-461a-b0e7-a99b8401881b (from https://www.uuidgenerator.net/)

UUID beaconID("11cd2933-9ea7-461a-b0e7-a99b8401881b");

void setup() {
  RGB.control(true);
  RGB.color(0, 0, 0);

  Serial.begin(9600);
  DuoBLE.begin();

  // Setup Advertising: Rates and "Non-connectable" (for beacon)
  DuoBLE.setAdvertisementParameters(MS(200), MS(500), BLE_GAP_ADV_TYPE_ADV_NONCONN_IND);

  // Setup the data
  DuoBLE.advertisingIBeacon(beaconID, 0x0102, 0x0304);

  DuoBLE.startAdvertising();
}

void loop() {
  // put your main code here, to run repeatedly:

}

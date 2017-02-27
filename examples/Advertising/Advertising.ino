/**
 * Examples of different advertising.
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

 #include <DuoBLE.h>

// Disable WiFi (for now)
SYSTEM_MODE(MANUAL);

// Test UUID: ac5eb0c9-a99f-4a23-b70d-1c927a290c1e
// (From https://www.uuidgenerator.net/)
UUID testUUID("ac5eb0c9-a99f-4a23-b70d-1c927a290c1e");


void setup() {
  Serial.begin(9600);
  // Start stack
  DuoBLE.begin();

  // Setup Advertising
  DuoBLE.setAdvertisementParameters();

  // Set BLE advertising and scan respond data

/* Test Case 1: Add service data to both ADV and SCAN_RESPONSE
  DuoBLE.advertisingDataAdd16BitServiceData(ADVERTISEMENT, 0x1234, (uint8_t *)"test", 4);
  DuoBLE.advertisingDataAdd128BitServiceData(SCAN_RESPONSE, testUUID, (uint8_t *)"test2", 5);
//*/

//* Test Case 2: Advertise services
  uint16_t uuids[] = {BLE_UUID_HEART_RATE_SERVICE};
  DuoBLE.advertisingDataAdd16bitUUIDs(ADVERTISEMENT, uuids, sizeof(uuids)/sizeof(uint16_t));
  DuoBLE.advertisingDataAdd128bitUUID(SCAN_RESPONSE, testUUID);
//*/

  DuoBLE.setName("Adv");
  DuoBLE.setAppearance(BLE_APPEARANCE_UNKNOWN);

  // Start advertising.
  DuoBLE.startAdvertising();
  Serial.println("BLE start advertising.");
}

void loop() {
}

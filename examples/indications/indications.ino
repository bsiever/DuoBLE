
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

// TODO: Modify the shortDeviceName and advServiceUUID
// Should be 4 letters or fewer
const char *shortDeviceName = "Ping";

// Define a service and its data
BLEService pingService("cadf02d7-cb77-46dc-9a29-5e6c42ba21f3");
BLECharacteristic pingCharacteristic("35603268-8474-4acb-886f-b26a1c1e9331", ATT_PROPERTY_INDICATE, 4,4);

/*
 *
 * Rather than using a specific function for each Characteristic, it's also possible to have one
 * function shared for all characteristics.  Sometimes this approach is easier
 */
void callback(BLERecipient recipient, BLECharacteristicCallbackReason reason) {
  const char *reasons[] = {"PREREAD",  "POSTREAD", "POSTWRITE", "NOTIFICATIONS_ENABLED", "NOTIFICATIONS_DISABLED",  "INDICATIONS_ENABLED", "INDICATIONS_DISABLED"};
  Serial.print("callback: ");
  Serial.println(reasons[reason]);
}



/**
 * @brief Setup.
 */
void setup() {
  delay(2000);
  Serial.begin(9600);

  // Set the data in the custom characteristics:
  byte intValue[] = {0,0,0,0};  // A 4 byte integer counter
  pingCharacteristic.setValue(intValue, 4);
  pingCharacteristic.setCallback(callback);
  pingService.addCharacteristic(pingCharacteristic);

  // Add the Service
  DuoBLE.addService(pingService);

  // Start stack
  DuoBLE.begin();

  // The Advertised name and "local" name should have some agreement
  DuoBLE.advertisingDataAddName(ADVERTISEMENT, shortDeviceName);
  DuoBLE.setName(shortDeviceName);

  // Start advertising.
  DuoBLE.startAdvertising();
}



int count = 0;
void loop() {
  // Wait 1s
  delay(1000);

  // Counts every second
  count++;

  // Update the stored data
  byte data[4];
  for(int i=0;i<4;i++)
    data[i]=count >> (24-8*i);
  pingCharacteristic.setValue(data, 4);

  // Sends a notification of the current "count" (if connected)
  pingCharacteristic.sendIndicate();

}

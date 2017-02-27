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
const char *shortDeviceName = "Test";

// Define a service and its data
BLEService simpleCustomService("208c9c6f-dcf8-4c1f-8a43-8f1674c21d6e");
BLECharacteristic countingCharacteristic("ec50d486-314c-4825-8365-2ee0e1952a4f", ATT_PROPERTY_READ, 4,4);
// Default size is 20 characters max
BLECharacteristic readWriteCharacteristic("207b67d2-46ab-4ea6-bdb6-9c12917253e3", ATT_PROPERTY_READ | ATT_PROPERTY_WRITE);


// The following two functions are optional: They identify when a device connects/disconnects
void BLE_connected() {
  Serial.println("Central Connected");
}
void BLE_disconnected() {
  Serial.println("Central Disconnected");
}


// Define a callback for each piece of data (also possible to use a single callback and compare the recipient)
void rwCharacCallback(BLERecipient recipient, BLECharacteristicCallbackReason reason) {
    Serial.print("Read/Write Characteristic; Reason: ");
    Serial.println(reason);
    if(reason == POSTWRITE) {
      byte value[20];
      int bytes = readWriteCharacteristic.getValue(value, 20);
      Serial.print("New Value written: ");
      for(int i=0;i<bytes;i++)
        Serial.print(value[i], HEX);
    }
    Serial.println();
}


void countingCharacCallback(BLERecipient recipient, BLECharacteristicCallbackReason reason) {
    Serial.println("Counting Characteristic: ");
    if(reason == PREREAD) {
       byte value[4];
       countingCharacteristic.getValue(value, 4);
       int i = value[0]<<24 | value[1] << 16 | value[2] << 8 | value[3];
       Serial.print("Old value; " );
       Serial.println(i);
       i++;
       value[0] = i>>24;
       value[1] = i>>16;
       value[2] = i>>8;
       value[3] = i;
       countingCharacteristic.setValue(value, 4);
    }
}

void setup() {
  delay(2000);
  Serial.begin(9600);

  // Set the data in the custom characteristics:
  byte intValue[] = {0,0,0,0};  // A 4 byte integer counter
  countingCharacteristic.setValue(intValue, 4);
  countingCharacteristic.setCallback(countingCharacCallback);
  simpleCustomService.addCharacteristic(countingCharacteristic);

  byte charValue[] = {'h','i'};  // A 2 byte character array (C style string)
  readWriteCharacteristic.setValue(charValue, 2);
  readWriteCharacteristic.setCallback(rwCharacCallback);
  simpleCustomService.addCharacteristic(readWriteCharacteristic);

  // Add the Service
  DuoBLE.addService(simpleCustomService);

  // Start stack
  DuoBLE.begin();

  // The Advertised name and "local" name should have some agreement
  DuoBLE.advertisingDataAddName(ADVERTISEMENT, shortDeviceName);
  DuoBLE.setName(shortDeviceName);

  // Start advertising.
  DuoBLE.startAdvertising();
  Serial.println("BLE start advertising.");
}



/**
   @brief Loop.
*/
void loop() {
  // Wait 1s before checking again.
  delay(1000);
}

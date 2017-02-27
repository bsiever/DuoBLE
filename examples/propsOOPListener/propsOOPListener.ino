/**
 * Simple Service that supports all properties of a characteristic and
 * uses an Object as a Listener for Events
 *
 * Search for HERE: to find distinguishing features
 *
 * Service (692eeca1-911e-42b6-ad4a-2bc6ef8db298)
 *   Ping Characteristic (5d638fbd-44ac-45cf-93f3-9a985ba68056)
 *     Read, Write, Notify, and Indicate.
 *         Notify/indicate increase value.
 *         Read merely reads.  Write has no effect.
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#include <DuoBLE.h>

// Disable WiFi
SYSTEM_MODE(MANUAL);

const char *shortDeviceName = "Prop";

// Define a service and its data
BLEService pingService("692eeca1-911e-42b6-ad4a-2bc6ef8db298");
BLECharacteristic pingCharacteristic("5d638fbd-44ac-45cf-93f3-9a985ba68056", ATT_PROPERTY_INDICATE|ATT_PROPERTY_NOTIFY|ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, 4,4);


/*
 * HERE:
 * Rather than using a c-style function for callbacks, this uses a Listener object.
 * (This uses one object for both Service updates and characteristic updates.
 *  They can be separated)

 * To keep the example concise, all function definitions are included in the class
 * declaration (this isn't ideal C++ style).
 *
 * Not all callback functions need to be implemented --- only those of interest
 */
class ListenerObject : public BLEServiceListener, public BLECharacteristicListener {
  // Service Listener Functions
  void connected(uint16_t conn_handle) {
    Serial.println("Connected");
  }
  void disconnected(uint16_t conn_handle) {
    Serial.println("Disconnected");
  }

  // Characteristic Listener Functions
  void preRead(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: preread");
    } else {
      Serial.println("unknown: preread");
    }
  }
  void postRead(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: postRead");
    } else {
      Serial.println("unknown: postRead");
    }
  }
  void postWrite(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: postWrite");
    } else {
      Serial.println("unknown: postWrite");
    }
  }
  void notificationsEnabled(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: notificationsEnabled");
    } else {
      Serial.println("unknown: notificationsEnabled");
    }
  }
  void notificationsDisabled(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: notificationsDisabled");
    } else {
      Serial.println("unknown: notificationsDisabled");
    }
  }
  void indicationsEnabled(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: indicationsEnabled");
    } else {
      Serial.println("unknown: indicationsEnabled");
    }
  }
  void indicationsDisabled(BLERecipient recipient) {
    if(recipient == pingCharacteristic) {
      Serial.println("ping: indicationsDisabled");
    } else {
      Serial.println("unknown: indicationsDisabled");
    }
  }
};

// HERE: Create an instance of it (as a global so it will always exist)
ListenerObject listenerObject;

// Forward declarations of callbacks for timers
void notify();
void indicate();

// Timer objects to demonstrate notifications and indications
Timer doNotify(1000, notify);
Timer doIndicate(1000, indicate);


void setup() {
  delay(2000);
  Serial.begin(9600);

  // Set the data in the custom characteristics:
  byte intValue[] = {0,0,0,0};  // A 4 byte integer counter
  pingCharacteristic.setValue(intValue, 4);
  // HERE: Set the listener object for the Characteristic
  pingCharacteristic.setListener(listenerObject);
  pingService.addCharacteristic(pingCharacteristic);
  // HERE: Set the listener object for the Service too
  pingService.setListener(listenerObject);

  // Add the Service
  DuoBLE.addService(pingService);

  // Start stack
  DuoBLE.begin();

  // The Advertised name and "local" name should have some agreement
  DuoBLE.advertisingDataAddName(ADVERTISEMENT, shortDeviceName);
  DuoBLE.setName(shortDeviceName);

  // Start advertising.
  DuoBLE.startAdvertising();

  Serial.println("Advertising");
  doNotify.start();
  delay(500);
  doIndicate.start();
}

int count = 0;
// Add one to the value and update the characteristic (characterstic uses little endian format)
void updateValue() {
  count++;
  // Update the stored data
  byte data[4];
  for(int i=0;i<4;i++)
    data[i]=count >> (24-8*i);
  pingCharacteristic.setValue(data, 4);
}

void notify() {
  updateValue();
  Serial.println("Notify");
  pingCharacteristic.sendNotify();
}
void indicate() {
  updateValue();
  Serial.println("Indicate");
  pingCharacteristic.sendIndicate();
}

void loop() {
  Serial.println("Loop");
  delay(10000);
}

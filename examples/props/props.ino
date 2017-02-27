/**
 *
 * Simple Service that supports all properties of a characteristic and
 * uses  C function style callbacks for events
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
 * Here's the callback that is used for all events on the characteristic
 */
void callback(BLERecipient recipient, BLECharacteristicCallbackReason reason) {
  const char *reasons[] = {"PREREAD",  "POSTREAD", "POSTWRITE", "NOTIFICATIONS_ENABLED", "NOTIFICATIONS_DISABLED",  "INDICATIONS_ENABLED", "INDICATIONS_DISABLED"};
  Serial.print("callback: ");
  Serial.println(reasons[reason]);
}

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
  // HERE: Set the callback function
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


/**
 * Example of using the Heart Rate Service 
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#include <DuoBLE.h>
#include"services/DIS.h"
#include"services/HRS.h"

// Disable WiFi 
SYSTEM_MODE(MANUAL);


uint8_t heartRate = 60; // Heart rate in beats per minute.
uint16_t respirationRate = 0;
uint16_t energyExpended = 0;
bool sensorConnected = true;
bool sending = false;
HRS hrs;

void hrsCallback(HRSCallbackReason reason) {
  switch(reason) {
    case HRS_RESPIRATION_RATE_CLEAR:
      energyExpended = 0;
    break;
    case HRS_SERVICE_STARTED:
      Serial.println("Starting HRS");
      sending = true;
    break;
    case HRS_SERVICE_STOPPED:
      Serial.println("Stopping HRS");
      sending = false;
    break;
  }
}


void setup() {
  RGB.control(true);
  RGB.color(0,0,0);

  Serial.begin(9600);

  hrs.setCallback(hrsCallback);
  // Set the initial heart rate
  hrs.setHeartRate8bit(heartRate);

  // Setup Services before Begin.
  DIS.addDeviceInfo("Acme", "M1", "Ser123", "HRev3", "Firm-4", "Soft5");
  DuoBLE.addService(hrs);
  // Start stack
  DuoBLE.begin();


  // Setup Advertising
  DuoBLE.setAdvertisementParameters();

  // Set BLE advertising and scan respond data
  DuoBLE.advertisingDataAddName(ADVERTISEMENT, "HRM");
  uint16_t uuids[] = {BLE_UUID_HEART_RATE_SERVICE};
  DuoBLE.advertisingDataAdd16bitUUIDs(ADVERTISEMENT, uuids, sizeof(uuids)/sizeof(uint16_t));

  DuoBLE.setName("HRM");
  DuoBLE.setAppearance(BLE_APPEARANCE_GENERIC_HEART_RATE_SENSOR);

  // Start advertising.
  DuoBLE.startAdvertising();
  Serial.println("BLE start advertising.");
}

void BLE_connected() {
  Serial.println("Main Connected");
}
void BLE_disconnected() {
  Serial.println("MainDisconnected");
  sending=false;
}


void loop() {
  if(sending) {
    // Send an update
    hrs.sendNotify();
    switch(random(0,10)) {
     case 0:
       Serial.println("Sensor Disconnected");
       hrs.sensorConnected(false);
     break;
     case 1:
       Serial.println("Sensor Connected");
       hrs.sensorConnected(true);
     break;
     case 2:
       Serial.println("Respiration Increase");
       respirationRate = constrain(respirationRate+random(0,10), 0, 0xFFFE);
       hrs.setRespirationRateInterval(respirationRate);
     break;
     case 3:
       Serial.println("Respiration Decrease");
       respirationRate = constrain(respirationRate-random(0,10), 0, 0xFFFE);
       hrs.setRespirationRateInterval(respirationRate);
     break;
     case 4:
       Serial.println("Respiration Clear");
       hrs.clearRespirationRateInterval();
     break;
      case 5:
       Serial.println("Energy Increase");
       energyExpended = constrain(energyExpended+random(0,10), 0, 0xFFFE);
       hrs.setEnergyExpended(energyExpended);
     break;
     case 6:
       Serial.println("Energy Increase");
       energyExpended = constrain(energyExpended+random(0,10), 0, 0xFFFE);
       hrs.setEnergyExpended(energyExpended);
     break;
     case 7:
       Serial.println("Energy Clear");
       hrs.clearEnergyExpended();
     break;
     case 8:
       Serial.println("Updating heart rate");
       heartRate = constrain(heartRate+random(-2,2),40,140);
       hrs.setHeartRate8bit(heartRate);
     break;
    }
  }
  // Wait 1s before checking again.
  delay(1000);
}

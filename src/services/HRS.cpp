
/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#include"HRS.h"
#include"bluetooth.h"

#include"Arduino.h"

HRS::HRS(uint8_t sensorLocation, bool oneByteFormat, bool sensorContactFeature, bool respirationFeature, bool energyExpendedFeature) : BLEService("180D"), m_hrm("2A37", ATT_PROPERTY_NOTIFY,7,2),
m_sensorLocation("2A38", ATT_PROPERTY_READ, 1,1), m_controlPoint("2A39",ATT_PROPERTY_WRITE, 1,1),
m_oneByteFormat(oneByteFormat), m_sensorContactFeature(sensorContactFeature), m_respirationFeature(respirationFeature), m_energyExpendedFeature(energyExpendedFeature),
m_energyExpended(0xFFFF), m_respirationRate(0xFFFF), m_sensorConnected(false), m_heartRate1Byte(0), m_heartRate2Bytes(0), m_callbackFunction(NULL)
 {
  addCharacteristic(m_hrm);
  m_hrm.setListener(this);

  addCharacteristic(m_sensorLocation);
  m_sensorLocation.setValue(&sensorLocation, 1);
  addCharacteristic(m_controlPoint);

  m_controlPoint.setListener(this);
}

void HRS::sensorConnected(bool connected) {
  m_sensorConnected = connected;
}

void HRS::setRespirationRateInterval(uint16_t rate) {
  m_respirationRate = rate;
}

void HRS::clearRespirationRateInterval() {
    m_respirationRate = 0xFFFF;
}

void HRS::setEnergyExpended(uint16_t value) {
  m_energyExpended = value;
}

void HRS::clearEnergyExpended() {
  m_energyExpended = 0xFFFF;
}

void HRS::setHeartRate16Bit(uint16_t value) {
  m_heartRate2Bytes = value;
}

void HRS::setHeartRate8bit(uint8_t value) {
  m_heartRate1Byte = value;
}

void HRS::setCallback(HRSCallback callback) {
  m_callbackFunction = callback;
}

void HRS::sendNotify() {
  uint8_t bytes[8];
  int index=0;

  bytes[0] = 0; // flags
  index++;

  if(m_oneByteFormat) {
    bytes[index] = m_heartRate1Byte;
    index++;
  } else {
    bytes[index] = m_heartRate2Bytes & 0xFF;
    index++;
    bytes[index] = m_heartRate2Bytes >> 8;
    index++;
    bytes[0] |= 0b1;  // Update flags
  }

  if(m_sensorContactFeature) {
    if(m_sensorConnected)
      bytes[0] |= (0b10 << 1);  // Update flags: Connected
    else
      bytes[0] |= (0xb11 << 1); // Update flags: Not connected
  }

  if(m_energyExpendedFeature && m_energyExpended!=0xFFFF) {
    bytes[index] = m_energyExpended & 0xFF;
    index++;
    bytes[index] = m_energyExpended >> 8;
    index++;
    bytes[0] |= (1<<3);     // Update flag
  }

  if(m_respirationFeature && m_respirationRate!=0xFFFF) {
    bytes[index] = m_respirationRate & 0xFF;
    index++;
    bytes[index] = m_respirationRate >> 8;
    index++;
    bytes[0] |= (1<<4);     // Update flag
  }
  m_hrm.setValue(bytes, index);
  m_hrm.sendNotify();
}

void HRS::postWrite(BLERecipient recipient) {
  if(m_callbackFunction)
    (m_callbackFunction)(HRS_RESPIRATION_RATE_CLEAR);
}

void HRS::notificationsEnabled(BLERecipient recipient) {
  if(m_callbackFunction)
    (m_callbackFunction)(HRS_SERVICE_STARTED);
}
void HRS::notificationsDisabled(BLERecipient recipient) {
  if(m_callbackFunction)
    (m_callbackFunction)(HRS_SERVICE_STOPPED);
}

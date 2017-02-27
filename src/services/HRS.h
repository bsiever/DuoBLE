/**
 * Heart Rate Service
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#ifndef _HRS_h_
#define _HRS_h_

#include"BLEService.h"
#include"BLECharacteristic.h"

// Simple Callbacks
typedef enum {
  HRS_RESPIRATION_RATE_CLEAR,
  HRS_SERVICE_STARTED,
  HRS_SERVICE_STOPPED
} HRSCallbackReason;

typedef void (*HRSCallback)(HRSCallbackReason reason);

// TODO: Omits the "sensor feature not supported in current connection"

class HRS : public BLEService, public BLECharacteristicListener  {

public:
  // Constructor
  HRS(uint8_t sensorLocation = 0, bool oneByteFormat=true, bool sensorContactFeature=true, bool respirationFeature=true, bool energyExpendedFeature=true);

  void sensorConnected(bool connected);

  void setRespirationRateInterval(uint16_t rate);
  void clearRespirationRateInterval();

  void setEnergyExpended(uint16_t value);
  void clearEnergyExpended();

  void setHeartRate16Bit(uint16_t value);
  void setHeartRate8bit(uint8_t value);

  void sendNotify();

  void setCallback(HRSCallback callback);

protected:
  void postWrite(BLERecipient recipient);
  void notificationsEnabled(BLERecipient recipient);
  void notificationsDisabled(BLERecipient recipient);

private:

  BLECharacteristic m_hrm; // Heart rate measurement
  BLECharacteristic m_sensorLocation; // Body Sensor location
  BLECharacteristic m_controlPoint;   // Control Point

  // Supported Features
  bool m_sensorContactFeature;
  bool m_respirationFeature;
  bool m_energyExpendedFeature;
  bool m_oneByteFormat;

  // Values
  uint16_t m_energyExpended;
  uint16_t m_respirationRate;
  bool     m_sensorConnected;

  uint8_t m_heartRate1Byte;
  uint16_t m_heartRate2Bytes;

  // Control point is only used to reset respiration rate
  HRSCallback m_callbackFunction;
};

#endif

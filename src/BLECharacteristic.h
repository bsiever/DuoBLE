/**
 * Bluetooth Low Energy Characteristic
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#ifndef _BLE_CHARACTERISTIC_h_
#define _BLE_CHARACTERISTIC_h_

#include<stdint.h>
#include"BLEAttribute.h"
#include"BLEUuid.h"
#include <cstddef>

#define MAX_ATT_SZ 20  // Maximum attribute size
#define MIN(a,b) ((a<b)?(a):(b))

class DuoBLE_;

// Stuff for C-style callbacks
typedef enum {
  PREREAD,
  POSTREAD,
  POSTWRITE,
  NOTIFICATIONS_ENABLED,
  NOTIFICATIONS_DISABLED,
  INDICATIONS_ENABLED,
  INDICATIONS_DISABLED
} BLECharacteristicCallbackReason;

// Simple Callbacks
typedef void (*BLECharacteristicCallback)(BLERecipient recipient, BLECharacteristicCallbackReason reason);


// Listener Object
class BLECharacteristicListener {

protected:
  virtual void preRead(BLERecipient recipient) {}
  virtual void postRead(BLERecipient recipient) {}
  virtual void postWrite(BLERecipient recipient) {}
  virtual void notificationsEnabled(BLERecipient recipient) {}
  virtual void notificationsDisabled(BLERecipient recipient) {}
  virtual void indicationsEnabled(BLERecipient recipient) {}
  virtual void indicationsDisabled(BLERecipient recipient) {}

  friend class DuoBLE_;
};


class BLECharacteristic : public BLEAttribute {
  public:
     BLECharacteristic(const char *uuidString, uint16_t properties, uint16_t maxSize=20, uint16_t minSize=1);
/*
Properties:
ATT_PROPERTY_READ
ATT_PROPERTY_WRITE_WITHOUT_RESPONSE 0x04
ATT_PROPERTY_WRITE               0x08
ATT_PROPERTY_NOTIFY              0x10
ATT_PROPERTY_INDICATE            0x20

ATT_PROPERTY_AUTHENTICATED_SIGNED_WRITE 0x40
ATT_PROPERTY_AUTHENTICATION_REQUIRED 0x400
ATT_PROPERTY_AUTHORIZATION_REQUIRED
*/

     void setValue(const uint8_t bytes[], uint16_t size);
     void setValue(const char *bytes, uint16_t size);
     uint16_t getValue(uint8_t bytes[], uint16_t maxSize);
     uint16_t size();
     void setListener(BLECharacteristicListener& listener);
     void setListener(BLECharacteristicListener* listener);
     void setCallback(BLECharacteristicCallback callback);

     void sendIndicate();
     void sendNotify();

     friend class BLEService;
     friend class DuoBLE_;

  private:
    UUID m_uuid;
    uint16_t m_minSize;
    uint16_t m_maxSize;
    uint16_t m_size;
    uint8_t m_value[MAX_ATT_SZ];
    uint16_t m_properties;
    bool m_notifications;
    bool m_indications;
    BLECharacteristicListener *m_listenerObject;
    BLECharacteristicCallback m_callbackFunction;
// Pointer to C-Style callback function
    BLECharacteristic *m_next;
};

#endif

/**
 * Bluetooth Low Energy Service
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#ifndef _BLE_SERVICE_h_
#define _BLE_SERVICE_h_

#include"BLEUuid.h"
#include"BLEAttribute.h"
#include <cstddef>

class BLECharacteristic;
class DuoBLE_;

// Status for C-style callbacks
typedef enum {
  CONNECTED,       //    ;;;status = BLE_STATUS_OK
  CONNECTION_ERROR, // Failed to successfully connect ;;; status = BLE_STATUS_CONNECTION_ERROR
  DISCONNECTED
} BLEServiceCallbackReason;

// Simple Callbacks
typedef void (*BLEServiceCallback)(uint16_t conn_handle, BLEServiceCallbackReason reason);

// OOP Callbacks: Listener Object
class BLEServiceListener {
protected:
  virtual void connected(uint16_t conn_handle) {}
  virtual void disconnected(uint16_t conn_handle) {}

  friend class DuoBLE_;
};

class BLEService {
public:
  BLEService(const char *uuidString);
  void addCharacteristic(BLECharacteristic& characteristics);
  const char *uuid();
  void setListener(BLEServiceListener& object);
  void setListener(BLEServiceListener* object);
  void setCallback(BLEServiceCallback callback);

  friend class DuoBLE_;

private:
  UUID m_uuid;
  BLEServiceListener *m_listenerObject;
  BLEServiceCallback m_callbackFunction;
  BLECharacteristic *m_characteristicChain;
  BLEService *m_nextService;
};

#endif

/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#include"BLECharacteristic.h"
#include"dble_assert.h"
#include<string.h>

#include"btstack_hal.h"
#define MIN(a,b) ((a<b)?(a):(b))
#include<Arduino.h>


BLECharacteristic::BLECharacteristic(const char *uuidString, uint16_t properties, uint16_t maxSize, uint16_t minSize) :
  BLEAttribute(), m_uuid(uuidString),  m_minSize(minSize), m_maxSize(maxSize), m_size(minSize), m_properties(properties),
  m_notifications(false), m_indications(false), m_listenerObject(NULL),  m_callbackFunction(NULL), m_next(NULL) {

/*
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

*/
  int stackBottom;
  dble_assert((int)this < (int)&stackBottom, "These objects should not be allocated on the stack (not local variables). Allocate as global or with new.");
  dble_assert(maxSize<=MAX_ATT_SZ && minSize>0 && minSize <= maxSize, "Invalid attribute size");
  memset(m_value, 0, MAX_ATT_SZ);
}

void BLECharacteristic::setValue(const char *bytes, uint16_t size) {
  ATOMIC_BLOCK() {
    m_size = MIN(size, m_maxSize);
    memcpy(m_value, bytes, m_size);
  }
}


void BLECharacteristic::setValue(const uint8_t bytes[], uint16_t size) {
  ATOMIC_BLOCK() {
    m_size = MIN(size, m_maxSize);
    memcpy(m_value, bytes, m_size);
  }
}

uint16_t BLECharacteristic::getValue(uint8_t bytes[], uint16_t maxSize) {
  SINGLE_THREADED_BLOCK() {
    memcpy(bytes, m_value, MIN(maxSize, m_size));
  }
  return m_size;
}

uint16_t BLECharacteristic::size() {
  return m_size;
}

void BLECharacteristic::setListener(BLECharacteristicListener& listener) {
  m_listenerObject = &listener;
}

void BLECharacteristic::setListener(BLECharacteristicListener* listener) {
  m_listenerObject =  listener;
}

void BLECharacteristic::setCallback(BLECharacteristicCallback callback) {
  m_callbackFunction = callback;
}

void BLECharacteristic::sendIndicate() {
  dble_assert(m_properties & ATT_PROPERTY_INDICATE, "Characteristic doesn't support indicate");
  if(m_indications && hal_btstack_attServerCanSend())
    hal_btstack_attServerSendIndicate(m_handle, m_value, m_size);
}

void BLECharacteristic::sendNotify() {
  dble_assert(m_properties & ATT_PROPERTY_NOTIFY , "Characteristic doesn't support notify");
  if(m_notifications && hal_btstack_attServerCanSend())
    hal_btstack_attServerSendNotify(m_handle, m_value, m_size);
}

/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#include"BLEService.h"
#include"BLECharacteristic.h"

BLEService::BLEService(const char *uuidString) : m_uuid(uuidString), m_listenerObject(NULL), m_callbackFunction(NULL), m_characteristicChain(NULL), m_nextService(NULL) {

}

void BLEService::addCharacteristic(BLECharacteristic& characteristic) {
  // Add to the end of the list (main chronological order of "addCharacteristic" calls)
  if(m_characteristicChain==NULL) {
    m_characteristicChain = &characteristic;
  } else {
    BLECharacteristic *last = m_characteristicChain;
    while(last->m_next != NULL)
        last = last->m_next;
    last->m_next = &characteristic;
  }
}

const char *BLEService::uuid() {
  return m_uuid.str();
}

void BLEService::setListener(BLEServiceListener& object) {
  m_listenerObject = &object;
}

void BLEService::setListener(BLEServiceListener* object) {
  m_listenerObject = object;
}

void BLEService::setCallback(BLEServiceCallback callback) {
  m_callbackFunction = callback;
}

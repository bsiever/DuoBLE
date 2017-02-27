/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#include"BLEAttribute.h"

BLEAttribute::BLEAttribute(uint16_t handle) : m_handle(handle) {

}

BLEAttribute::BLEAttribute() : m_handle(0) {

}

BLERecipient::BLERecipient(uint16_t handle) : BLEAttribute(handle) {
}

bool operator==(const BLEAttribute& one, const BLEAttribute& two) {
  // TODO: Should also validate connection handle (for mult-connection scenarios / central)
  return one.m_handle == two.m_handle;
}

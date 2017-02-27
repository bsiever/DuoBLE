/**
 * BLEAttributes (Common to all characteristics)
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#ifndef _BLE_ATTRIBTE_h_
#define _BLE_ATTRIBTE_h_

#include<stdint.h>

class DuoBLE_;

class BLEAttribute {
  friend bool operator==(const BLEAttribute& one, const BLEAttribute& two);
protected:
  BLEAttribute(uint16_t handle);
  BLEAttribute();

  uint16_t m_handle;
//  uint16_t m_connection; // Not needed now, but desirable for multi-connected scenarios

};

// BLERecipient is a generic BLEAttribute (used to match a Characteristic in a callback)
class BLERecipient : public BLEAttribute {
public:
protected:
  BLERecipient(uint16_t handle);
  friend class DuoBLE_;
};

bool operator==(const BLEAttribute& one, const BLEAttribute& two);

#endif

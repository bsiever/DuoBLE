/**
 *  UUID: Currently only supports 128-bit and 16-bit UUIDs
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 */

#ifndef _BLE_UUID_H_
#define _BLE_UUID_H_
// Loosely based on work by https://github.com/sandeepmistry/arduino-BLEPeripheral/

#include<stdint.h>

class DuoBLE_;

// For new library
class UUID {
public:
    UUID(const char *uuidString);  // Only allow string format / hex for UUID

    const char* str() const;
    int size() { return m_size; }

    friend class DuoBLE_;
  protected:
    // Bytes / pointer
     uint8_t * const bytes128() { return bytes; }
     uint16_t value16() { return bytes[0]<<8 | bytes[1]; }


    uint8_t bytes[16];
    int     m_size;
    const char   *_str;
};

#endif

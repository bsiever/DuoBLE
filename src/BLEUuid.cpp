/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 * Based on work by Sandeep Mistry: https://github.com/sandeepmistry/arduino-BLEPeripheral/
 */

#include "Arduino.h"
#include "BLEUuid.h"
#include"dble_assert.h"

UUID::UUID(const char *uuidString) {
  this->_str = uuidString;
  int len = strlen(uuidString);
  dble_assert(len == 4 || len==36, "Invalid UUID. Format is either XXXX or XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX");
  if(len == 4) {
    this->m_size = 2;
    char temp[] = {0, 0, 0};
    temp[0] = uuidString[0];
    temp[1] = uuidString[1];
    this->bytes[0] = strtoul(temp, NULL, 16);

    temp[0] = uuidString[2];
    temp[1] = uuidString[3];
    this->bytes[1] = strtoul(temp, NULL, 16);
  } else {
    this->m_size=16;
    char temp[] = {0, 0, 0};
    int length = 0;

    for (int i = 0; i < len && length < 16; i += 2) {
      if (uuidString[i] == '-') {
        i++;
      }

      temp[0] = uuidString[i];
      temp[1] = uuidString[i + 1];

      uint8_t value = strtoul(temp, NULL, 16);
      this->bytes[length] = value;

      length++;
    }
  }
}

const char* UUID::str() const {
  return this->_str;
}

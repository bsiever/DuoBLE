/**
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#include"DuoBLE.h"
#include"services/DIS.h"
#include<string.h>

static BLEService dis("180A");
static BLECharacteristic disManuName("2A29", ATT_PROPERTY_READ);
static BLECharacteristic disModelNum("2A24", ATT_PROPERTY_READ);
static BLECharacteristic disSerialNum("2A25", ATT_PROPERTY_READ);
static BLECharacteristic disHardwareRev("2A27", ATT_PROPERTY_READ);
static BLECharacteristic disFirmwareRev("2A26", ATT_PROPERTY_READ);
static BLECharacteristic disSoftwareRev("2A28", ATT_PROPERTY_READ);

static int length(const char *str) {
  return str?strlen(str):0;
}

void DIS_::addDeviceInfo( const char *manuName,           // Manufacturer Name
                          const char *modelNum,           // Model number
                          const char *serialNum,   // Serial number
                          const char *hardwareRev, // Hardware revision number
                          const char *firmwareRev, // Firmware revision number
                          const char *softwareRev  // Software revision number
) {
  const char *field;
  BLECharacteristic *charac;
  int size;
  int fields = 0;

  field = manuName;
  charac = &disManuName;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }

  field = modelNum;
  charac = &disModelNum;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }

  field = serialNum;
  charac = &disSerialNum;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }

  field = hardwareRev;
  charac = &disHardwareRev;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }

  field = firmwareRev;
  charac = &disFirmwareRev;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }

  field = softwareRev;
  charac = &disSoftwareRev;
  size = length(field);
  if(size>0) {
    charac->setValue(field, size);
    dis.addCharacteristic(*charac);
    fields++;
  }
  if(fields>0)
    DuoBLE.addService(dis);
}

DIS_ DIS;

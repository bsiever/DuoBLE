
/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#ifndef _BLETimerService_h_
#define _BLETimerService_h_

#include <DuoBLE.h>


/*
 * A Simple Service with a count-down timer
 */

class BLETimerService : public BLEService, public BLEServiceListener, public BLECharacteristicListener {
  static const int defaultInterval = 1000;

public:
  BLETimerService();


  void timerUp();


protected:
  void postWrite(BLERecipient recipient);
  void notificationsEnabled(BLERecipient recipient);
  void notificationsDisabled(BLERecipient recipient);
  void disconnected(uint16_t conn_handle);


private:
  BLECharacteristic notify;
  BLECharacteristic interval;
  Timer timer;
  int intervalVal;
  byte sendValue;
};

#endif

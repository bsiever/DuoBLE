
/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#include "BLETimerService.h"

BLETimerService::BLETimerService() :
   BLEService("79adead0-2326-4c55-8de6-ab60a504a7ee"),
   notify("79adead0-2326-4c55-8de6-ab60a504a7ee",ATT_PROPERTY_NOTIFY, 1,1),
   interval("7d7f2364-ca4d-47be-98bc-037317035f17",ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, 4, 4),
   timer(defaultInterval, &BLETimerService::timerUp, *this)
{
  addCharacteristic(notify);
  notify.setListener(this);

  intervalVal = defaultInterval;
  interval.setValue((byte*)&intervalVal, 4);
  addCharacteristic(interval);
  interval.setListener(this);

  // Set Service Listener (for disconnect);
  setListener(this);
}

void BLETimerService::timerUp() {
  // Send a notify
  sendValue++;
  notify.setValue(&sendValue, 1);
  notify.sendNotify();
}

void BLETimerService::postWrite(BLERecipient recipient) {
  if(recipient == interval) {
    timer.stop();
    byte bytes[4];
    interval.getValue(bytes, 4);
    intervalVal = bytes[0]<<0 | bytes[1]<<8 | bytes[2]<<16 | bytes[3]<<24;
    Serial.println(intervalVal);
    timer.changePeriod(intervalVal);
    timer.start();
  }
}
void BLETimerService::notificationsEnabled(BLERecipient recipient) {
  timer.start();
  sendValue = 0;
}
void BLETimerService::notificationsDisabled(BLERecipient recipient) {
  timer.stop();
}

void BLETimerService::disconnected(uint16_t conn_handle) {
  timer.stop();
}

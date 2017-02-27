

/**
 * Device Information Service
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */

#ifndef _DIS_h_
#define _DIS_h_

/**
  * Bluetooth Low Energy Device Information Service
*/
class DIS_ {
public:
  void addDeviceInfo(const char *manuName,    // Manufacturer Name
                     const char *modelNum = NULL,    // Model number
                     const char *serialNum = NULL,   // Serial number
                     const char *hardwareRev = NULL, // Hardware revision number
                     const char *firmwareRev = NULL, // Firmware revision number
                     const char *softwareRev = NULL  // Software revision number
  );
};


extern DIS_ DIS;

#endif

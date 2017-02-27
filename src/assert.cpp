/**
 * This is a library to simplify Bluetooth Low Energy for the RedBear Duo (and btstack).
 *
 * Copyright (c) 2017 Bill Siever.  All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for full license information.
 *
 */
#include<Arduino.h>

#include"assert.h"// handle diagnostic informations given by assertion and abort program execution:

#ifdef DEBUG

void __assert(const char *file, int line, const char *funct, const char *expres, const char * message) {
    // transmit diagnostic informations through serial link.
    Serial.begin(9600);
    Serial.print("Assertion failure in ");
    Serial.print(file);
    Serial.print(":");
    Serial.print(line, DEC);
    Serial.print(" ");
    Serial.print(funct);
    Serial.print("(): \n\t");
    Serial.print(expres);
    Serial.print(" : ");
    Serial.println(message);
    Serial.flush();
    // Halt program execution.
    SINGLE_THREADED_BLOCK() {
      // code here is executed atomically, without task switching or interrupts
      RGB.control(true);
      RGB.color(255,0,0);
      while(true) { delay(100); };  // Stay here forever

    }
}
#endif

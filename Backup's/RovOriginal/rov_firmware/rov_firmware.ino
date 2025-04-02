
/*
 Name:		rov_firmware.ino
 Created:	15.01.2018 14:16:59
 Author:	zinjk
*/


#include <Arduino.h>
#include <Wire.h>
#include "HighROV.h"

void setup() {
    Wire.begin();
    Wire.setTimeout(1000);
    Wire.setClock(10000);

    delay(100);

    SerialUSB.begin(115200);
    // Serial.begin(115200);
    // Serial1.begin(115200);

    HighROV::init();
}

void loop() {
    HighROV::run();
}

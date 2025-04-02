# SAMD_ISR_Servo Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_ISR_Servo.svg?)](https://www.ardu-badge.com/SAMD_ISR_Servo)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/SAMD_ISR_Servo.svg)](https://github.com/khoih-prog/SAMD_ISR_Servo/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/SAMD_ISR_Servo/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/SAMD_ISR_Servo.svg)](http://github.com/khoih-prog/SAMD_ISR_Servo/issues)

---
---

## Table of Contents

* [Why do we need this SAMD_ISR_Servo library](#why-do-we-need-this-samd_isr_servo-library)
  * [Features](#features)
  * [Important Notes about using ISR](#important-notes-about-using-isr)
  * [Currently supported Boards](#currently-supported-boards)
* [Changelog](#changelog)
  * [Releases v1.0.0](#releases-v100)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [What special in this SAMD_ISR_Servo library](#what-special-in-this-samd_isr_servo-library)
* [HOWTO Usage](#howto-usage)
* [Examples](#examples)
  * [ 1. SAMD_MultipleRandomServos](examples/SAMD_MultipleRandomServos)
  * [ 2. SAMD_MultipleServos](examples/SAMD_MultipleServos)
* [Example SAMD_MultipleServos](#example-samd_multipleservos)
  * [1. File SAMD_MultipleServos.ino](#1-file-samd_multipleservosino)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. SAMD_MultipleRandomServos on Adafruit SAMD51 ITSYBITSY_M4](#1-samd_multiplerandomservos-on-adafruit-samd51-itsybitsy_m4)
  * [2. SAMD_MultipleRandomServos on Arduino SAMD21 SAMD_NANO_33_IOT](#2-samd_multiplerandomservos-on-arduino-samd21-samd_nano_33_iot)
  * [3. SAMD_MultipleRandomServos on Seeeduino SAMD21 SEEED_XIAO_M0](#3-samd_multiplerandomservos-on-seeeduino-samd21-seeed_xiao_m0)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Releases](#releases)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)


---
---

### Why do we need this [SAMD_ISR_Servo library](https://github.com/khoih-prog/SAMD_ISR_Servo)

#### Features

Imagine you have a system with a **mission-critical function** controlling a **robot arm** or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is blocking the loop() or setup().

So your function might not be executed, and the result would be disastrous.

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a **Hardware Timer with Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are **much more precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the **loop() or setup() is blocked by certain operation**. For example, certain function is blocking while it's connecting to WiFi or some services.

This library enables you to use `1 Hardware Timer` on an SAMD-based board to control up to `16 independent servo motors`.


#### Important Notes about using ISR

1. Inside the attached function, delay() won’t work and the value returned by millis() will not increment. Serial data received while in the function may be lost. You should declare as volatile any variables that you modify within the attached function.

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

3. Avoid using Serial.print()-related functions inside ISR. Just for temporary debug purpose, but even this also can crash the system any time. Beware.

4. Your functions are now part of **ISR (Interrupt Service Routine)**, and must be `lean / mean`, and follow certain rules. More to read on:

[HOWTO Attach Interrupt](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)


### Currently supported Boards

  - **Arduino SAMD21 (ZERO, MKR, NANO_33_IOT, etc.)**.
  - **Adafruit SAM21 (Itsy-Bitsy M0, Metro M0, Feather M0, Gemma M0, etc.)**.
  - **Adafruit SAM51 (Itsy-Bitsy M4, Metro M4, Grand Central M4, Feather M4 Express, etc.)**.
  - **Seeeduino SAMD21/SAMD51 boards (SEEED_WIO_TERMINAL, SEEED_FEMTO_M0, SEEED_XIAO_M0, Wio_Lite_MG126, WIO_GPS_BOARD, SEEEDUINO_ZERO, SEEEDUINO_LORAWAN, SEEED_GROVE_UI_WIRELESS, etc.)** 
  - **Sparkfun SAMD21 boards** such as **SparkFun_RedBoard_Turbo, SparkFun_Qwiic_Micro, etc.**
  - **Sparkfun SAMD51 boards** such as **SparkFun_SAMD51_Thing_Plus, SparkFun_SAMD51_MicroMod, etc.**
 
---
---

## Changelog

### Releases v1.0.0

1. Basic 16 ISR-based servo controllers using 1 hardware timer for SAMD-based board
2. Support to both SAMD21 and SAMD51


---
---

## Prerequisites

1. [`Arduino IDE 1.8.15+`](https://www.arduino.cc/en/Main/Software)
2. [`Arduino SAMD core 1.8.11+`](https://github.com/arduino/ArduinoCore-samd) for SAMD ARM Cortex-M0+ boards. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-samd.svg)](https://github.com/arduino/ArduinoCore-samd/releases/latest)
3. [`Adafruit SAMD core 1.7.4+`](https://github.com/adafruit/ArduinoCore-samd) for SAMD ARM Cortex-M0+ and M4 boards (Nano 33 IoT, etc.). [![GitHub release](https://img.shields.io/github/release/adafruit/ArduinoCore-samd.svg)](https://github.com/adafruit/ArduinoCore-samd/releases/latest)
4. [`Seeeduino SAMD core 1.8.1+`](https://github.com/Seeed-Studio/ArduinoCore-samd) for SAMD21/SAMD51 boards (XIAO M0, Wio Terminal, etc.). [![Latest release](https://img.shields.io/github/release/Seeed-Studio/ArduinoCore-samd.svg)](https://github.com/Seeed-Studio/ArduinoCore-samd/releases/latest/)
5. [`Sparkfun SAMD core 1.8.1+`](https://github.com/sparkfun/Arduino_Boards) for SAMD21/SAMD51 boards (SparkFun_RedBoard_Turbo, SparkFun_SAMD51_Thing_Plus, etc.).

---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `SAMD_ISR_Servo`, then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/SAMD_ISR_Servo.svg?)](https://www.ardu-badge.com/SAMD_ISR_Servo) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [SAMD_ISR_Servo](https://github.com/khoih-prog/SAMD_ISR_Servo) page.
2. Download the latest release `SAMD_ISR_Servo-main.zip`.
3. Extract the zip file to `SAMD_ISR_Servo-main` directory 
4. Copy whole `SAMD_ISR_Servo-main` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**SAMD_ISR_Servo** library](https://platformio.org/lib/show/12721/SAMD_ISR_Servo) by using [Library Manager](https://platformio.org/lib/show/12721/SAMD_ISR_Servo/installation). Search for **SAMD_ISR_Servo** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

### What special in this [SAMD_ISR_Servo library](https://github.com/khoih-prog/SAMD_ISR_Servo)

Now these new **16 ISR-based Servo controllers** just use one SAMD Hardware Timer. The number 16 is just arbitrarily chosen, and depending on application, you can increase that number to 32, 48, etc. without problem.

The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers

Therefore, their executions are not blocked by bad-behaving functions / tasks. This important feature is absolutely necessary for mission-critical tasks. 

The [**SAMD_MultipleServos**](examples/SAMD_MultipleServos) example, which controls 6 servos independently, will demonstrate the nearly perfect accuracy.

Being ISR-based servo controllers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

---
---

## HOWTO Usage

How to use:

```
#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

#define TIMER_INTERRUPT_DEBUG       4
#define ISR_SERVO_DEBUG             4

#include "SAMD_ISR_Servo.h"

// TIMER_TC3 for SAMD51, TIMER_TC3/TIMER_TCC for SAMD21
#define USING_SAMD_TIMER            TIMER_TC3

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS        800
#define MAX_MICROS        2450 

#define SERVO_PIN_1       A0
#define SERVO_PIN_2       A1
#define SERVO_PIN_3       A2
#define SERVO_PIN_4       A3
#define SERVO_PIN_5       A4
#define SERVO_PIN_6       A5

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

#define NUM_SERVOS            6

ISR_servo_t ISR_servo[] =
{
  { -1, SERVO_PIN_1 }, { -1, SERVO_PIN_2 }, { -1, SERVO_PIN_3 }, { -1, SERVO_PIN_4 }, { -1, SERVO_PIN_5 }, { -1, SERVO_PIN_6 }
};


void setup()
{
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    pinMode(ISR_servo[index].servoPin, OUTPUT);
    digitalWrite(ISR_servo[index].servoPin, LOW);
  }
  
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting SAMD_MultipleRandomServos on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_ISR_SERVO_VERSION);

  // SAMD51 always uses TIMER_TC3
  SAMD_ISR_Servos.useTimer(USING_SAMD_TIMER);

  for (int index = 0; index < NUM_SERVOS; index++)
  {
    ISR_servo[index].servoIndex = SAMD_ISR_Servos.setupServo(ISR_servo[index].servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo[index].servoIndex != -1)
    {
      Serial.print(F("Setup OK Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
    else
    {
      Serial.print(F("Setup Failed Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
  }

  SAMD_ISR_Servos.setReadyToRun();
}

void printServoInfo(int indexServo)
{
  Serial.print(F("Servos idx = "));
  Serial.print(indexServo);
  Serial.print(F(", act. pos. (deg) = "));
  Serial.print(SAMD_ISR_Servos.getPosition(ISR_servo[indexServo].servoIndex) );
  Serial.print(F(", pulseWidth (us) = "));
  Serial.println(SAMD_ISR_Servos.getPulseWidth(ISR_servo[indexServo].servoIndex));
}

void loop()
{
  int position;      // position in degrees

  position = 0;
  Serial.println(F("Servos @ 0 degree"));
  
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    SAMD_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position );
    printServoInfo(index);
  }
  // waits 5s between test
  delay(5000);

  position = 90;
  Serial.println(F("Servos @ 90 degree"));
  
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    SAMD_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position );
    printServoInfo(index);
  }
  
  // waits 5s between test
  delay(5000);

  position = 180;
  Serial.println(F("Servos @ 180 degree"));
  
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    SAMD_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position );
    printServoInfo(index);
  }
  
  // waits 5s between test
  delay(5000);

  Serial.println(F("Servos sweeps from 0-180 degress"));
  
  for (position = 0; position <= 180; position += 5)
  {
    // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    for (int index = 0; index < NUM_SERVOS; index++)
    {
      SAMD_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position );
    }
    
    // waits 0.1s for the servo to reach the position
    delay(100);
  }
  
  // waits 5s between test
  delay(5000);
}
```

---
---

### Examples: 

 1. [SAMD_MultipleRandomServos](examples/SAMD_MultipleRandomServos) 
 2. [SAMD_MultipleServos](examples/SAMD_MultipleServos) 
 
---

### Example [SAMD_MultipleServos](examples/SAMD_MultipleServos)


#### 1. File [SAMD_MultipleServos.ino](examples/SAMD_MultipleServos/SAMD_MultipleServos.ino)

```cpp
#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

#define TIMER_INTERRUPT_DEBUG       4
#define ISR_SERVO_DEBUG             4

#include "SAMD_ISR_Servo.h"

// TIMER_TC3 for SAMD51, TIMER_TC3/TIMER_TCC for SAMD21
#define USING_SAMD_TIMER            TIMER_TC3

// Published values for SG90 servos; adjust if needed
#define MIN_MICROS        800
#define MAX_MICROS        2450

#define SERVO_PIN_1       A0
#define SERVO_PIN_2       A1
#define SERVO_PIN_3       A2
#define SERVO_PIN_4       A3
#define SERVO_PIN_5       A4
#define SERVO_PIN_6       A5

typedef struct
{
  int     servoIndex;
  uint8_t servoPin;
} ISR_servo_t;

#define NUM_SERVOS            6

ISR_servo_t ISR_servo[] =
{
  { -1, SERVO_PIN_1 }, { -1, SERVO_PIN_2 }, { -1, SERVO_PIN_3 }, { -1, SERVO_PIN_4 }, { -1, SERVO_PIN_5 }, { -1, SERVO_PIN_6 }
};

void setup()
{
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    pinMode(ISR_servo[index].servoPin, OUTPUT);
    digitalWrite(ISR_servo[index].servoPin, LOW);
  }
  
  Serial.begin(115200);
  while (!Serial);

  delay(200);

  Serial.print(F("\nStarting SAMD_MultipleServos on ")); Serial.println(BOARD_NAME);
  Serial.println(SAMD_ISR_SERVO_VERSION);

  // SAMD51 always uses TIMER_TC3
  SAMD_ISR_Servos.useTimer(USING_SAMD_TIMER);
  
  for (int index = 0; index < NUM_SERVOS; index++)
  {
    ISR_servo[index].servoIndex = SAMD_ISR_Servos.setupServo(ISR_servo[index].servoPin, MIN_MICROS, MAX_MICROS);

    if (ISR_servo[index].servoIndex != -1)
    {
      Serial.print(F("Setup OK, Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
    else
    {
      Serial.print(F("Setup Failed, Servo index = ")); Serial.println(ISR_servo[index].servoIndex);
    }
  }

  SAMD_ISR_Servos.setReadyToRun();
}

void loop()
{
  int position;      // position in degrees

  for (position = 0; position <= 180; position += 10)
  {
    // goes from 0 degrees to 180 degrees
    // in steps of 10 degree
    for (int index = 0; index < NUM_SERVOS; index++)
    {
      SAMD_ISR_Servos.setPosition(ISR_servo[index].servoIndex, position);
    }
    
    // waits 1s for the servo to reach the position
    delay(1000);
  }

  delay(5000);
}
```
---
---

### Debug Terminal Output Samples

### 1. SAMD_MultipleRandomServos on Adafruit SAMD51 ITSYBITSY_M4


```
Starting SAMD_MultipleRandomServos on ITSYBITSY_M4
SAMD_ISR_Servo v1.0.0
Setup OK, Servo index = 0
Setup OK, Servo index = 1
Setup OK, Servo index = 2
Setup OK, Servo index = 3
Setup OK, Servo index = 4
Setup OK, Servo index = 5
[ISR_SERVO] _period = 10 , frequency = 100000.00
[ISR_SERVO] SAMDFastTimerInterrupt: F_CPU (MHz) = 120 , TIMER_HZ = 48
[ISR_SERVO] TC_Timer::startTimer _Timer = 0x 4101C000
[ISR_SERVO] TC_Timer::startTimer TC3 = 0x 4101C000
[ISR_SERVO] SAMD51 TC3 period = 10 , _prescaler = 1
[ISR_SERVO] _compareValue = 479
[ISR_SERVO] Starting  ITimer OK
Servos @ 0 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos @ 90 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos @ 180 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos sweeps from 0-180 degress
Servos @ 0 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
800
```

---

### 2. SAMD_MultipleRandomServos on Arduino SAMD21 SAMD_NANO_33_IOT

```
Starting SAMD_MultipleRandomServos on SAMD_NANO_33_IOT
SAMD_ISR_Servo v1.0.0
Setup OK Servo index = 0
Setup OK Servo index = 1
Setup OK Servo index = 2
Setup OK Servo index = 3
Setup OK Servo index = 4
Setup OK Servo index = 5
[ISR_SERVO] _period = 20 , frequency = 50000.00
[ISR_SERVO] SAMDFastTimerInterrupt: F_CPU (MHz) = 48 , TIMER_HZ = 48
[ISR_SERVO] TC_Timer::startTimer _Timer = 0x 42002C00
[ISR_SERVO] TC_Timer::startTimer TC3 = 0x 42002C00
[ISR_SERVO] SAMD21 TC3 period = 20 , _prescaler = 1
[ISR_SERVO] _compareValue = 959
[ISR_SERVO] Starting  ITimer OK
Servos @ 0 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 40 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 40 , pos = 0
800
Servos @ 90 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 81 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 81 , pos = 90
1620
Servos @ 180 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos idx = 3, act. pos. (deg) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 3
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos idx = 4, act. pos. (deg) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 4
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos idx = 5, act. pos. (deg) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 122 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 5
[ISR_SERVO] cnt = 122 , pos = 180
2440
Servos sweeps from 0-180 degress
Servos @ 0 degree
```

---

### 3. SAMD_MultipleRandomServos on Seeeduino SAMD21 SEEED_XIAO_M0

```
Starting SAMD_MultipleRandomServos on SEEED_XIAO_M0
SAMD_ISR_Servo v1.0.0
[ISR_SERVO] _period = 20 , frequency = 50000.00
[ISR_SERVO] SAMDFastTimerInterrupt: F_CPU (MHz) = 48 , TIMER_HZ = 48
[ISR_SERVO] TC_Timer::startTimer _Timer = 0x 42002C00
[ISR_SERVO] TC_Timer::startTimer TC3 = 0x 42002C00
[ISR_SERVO] SAMD21 TC3 period = 10 , _prescaler = 1
[ISR_SERVO] _compareValue = 479
[ISR_SERVO] Starting  ITimer OK
Setup OK, Servo index = 0
Setup OK, Servo index = 1
Setup OK, Servo index = 2
Servos @ 0 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos @ 90 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
90, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 162 , pos = 90
1620
Servos @ 180 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 245 , pos = 180
180, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 245 , pos = 180
2450
Servos sweeps from 0-180 degress
Servos @ 0 degree
Servos idx = 0, act. pos. (deg) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 0
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 1, act. pos. (deg) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 1
[ISR_SERVO] cnt = 80 , pos = 0
800
Servos idx = 2, act. pos. (deg) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
0, pulseWidth (us) = [ISR_SERVO] Idx = 2
[ISR_SERVO] cnt = 80 , pos = 0
800
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level from 0 to 2. Be careful and using level 2 only for temporary debug purpose only.

```cpp
#define TIMER_INTERRUPT_DEBUG       1
#define ISR_SERVO_DEBUG             1
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

## Releases

### Releases v1.0.0

1. Basic 16 ISR-based servo controllers using 1 hardware timer for SAMD-based board
2. Support to both SAMD21 and SAMD51

---
---

### Issues

Submit issues to: [SAMD_ISR_Servo issues](https://github.com/khoih-prog/SAMD_ISR_Servo/issues)

---
---

## TO DO

1. Search for bug and improvement.

---

## DONE

1. Similar features for Arduino (UNO, Mega, etc...), ESP32, ESP8266, STM32 and RP2040
2. Add functions `getPosition()` and `getPulseWidth()`
3. Optimize the code
4. Add complicated examples

---
---

### Contributions and thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library. Especially to these people who have directly or indirectly contributed to this [SAMD_ISR_Servo library](https://github.com/khoih-prog/SAMD_ISR_Servo)


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/SAMD_ISR_Servo/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang

/****************************************************************************************************************************
  SAMD_ISR_Servo.h
  For :
  - SAMD21-based boards such as Nano-33-IoT, SAMD Zero, Seeeduino XIAO, etc.
  - SAMD51-based boards such as Adafruit Itsy-Bitsy M4, Metro M4, Seeeduino WIO-Terminal, etc.

  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/SAMD_ISR_Servo
  Licensed under MIT license

  Based on SimpleTimer - A timer library for Arduino.
  Author: mromani@ottotecnica.com
  Copyright (c) 2010 OTTOTECNICA Italy

  Based on BlynkTimer.h
  Author: Volodymyr Shymanskyy

  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K Hoang      21/08/2021 Initial coding for SAMD21/SAMD51 boards
 *****************************************************************************************************************************/

#pragma once

#ifndef SAMD_ISR_Servo_H
#define SAMD_ISR_Servo_H

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
    || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
    || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) || defined(__SAMD21G18A__) \
    || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD21E18A__) || defined(__SAMD51__) || defined(__SAMD51J20A__) || defined(__SAMD51J19A__) \
    || defined(__SAMD51G19A__) || defined(__SAMD51P19A__) || defined(__SAMD21G18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

// Too many boards sharing the same ARDUINO_SAMD_ZERO but very different, such as SAMD21 SparkFun RedBoard Turbo
// Have to exclude some from the list
#if ( defined(ARDUINO_SAMD_ZERO) && ! ( defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_SAMD_FEATHER_M0) || \
      defined(ADAFRUIT_METRO_M0_EXPRESS) || defined(ARDUINO_SAMD_HALLOWING_M0) || defined(ADAFRUIT_BLM_BADGE) ) )
  // Specific for SAMD21 SparkFun RedBoard Turbo
  #if !defined(Serial)
    #define Serial    SerialUSB
  #endif
#endif

#include "Arduino.h"

#ifndef SAMD_ISR_SERVO_VERSION
  #define SAMD_ISR_SERVO_VERSION       "SAMD_ISR_Servo v1.0.0"
#endif

//////////////////////////////////////////////////////////////////

#include <stddef.h>

#include <inttypes.h>

#if defined(ARDUINO)
  #if ARDUINO >= 100
    #include <Arduino.h>
  #else
    #include <WProgram.h>
  #endif
#endif

#include "SAMD_ISR_Servo_Debug.h"
#include "SAMD_FastTimerInterrupt.h"

#define SAMD_MAX_PIN           NUM_DIGITAL_PINS
#define SAMD_WRONG_PIN         255

// From Servo.h - Copyright (c) 2009 Michael Margolis.  All right reserved.

#define MIN_PULSE_WIDTH         800       // the shortest pulse sent to a servo
#define MAX_PULSE_WIDTH         2450      // the longest pulse sent to a servo
#define DEFAULT_PULSE_WIDTH     1500      // default pulse width when servo is attached
#define REFRESH_INTERVAL        20000     // minumim time to refresh servos in microseconds

extern void SAMD_ISR_Servo_Handler();

#if TIMER_INTERRUPT_USING_SAMD51
  // Use 10 microsecs timer for fast SAMD51, just fine enough to control Servo, normally requiring pulse width (PWM) 500-2000us in 20ms.
  #define TIMER_INTERVAL_MICRO        10
#else
  // Use 20 microsecs timer for slow SAMD21, just fine enough to control Servo, normally requiring pulse width (PWM) 500-2000us in 20ms.
  #define TIMER_INTERVAL_MICRO        20
#endif

class SAMD_ISR_Servo
{

  public:
    // maximum number of servos
    const static int MAX_ISR_SERVOS = 16;

    // constructor
    SAMD_ISR_Servo();

    // destructor
    ~SAMD_ISR_Servo()
    {
      if (SAMD_ITimer)
      {
        SAMD_ITimer->detachInterrupt();
        delete SAMD_ITimer;
      }
    }

    //////////////////////////////////////////////////

    void run();

    //////////////////////////////////////////////////

    // useTimer select which timer of SAMD to use for Servos: TIMER_TC3 for SAMD51, TIMER_TC3/TIMER_TCC for SAMD21
    //Return true if timerN0 in range
    void useTimer(SAMDTimerNumber timerNo)
    {
#if (TIMER_INTERRUPT_USING_SAMD51)
      (void) timerNo;

      _timerNo = TIMER_TC3;
#else
      if ( (timerNo == TIMER_TC3) || (timerNo == TIMER_TCC) )
      {
        _timerNo = timerNo;
      }
      else
      {
        ISR_SERVO_LOGERROR("Wrong Timer, force to use TIMER_TC3");
      }
#endif
    }

    //////////////////////////////////////////////////

    void setReadyToRun()
    {
      // Interval in microsecs
      if ( SAMD_ITimer && SAMD_ITimer->attachInterruptInterval(TIMER_INTERVAL_MICRO, (samd_timer_callback) SAMD_ISR_Servo_Handler ) )
      {
        ISR_SERVO_LOGERROR("Starting  ITimer OK");
      }
      else
      {
        ISR_SERVO_LOGERROR("Fail setup SAMD_ITimer");      }
    }

    // Bind servo to the timer and pin, return servoIndex
    int setupServo(uint8_t pin, int min = MIN_PULSE_WIDTH, int max = MAX_PULSE_WIDTH);

    // setPosition will set servo to position in degrees
    // by using PWM, turn HIGH 'duration' microseconds within REFRESH_INTERVAL (20000us)
    // returns true on success or -1 on wrong servoIndex
    bool setPosition(unsigned servoIndex, int position);

    // returns last position in degrees if success, or -1 on wrong servoIndex
    int getPosition(unsigned servoIndex);

    // setPulseWidth will set servo PWM Pulse Width in microseconds, correcponding to certain position in degrees
    // by using PWM, turn HIGH 'pulseWidth' microseconds within REFRESH_INTERVAL (20000us)
    // min and max for each individual servo are enforced
    // returns true on success or -1 on wrong servoIndex
    bool setPulseWidth(unsigned servoIndex, unsigned int pulseWidth);

    // returns pulseWidth in microsecs (within min/max range) if success, or 0 on wrong servoIndex
    unsigned int getPulseWidth(unsigned servoIndex);

    // destroy the specified servo
    void deleteServo(unsigned servoIndex);

    // returns true if the specified servo is enabled
    bool isEnabled(unsigned servoIndex);

    // enables the specified servo
    bool enable(unsigned servoIndex);

    // disables the specified servo
    bool disable(unsigned servoIndex);

    // enables all servos
    void enableAll();

    // disables all servos
    void disableAll();

    // enables the specified servo if it's currently disabled,
    // and vice-versa
    bool toggle(unsigned servoIndex);

    // returns the number of used servos
    int getNumServos();

    // returns the number of available servos
    int getNumAvailableServos() {
      return MAX_ISR_SERVOS - numServos;
    };

  private:

    void init()
    {
      SAMD_ITimer = new SAMDFastTimer(_timerNo);

      for (int servoIndex = 0; servoIndex < MAX_ISR_SERVOS; servoIndex++)
      {
        memset((void*) &servo[servoIndex], 0, sizeof (servo_t));
        servo[servoIndex].count    = 0;
        servo[servoIndex].enabled  = false;
        // Intentional bad pin
        servo[servoIndex].pin      = SAMD_WRONG_PIN;
      }

      numServos   = 0;

      // Init timerCount
      timerCount  = 1;
    }

    //////////////////////////////////////////////////

    // find the first available slot
    int findFirstFreeSlot();

    //////////////////////////////////////////////////

    typedef struct
    {
      uint8_t       pin;                  // pin servo connected to
      unsigned long count;                // In microsecs
      int           position;             // In degrees
      bool          enabled;              // true if enabled
      uint16_t      min;
      uint16_t      max;
    } servo_t;

    //////////////////////////////////////////////////

    volatile servo_t servo[MAX_ISR_SERVOS];

    // actual number of servos in use (-1 means uninitialized)
    volatile int numServos;

    // timerCount starts at 1, and counting up to (REFRESH_INTERVAL / TIMER_INTERVAL_MICRO) = (20000 / 10) = 2000
    // then reset to 1. Use this to calculate when to turn ON / OFF pulse to servo
    // For example, servo1 uses pulse width 1000us => turned ON when timerCount = 1, turned OFF when timerCount = 1000 / TIMER_INTERVAL_MICRO = 100
    volatile unsigned long timerCount;

    // For SAMD timer
    SAMDTimerNumber  _timerNo;
    SAMDFastTimer*   SAMD_ITimer;
};

extern SAMD_ISR_Servo SAMD_ISR_Servos;  // create servo object to control up to 16 servos


#endif

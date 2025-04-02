/****************************************************************************************************************************
  SAMD_ISR_Servo_Debug.h
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

#ifndef SAMD_ISR_Servo_Debug_h
#define SAMD_ISR_Servo_Debug_h

//////////////////////////////////////////

#ifndef TIMER_INTERRUPT_DEBUG
  #define TIMER_INTERRUPT_DEBUG         0
#endif

#ifndef ISR_SERVO_DEBUG
  #define ISR_SERVO_DEBUG               0
#endif

//////////////////////////////////////////

#if !defined(ISR_SERVO_DEBUG_OUTPUT)
  #define ISR_SERVO_DEBUG_OUTPUT    SerialUSB
#endif

//////////////////////////////////////////////////////

const char ISR_SERVO_MARK[]  = "[ISR_SERVO] ";
const char ISR_SERVO_SP[]    = " ";

#define ISR_SERVO_PRINT          ISR_SERVO_DEBUG_OUTPUT.print
#define ISR_SERVO_PRINTLN        ISR_SERVO_DEBUG_OUTPUT.println
#define ISR_SERVO_FLUSH          ISR_SERVO_DEBUG_OUTPUT.flush

#define ISR_SERVO_PRINT_MARK     ISR_SERVO_PRINT(ISR_SERVO_MARK)
#define ISR_SERVO_PRINT_SP       ISR_SERVO_PRINT(ISR_SERVO_SP)

//////////////////////////////////////////////////////

#define ISR_SERVO_LOGERROR(x)         if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x); }
#define ISR_SERVO_HEXLOGERROR(x)      if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x, HEX); }
#define ISR_SERVO_LOGERROR0(x)        if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT(x); }
#define ISR_SERVO_HEXLOGERROR0(x)     if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT(x, HEX); }
#define ISR_SERVO_LOGERROR1(x,y)      if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y); }
#define ISR_SERVO_HEXLOGERROR1(x,y)   if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y, HEX); }
#define ISR_SERVO_LOGERROR2(x,y,z)    if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(z); }
#define ISR_SERVO_LOGERROR3(x,y,z,w)  if(ISR_SERVO_DEBUG>0) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(z); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(w); }


//////////////////////////////////////////////////////

#define ISR_SERVO_LOGDEBUG(x)         if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x); }
#define ISR_SERVO_HEXLOGDEBUG(x)      if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINTLN(x, HEX); }
#define ISR_SERVO_LOGDEBUG0(x)        if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT(x); }
#define ISR_SERVO_HEXLOGDEBUG0(x)     if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT(x, HEX); }
#define ISR_SERVO_LOGDEBUG1(x,y)      if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y); }
#define ISR_SERVO_HEXLOGDEBUG1(x,y)   if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(y, HEX); }
#define ISR_SERVO_LOGDEBUG2(x,y,z)    if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(z); }
#define ISR_SERVO_LOGDEBUG3(x,y,z,w)  if(ISR_SERVO_DEBUG>1) { ISR_SERVO_PRINT_MARK; ISR_SERVO_PRINT(x); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(y); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINT(z); ISR_SERVO_PRINT_SP; ISR_SERVO_PRINTLN(w); }
//////////////////////////////////////////


#endif      // SAMD_ISR_Servo_Debug_h

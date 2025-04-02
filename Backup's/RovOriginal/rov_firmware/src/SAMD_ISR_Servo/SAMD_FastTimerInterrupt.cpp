/****************************************************************************************************************************
  SAMD_FastTimerInterrupt.cpp
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

#include "SAMD_FastTimerInterrupt.h"

#if (TIMER_INTERRUPT_USING_SAMD51)

////////////////////////////////////////////////////////

timerCallback TC3_callback;

void TC3_Handler() 
{
  // If this interrupt is due to the compare register matching the timer count
  if (TC3->COUNT16.INTFLAG.bit.MC0 == 1) 
  {
    TC3->COUNT16.INTFLAG.bit.MC0 = 1;
    (*TC3_callback)();
  }
}

////////////////////////////////////////////////////////

#elif (TIMER_INTERRUPT_USING_SAMD21)

////////////////////////////////////////////////////////

timerCallback TC3_callback;
timerCallback TCC_callback;

void TC3_Handler()
{
  // get timer struct
	TcCount16* TC = (TcCount16*) TC3;
	
  // If the compare register matching the timer count, trigger this interrupt
  if (TC->INTFLAG.bit.MC0 == 1) 
  {
    TC->INTFLAG.bit.MC0 = 1;
		(*TC3_callback)();
  }
}

//////////////////////////////////////////////////

void TCC0_Handler()
{
  // get timer struct
  Tcc* TC = (Tcc*) TCC0;
  
  // If the compare register matching the timer count, trigger this interrupt
  if (TC->INTFLAG.bit.MC0 == 1) 
  {  
    // A compare to cc0 caused the interrupt
	  TC->INTFLAG.bit.MC0 = 1;    // writing a one clears the flag ovf flag
  }

  if (TC->INTFLAG.bit.OVF == 1) 
  {
	  (*TCC_callback)();
	  
	  TC->INTFLAG.bit.OVF = 1;
  }
}

////////////////////////////////////////////////////////

#endif    // (TIMER_INTERRUPT_USING_SAMD51)

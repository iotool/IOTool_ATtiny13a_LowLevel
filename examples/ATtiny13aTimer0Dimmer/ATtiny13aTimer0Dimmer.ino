// Timer example for accurate timing with low level api to pulse led 100 nanoseconds.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2019-03-21  RoHa  304 byte ROM (16%), 2 byte RAM (3%)

#include "iotool_attiny13a_lowlevel.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define DIMMER_4V6  0    /* 6.3% current = 52% light */
#define DIMMER_4V5  43   /* 7.5% current = 62% light */
#define DIMMER_4V4  96   /* 10% current = 66% light */
#define DIMMER_4V3  133  /* 13% current = 70% light */
#define DIMMER_4V2  175  /* 20% current = 76% light */
#define DIMMER_4V1  201  /* 29% current = 82% light */
#define DIMMER_4V0  224  /* 50% current = 89% light */
#define DIMMER_3V9  238  /* 89% current = 97% light */
#define DIMMER_3V9  238  /* 89% current = 97% light */
#define DIMMER_ON   239  /* 100% current = 100% light */
#define DIMMER_OFF  240  /* 0% current = 0% light */

volatile static uint8_t vTimerDimmer = DIMMER_OFF;

TIMER_ON_OVERFLOW 
{
  if (vTimerDimmer != DIMMER_ON) {
    DIGITALWRITE_PB4_LOW;
  }
  TIMER_SET_COUNTER = vTimerDimmer;
}

TIMER_ON_COMPARE_A
{
  DIGITALWRITE_PB4_HIGH;
}

int main() 
{
  // --- setup ---
  PINMODE_PB4_OUTPUT;

  OVRFLTI600KHZ_586HZ;         // 256x 6.66us @ 600 kHz
  TIMER_USE_OVERFLOW;          
  TIMER_USE_COMP_KEEP; 
  TIMER_USE_COMPARE_A;
  TIMER_SET_COUNTER   = 0;     // reset Timer
  TIMER_SET_COMPARE_A = 240;   // 100ns = 15x 6.666us
  TIMER_START;                 // Global Interrupts aktivieren
  
  // --- loop ---
  while(1)
  {
    loop();
  }
}

volatile static uint32_t vTimerNext = 150000;

void loop() {
  DELAY_1S_600KHZ;
       if (vTimerDimmer == DIMMER_OFF) { vTimerDimmer = DIMMER_ON; }
  else if (vTimerDimmer == DIMMER_ON)  { vTimerDimmer = DIMMER_3V9; }
  else if (vTimerDimmer == DIMMER_3V9) { vTimerDimmer = DIMMER_4V0; }
  else if (vTimerDimmer == DIMMER_4V0) { vTimerDimmer = DIMMER_4V1; }
  else if (vTimerDimmer == DIMMER_4V1) { vTimerDimmer = DIMMER_4V2; }
  else if (vTimerDimmer == DIMMER_4V2) { vTimerDimmer = DIMMER_4V3; }
  else if (vTimerDimmer == DIMMER_4V3) { vTimerDimmer = DIMMER_4V4; }
  else if (vTimerDimmer == DIMMER_4V4) { vTimerDimmer = DIMMER_4V5; }
  else if (vTimerDimmer == DIMMER_4V5) { vTimerDimmer = DIMMER_4V6; }
  else if (vTimerDimmer == DIMMER_4V6) { vTimerDimmer = DIMMER_OFF; }
}


// Timer example for accurate timing with low level api to toggle led every second.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2018-04-04  RoHa  168 byte ROM (16%), 2 byte RAM (3%)

#include "iotool_attiny13a_lowlevel.h"

volatile static uint32_t vTimerCount = 0;

TIMER_ON_OVERFLOW
{
  OVERFLOWCOUNT_100US;
  if (vTimerCount == 0) {
    // every 1 second (1.000.000 us)
    DIGITALWRITE_PB4_TOGGLE;
    vTimerCount = 10000;
  }
  vTimerCount--;
}

int main() 
{
  // --- setup ---
  PINMODE_PB4_OUTPUT;

  // --- timer ---
  OVERFLOWTIMER_1X2MHZ;
  TIMER_USE_OVERFLOW;
  TIMER_START;

  // --- loop ---
  while(1)
  {
    loop();
  }
}

void loop() {
  DELAY_1MS;
}

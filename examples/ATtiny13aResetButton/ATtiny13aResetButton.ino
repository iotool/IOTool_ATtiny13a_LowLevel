// Example of reset safe memory.
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// This sketch demonstrate how you can use the noinit memory.
// After each reset the blink code will increment.
// 
// 2018-04-13  RoHa  Blink

#include "iotool_attiny13a_lowlevel.h"

USE_RESET_BUTTON;

int main() 
{
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;

  INIT_RESET_BUTTON;
  gRstMem.dat++; // 32-bit user data segment

  DIGITALWRITE_PB3_HIGH;
  DIGITALWRITE_PB4_LOW;

  while(1) {
    loop();
  }
}

void loop() 
{
  // display blink code
  for (uint32_t i=0; i<gRstMem.dat; i++) 
  {
    DIGITALWRITE_PB4_HIGH;
    DELAY_100MS;
    DIGITALWRITE_PB4_LOW;
    DELAY_100MS;
  }
  DELAY_1S;
}

// Blink example for usage low level api to digital write pins.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2018-04-03  RoHa  94 byte ROM (9%), 0 byte RAM (0%) 

#include "iotool_attiny13a_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB0_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB1_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB2_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB3_INPUT;      // pinMode(INPUT)
  PINMODE_PB4_OUTPUT;     // pinMode(OUTPUT)

  // --- loop ---
  while(1)                // forever
  {
    loop();               
  }
}

void loop() 
{
  DELAY_1S;
  DIGITALWRITE_PB4_HIGH;
  DELAY_1S;
  DIGITALWRITE_PB4_LOW;
}

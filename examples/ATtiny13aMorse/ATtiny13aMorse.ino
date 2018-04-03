// Example for output by morse code.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2018-04-03  RoHa  448 byte ROM (42%), 2 byte RAM (3%) 

#define ATTINY13A_MORSE_PIN4 1
//#define ATTINY13A_MORSE_DIGIT_5 1
#include "iotool_attiny13a_morse.h"
#include "iotool_attiny13a_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB0_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB1_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB2_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB3_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB4_OUTPUT;     // pinMode(OUTPUT)

  // --- loop ---
  while(1)                // forever
  {
    loop();               
  }
}

uint16_t gLoopCount = 0;  // value

void loop() 
{
  DELAY_1S;
  gLoopCount++;
  morseValue(gLoopCount);
}

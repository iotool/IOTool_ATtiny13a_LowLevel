// Example for usage low level api to read and write pins.
// Significant reduction of power customization by deep sleep.
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2017-04-07  RoHa  402 byte ROM (39%), 6 byte RAM (9%) 

#include "iotool_attiny13a_lowlevel.h"

uint16_t pinAdc = 0;       // 0-1023 ADC value
uint16_t timerCount = 0;   // count 64 ms intervals
uint16_t timerReset = 32;  // end of sleep interval 2s

// --- ticker ---
ISR(WDT_vect) {
  timerCount++;
}

int main() 
{
  // --- setup ---
  PINMODE_PB0_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB1_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB2_OUTPUT;     // pinMode(OUTPUT)
  PINMODE_PB3_INPUT;      // pinMode(INPUT)
  PINMODE_PB4_INPUT;      // pinMode(INPUT_PULLUP)
  DIGITALWRITE_PB4_HIGH;  // pullup

  // --- loop ---
  SLEEPTIMER_64MS;        // 64ms timer overflow interrupt
  SLEEPTIMER_START;       // start timer interrupt
  while(1)                // forever
  {
    loop();               // execute logic every 64ms
    sleep_mode();         // deep sleep to reduce power
  }
}

void loop() {
  // --- multiple tasks ---
  switch(timerCount) {
    case 1:
      // analog read
      ADC_ENABLE;
      ADC3_PB3_SELECT;
      pinAdc = readADC(8);
      pinAdc = readADC(8);
      ADC_DISABLE;
      // led
      if (pinAdc < 512) {
        DIGITALWRITE_PB0_HIGH;
        DIGITALWRITE_PB1_LOW;
      } else {
        DIGITALWRITE_PB0_LOW;
        DIGITALWRITE_PB1_HIGH;
      }
      break;
    case 2:
      // digital write
      if (DIGITALREAD_PB4_LOW) {
        DIGITALWRITE_PB2_HIGH;
      } else {
        DIGITALWRITE_PB2_LOW;
      }
      break;
    default:
      // restart logic after 2 seconds of power down
      if (timerCount > timerReset) {
        timerCount = 0;
      }
      break;
  }
}

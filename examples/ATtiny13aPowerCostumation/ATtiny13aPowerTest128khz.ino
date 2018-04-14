// ATtiny13A Power Costumation
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// This sketch test the power costumation.
// 
// 2018-04-13  RoHa  Blink

#include "iotool_attiny13a_lowlevel.h"

#define TEST_OFF 0
#define TEST_ON  1
#define TEST_CPU_PRESCALE  TEST_ON
#define TEST_ADC_DISABLED  TEST_ON
#define TEST_ACD_DISABLED  TEST_ON

USE_RESET_BUTTON;

int main() 
{
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;
  DIGITALWRITE_PB3_LOW;
  DIGITALWRITE_PB4_LOW;
  INIT_RESET_BUTTON;

  uint8_t lPrescale = 0;
  if (gRstMem.cnt >= 3) // keep programmable
  { 
    lPrescale = (gRstMem.cnt-5) % 9;
      for (uint8_t i=0; i<=lPrescale; i++) 
      {
        DIGITALWRITE_PB4_HIGH;
        DELAY_10MS;
        DIGITALWRITE_PB4_LOW;
        DELAY_10MS;
        DELAY_10MS;
      }
      if (TEST_CPU_PRESCALE == TEST_ON) {
        cli();
        CLKPR=(1<<CLKPCE);
        CLKPR=lPrescale;
        sei();
      }
      if (TEST_ADC_DISABLED == TEST_ON) {
        ADCSRA &= ~(1<<ADEN);
      }
      if (TEST_ACD_DISABLED == TEST_ON) {
        ACSR = (1<<ACD);
      }
  }

  while(1) {
    loop();
  }
}

void loop() 
{
  if (gRstMem.cnt < 3) {
    DIGITALWRITE_PB3_HIGH;
    DELAY_100MS;
    DIGITALWRITE_PB3_LOW;
    DELAY_100MS;
  }
  DELAY_1S;
}

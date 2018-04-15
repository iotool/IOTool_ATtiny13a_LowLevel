// ATtiny13A Power Costumation
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// This sketch test different power costumations.
// cpu source: 128 kHz, 4.8 MHz, 9.6 MHz
// pb3 led signal programmable mode
// pb4 led prescale blink code
// pb5 change prescale
// 
// 2018-04-13  RoHa  Blink

#include "iotool_attiny13a_lowlevel.h"

#define PRR     _SFR_IO8(0x25)
#define PRADC   0
#define PRTIM0  1

#define TEST_CPU 9600
#define TEST_OFF 0
#define TEST_ON  1
#define TEST_CPU_PRESCALE   TEST_ON
#define TEST_PIN_INPUTHIGH  TEST_OFF
#define TEST_ADC_DISABLED   TEST_ON
#define TEST_ACD_DISABLED   TEST_ON
#define TEST_TIM_DISABLED   TEST_ON

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
        if (TEST_CPU == 128)  {DELAY_10MS;}
        if (TEST_CPU == 4800) {DELAY_50MS;}
        if (TEST_CPU == 9600) {DELAY_100MS;}
        DIGITALWRITE_PB4_LOW;
        if (TEST_CPU == 128)  {DELAY_10MS;}
        if (TEST_CPU == 128)  {DELAY_10MS;}
        if (TEST_CPU == 4800) {DELAY_50MS;}
        if (TEST_CPU == 4800) {DELAY_50MS;}
        if (TEST_CPU == 9600) {DELAY_100MS;}
        if (TEST_CPU == 9600) {DELAY_100MS;}
      }
      if (TEST_CPU_PRESCALE == TEST_ON) {
        cli();
        CLKPR=(1<<CLKPCE);
        CLKPR=lPrescale;
        sei();
      }
      if (TEST_PIN_INPUTHIGH == TEST_ON) {
        PINMODE_PB0_INPUT; DIGITALWRITE_PB0_LOW;
        PINMODE_PB1_INPUT; DIGITALWRITE_PB1_LOW;
        PINMODE_PB2_INPUT; DIGITALWRITE_PB2_LOW;
        PINMODE_PB3_INPUT; DIGITALWRITE_PB3_LOW;
        PINMODE_PB4_INPUT; DIGITALWRITE_PB4_LOW;
        PINMODE_PB5_INPUT; DIGITALWRITE_PB5_LOW;
      }
      if (TEST_ADC_DISABLED == TEST_ON) {
        ADCSRA &= ~(1<<ADEN);
        PRR |= (1<<PRADC);
      }
      if (TEST_ACD_DISABLED == TEST_ON) {
        ACSR = (1<<ACD);
      }
      if (TEST_TIM_DISABLED == TEST_ON) {
        PRR |= (1<<PRTIM0);
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

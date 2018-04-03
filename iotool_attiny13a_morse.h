// Output data by morse code for ATtiny13a at Arduino IDE 1.6.x
// I use the erdatsai's app "Morse Code Agent" to decode light.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// 2018-04-03  RoHa  v1.0

#include "iotool_attiny13a_lowlevel.h"

// morse 12 words per minute

#define ATTINY13A_MORSE
#define DELAY_MORSE   _delay_ms(100)
#define DELAY_MORSE_2 _delay_ms(200)
#define DELAY_MORSE_3 _delay_ms(300)

void morseDigit(byte digit) 
{
  uint16_t mc = 15391; // 11110000011111
  mc = mc >> digit;
  #ifdef ATTINY13A_MORSE_PIN0
    DIGITALWRITE_PB0_HIGH; DELAY_MORSE; if (mc & 16) DELAY_MORSE; DIGITALWRITE_PB0_LOW; DELAY_MORSE;
    DIGITALWRITE_PB0_HIGH; DELAY_MORSE; if (mc &  8) DELAY_MORSE; DIGITALWRITE_PB0_LOW; DELAY_MORSE;
    DIGITALWRITE_PB0_HIGH; DELAY_MORSE; if (mc &  4) DELAY_MORSE; DIGITALWRITE_PB0_LOW; DELAY_MORSE;
    DIGITALWRITE_PB0_HIGH; DELAY_MORSE; if (mc &  2) DELAY_MORSE; DIGITALWRITE_PB0_LOW; DELAY_MORSE;
    DIGITALWRITE_PB0_HIGH; DELAY_MORSE; if (mc &  1) DELAY_MORSE; DIGITALWRITE_PB0_LOW; DELAY_MORSE_3;
  #endif
  #ifdef ATTINY13A_MORSE_PIN1
    DIGITALWRITE_PB1_HIGH; DELAY_MORSE; if (mc & 16) DELAY_MORSE; DIGITALWRITE_PB1_LOW; DELAY_MORSE;
    DIGITALWRITE_PB1_HIGH; DELAY_MORSE; if (mc &  8) DELAY_MORSE; DIGITALWRITE_PB1_LOW; DELAY_MORSE;
    DIGITALWRITE_PB1_HIGH; DELAY_MORSE; if (mc &  4) DELAY_MORSE; DIGITALWRITE_PB1_LOW; DELAY_MORSE;
    DIGITALWRITE_PB1_HIGH; DELAY_MORSE; if (mc &  2) DELAY_MORSE; DIGITALWRITE_PB1_LOW; DELAY_MORSE;
    DIGITALWRITE_PB1_HIGH; DELAY_MORSE; if (mc &  1) DELAY_MORSE; DIGITALWRITE_PB1_LOW; DELAY_MORSE_3;
  #endif
  #ifdef ATTINY13A_MORSE_PIN2
    DIGITALWRITE_PB2_HIGH; DELAY_MORSE; if (mc & 16) DELAY_MORSE; DIGITALWRITE_PB2_LOW; DELAY_MORSE;
    DIGITALWRITE_PB2_HIGH; DELAY_MORSE; if (mc &  8) DELAY_MORSE; DIGITALWRITE_PB2_LOW; DELAY_MORSE;
    DIGITALWRITE_PB2_HIGH; DELAY_MORSE; if (mc &  4) DELAY_MORSE; DIGITALWRITE_PB2_LOW; DELAY_MORSE;
    DIGITALWRITE_PB2_HIGH; DELAY_MORSE; if (mc &  2) DELAY_MORSE; DIGITALWRITE_PB2_LOW; DELAY_MORSE;
    DIGITALWRITE_PB2_HIGH; DELAY_MORSE; if (mc &  1) DELAY_MORSE; DIGITALWRITE_PB2_LOW; DELAY_MORSE_3;
  #endif
  #ifdef ATTINY13A_MORSE_PIN3
    DIGITALWRITE_PB3_HIGH; DELAY_MORSE; if (mc & 16) DELAY_MORSE; DIGITALWRITE_PB3_LOW; DELAY_MORSE;
    DIGITALWRITE_PB3_HIGH; DELAY_MORSE; if (mc &  8) DELAY_MORSE; DIGITALWRITE_PB3_LOW; DELAY_MORSE;
    DIGITALWRITE_PB3_HIGH; DELAY_MORSE; if (mc &  4) DELAY_MORSE; DIGITALWRITE_PB3_LOW; DELAY_MORSE;
    DIGITALWRITE_PB3_HIGH; DELAY_MORSE; if (mc &  2) DELAY_MORSE; DIGITALWRITE_PB3_LOW; DELAY_MORSE;
    DIGITALWRITE_PB3_HIGH; DELAY_MORSE; if (mc &  1) DELAY_MORSE; DIGITALWRITE_PB3_LOW; DELAY_MORSE_3;
  #endif
  #ifdef ATTINY13A_MORSE_PIN4
    DIGITALWRITE_PB4_HIGH; DELAY_MORSE; if (mc & 16) DELAY_MORSE; DIGITALWRITE_PB4_LOW; DELAY_MORSE;
    DIGITALWRITE_PB4_HIGH; DELAY_MORSE; if (mc &  8) DELAY_MORSE; DIGITALWRITE_PB4_LOW; DELAY_MORSE;
    DIGITALWRITE_PB4_HIGH; DELAY_MORSE; if (mc &  4) DELAY_MORSE; DIGITALWRITE_PB4_LOW; DELAY_MORSE;
    DIGITALWRITE_PB4_HIGH; DELAY_MORSE; if (mc &  2) DELAY_MORSE; DIGITALWRITE_PB4_LOW; DELAY_MORSE;
    DIGITALWRITE_PB4_HIGH; DELAY_MORSE; if (mc &  1) DELAY_MORSE; DIGITALWRITE_PB4_LOW; DELAY_MORSE_3;
  #endif
}

void morseValue(uint16_t value) 
{
  #ifdef ATTINY13A_MORSE_DIGIT_5
    byte morse_10000 = 0;
  #endif
  byte morse_1000 = 0;
  byte morse_100 = 0;
  byte morse_10 = 0;
  byte morse_1 = 0;
  #ifdef ATTINY13A_MORSE_DIGIT_5
    while(value >= 10000) 
    {
      morse_10000++;
      value-=10000;
    }
  #endif
  while(value >= 1000) 
  {
    morse_1000++;
    value-=1000;
  }
  while(value >= 100) 
  {
    morse_100++;
    value-=100;
  }
  while(value >= 10) 
  {
    morse_10++;
    value-=10;
  }
  morse_1 = value;
  #ifdef ATTINY13A_MORSE_DIGIT_5
    morseDigit(morse_10000);
  #endif
  morseDigit(morse_1000);
  morseDigit(morse_100);
  morseDigit(morse_10);
  morseDigit(morse_1);
  DELAY_MORSE_3;
}
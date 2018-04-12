// Example for output by morse code of infrared command.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// AX1838 Infrared @ ATtiny13a 1.2 MHz
// GND = IR_GND(-)
// VCC = IR_VCC
// PB1 = IR_DAT(S)
// 
// 2018-04-03  RoHa  784 byte ROM (76%), 42 byte RAM (65%) 
// 2018-04-03  RoHa  NEC-17-Key-RC Aliexpress

#define ATTINY13A_MORSE_PIN3 1
// #define ATTINY13A_MORSE_DIGIT_5 1
#include "iotool_attiny13a_morse.h"
#include "iotool_attiny13a_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB1_INPUT;
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;

  DIGITALWRITE_PB3_HIGH;
  DELAY_1S;
  DIGITALWRITE_PB3_LOW;

  // --- loop ---
  while(1)                // forever
  {
    loop();               
  }
}

uint8_t  gToogle       = 0;
uint16_t gCountHigh    = 0;
uint16_t gCountLow     = 0;
uint8_t  gBitIndex     = 0;
uint8_t  gBitValue[36] = {0};

void loop() 
{
  // HIGH
  if (DIGITALREAD_PB1_HIGH) {
    // NEW IMPULSE
    if (gToogle == 0) {
      gToogle = 1;
      gCountHigh = 0;
      if (gBitIndex < 36) {
        if (gCountLow < 255) {
          gBitValue[gBitIndex] = gCountLow;            
        } else {
          gBitValue[gBitIndex] = 255;
        }
        gBitIndex++;
      }
    } 
    // OLD IMPULSE
    else {
      _delay_us(1);
      if (gCountHigh < 65500) {
        gCountHigh++;
      }
    }
  } 

  // LOW
  else {
    // NEW PAUSE
    if (gToogle == 1) {
      gToogle = 0;
      gCountLow = 0;
    } 
    // OLD PAUSE
    else {
      if (gCountLow < 65500) {
        gCountLow++;
      }
      if (gCountLow == 10000) {
        // END SIGNAL = 350 ms (10.000 * 35 us)
        if (gBitIndex > 0) {
          if (gBitIndex == 36) {
            // FULL SIGNAL
            uint8_t addr = 2+16; // HEADER
            uint8_t cmd1 = 0;
            uint8_t cmd2 = 0;
            // count 15 * 35 us =   525 us (lo_bit)
            // count 44 * 35 us = 1.540 us (hi_bit)
            if (gBitValue[++addr] > 30) {cmd1 += 0b10000000;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b01000000;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00100000;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00010000;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00001000;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00000100;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00000010;}
            if (gBitValue[++addr] > 30) {cmd1 += 0b00000001;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b10000000;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b01000000;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00100000;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00010000;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00001000;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00000100;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00000010;}
            if (gBitValue[++addr] < 31) {cmd2 += 0b00000001;}
            if ((cmd1 == cmd2) || (cmd1 == cmd2 +1)) {
              // SIGNAL EQUAL CHECKSUM
              if ((cmd1 == 80) && (cmd2 == 80)) {
                // OK
                DIGITALWRITE_PB3_TOGGLE;
                DIGITALWRITE_PB4_TOGGLE;
              }
              if ((cmd1 == 16) && (cmd2 == 16)) {
                // +
                DIGITALWRITE_PB3_HIGH;
              }
              if ((cmd1 == 48) && (cmd2 == 48)) {
                // -
                DIGITALWRITE_PB3_LOW;
              }
              if ((cmd1 == 81) && (cmd2 == 80)) {
                // <
                DIGITALWRITE_PB4_LOW;
              }
              if ((cmd1 == 209) && (cmd2 == 208)) {
                // >
                DIGITALWRITE_PB4_HIGH;
              }
              if ((cmd1 == 97) && (cmd2 == 96)) {
                // *
              }
              if ((cmd1 == 225) && (cmd2 == 224)) {
                // #
              }
              if ((cmd1 == 1) && (cmd2 == 0)) {
                // 1
              }
              if ((cmd1 == 0) && (cmd2 == 0)) {
                // 2
              }
              if ((cmd1 == 129) && (cmd2 == 128)) {
                // 3
              }
              if ((cmd1 == 65) && (cmd2 == 64)) {
                // 4
              }
              if ((cmd1 == 64) && (cmd2 == 64)) {
                // 5
              }
              if ((cmd1 == 193) && (cmd2 == 192)) {
                // 6
              }
              if ((cmd1 == 33) && (cmd2 == 32)) {
                // 7
              }
              if ((cmd1 == 32) && (cmd2 == 32)) {
                // 8
              }
              if ((cmd1 == 161) && (cmd2 == 160)) {
                // 9
              }
              if ((cmd1 == 96) && (cmd2 == 96)) {
                // 0
                DIGITALWRITE_PB3_LOW;
                DIGITALWRITE_PB4_LOW;
              }
              // morseValue(cmd1);
              // morseValue(cmd2);
            }
            // for (byte i=0; i<gBitIndex; i++) {
            //  morseValue(gBitValue[i]);          
            // }
          }
          gBitIndex = 0;
        }
      }
    }
  }
}

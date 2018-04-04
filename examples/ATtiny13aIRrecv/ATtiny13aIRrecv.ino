// Example for output by morse code of infrared command.
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// HX1838 Infrared @ ATtiny13a 1.2 MHz
// GND = IR_GND(-)
// VCC = IR_VCC
// PB1 = IR_DAT(S)
// 
// Implementation of the NEC protocol (without timer)
// http://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
// 
// 2018-04-03  RoHa  784 byte ROM (76%), 42 byte RAM (65%) 

#define ATTINY13A_MORSE_PIN4 1
// #define ATTINY13A_MORSE_DIGIT_5 1
#include "iotool_attiny13a_morse.h"
#include "iotool_attiny13a_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB1_INPUT;
  PINMODE_PB4_OUTPUT;

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
              morseValue(cmd1);
              morseValue(cmd2);
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

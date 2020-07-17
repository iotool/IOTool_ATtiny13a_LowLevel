// Blink ADC value
//
// LED on = start
// LED ticks 1.000 + 1000
// LED ticks 100   + 100
// LED ticks 10    + 10
// LED ticks 1     + 1
// 
// example: (1)(8)(10)(1) = 0790
// 
//         +---+
//      PB5| U |VCC (+)5V
//      PB3|   |PB2 Vref
//  LED PB4|   |PB1
//  (-) GND|   |PB0
//         +---+

#define _AVR_ATTINY13A_H_ 1  // iotn13a.h
#define F_CPU 1200000UL      // delay cpu clock
#include "iotool_attiny13a_lowlevel.h"
#include <avr/sleep.h>       // sleep to reduce power
#include <avr/eeprom.h>      // eeprom data
#include <util/delay.h>      // low level delay

#define DELAY_10MS_1MHZ2    _delay_ms(12)

#define RESET_MAGIC  0xDE49
typedef struct {                 //
  uint16_t crc;                  //
  uint8_t  cnt;                  // count reboots
} tRstMem;                       //
tRstMem gRstMem __attribute__    //
  ((__section__ (".noinit")))    //
;

void ledTick() {
  DIGITALWRITE_PB4_HIGH; 
  DELAY_100MS_1MHZ2; 
  DIGITALWRITE_PB4_LOW; 
  DELAY_100MS_1MHZ2;
  DELAY_100MS_1MHZ2;
  DELAY_100MS_1MHZ2;
}

int main() {

  // --- input ---
  ADC_ENABLE;
  ADC1_PB2_SELECT;
  uint16_t adc = readADC(2);  
  DELAY_10MS_1MHZ2; adc += readADC(2);
  DELAY_10MS_1MHZ2; adc += readADC(2);
  DELAY_10MS_1MHZ2; adc += readADC(2);
  adc /= 4;
  
  // --- reduce current ---
  ADCSRA &= ~(1<<ADEN);   // disable ADC to reduce current
  ACSR = (1<<ACD);        // disable ACD to reduce current
  DIDR0 = 0x3F;           // disable digital input buffers on all ADC0-ADC5 pins

  // --- boot ---
  if (gRstMem.crc != RESET_MAGIC) {
    gRstMem.crc = RESET_MAGIC;
    gRstMem.cnt = 0;
  } else {
    gRstMem.cnt++;
  }

  //--- display start
  PINMODE_PB4_OUTPUT;
  DIGITALWRITE_PB4_HIGH;
  DELAY_1S_1MHZ2;
  DIGITALWRITE_PB4_LOW;
  DELAY_1S_1MHZ2;

  //--- display adc

  ledTick();
  while (adc>1000) {
    ledTick();
    adc-=1000;  
  }
  DELAY_1S_1MHZ2;

  ledTick();
  while (adc>100) {
    ledTick();
    adc-=100;    
  }
  DELAY_1S_1MHZ2;

  ledTick();
  while (adc>10) {
    ledTick();
    adc-=10;    
  }
  DELAY_1S_1MHZ2;

  ledTick();
  while (adc>0) {
    ledTick();
    adc-=1;    
  }
  DELAY_1S_1MHZ2;


  PINMODE_PB4_INPUT;
  DIGITALWRITE_PB4_LOW;
  
  // --- restart ---
  SLEEPTIMER_CLEAR;
  SLEEPTIMER_2S;
  SLEEPTIMER_START;
  while(1) { sleep_mode();}

}

// ATtiny13 - Reaktiver Schalter mit 7.5Hz Lichtpulse
// 
// Dieses Programm schaltet PB2 ein, wenn der Lichtsensor 
// pulsiertes Licht (7.5 Hz bzw. 66ms Pulsweite) erkennt.
// 
//           ATtiny13
//         RST (1()8) VCC --- VCC --- 1M --- PB3
// LDR --- PB3 (2  7) PB2 --- LED
//         PB4 (3  6) PB1
// GND --- GND (4  5) PB0


#define IOTOOL__F_CPU 1200000UL   //  600 kHz CPU 
#include "iotool_attiny13a_lowlevel.h"

#define RESET_MAGIC  0xDE49      /*  detect initial boot  */
typedef struct {                 //  
  uint16_t crc;                  //  crc
  uint16_t count;                //  loopcount
  uint16_t adc_ldr;              //  light
  unsigned poweron:1;            //  poweron
  unsigned :4;                   //  
  unsigned blinkcount:3;         //  pulsecount
  uint8_t  blinkstream[8];       //  pulsestream

} tRstMem;                       //
tRstMem gRstMem __attribute__    //  
  ((__section__ (".noinit")))    //   
;                                //  

void main_init();                //  init
void main_work();                //  work
void main_work_blink();          //  blink
void main_delay();               //  delay

int main() 
{
  while(1) {
    main_init();
    main_work();
    main_delay();
  }
}

void main_init() 
{
  if (gRstMem.crc != RESET_MAGIC) {
    // --- Speicher ---
    gRstMem.crc = RESET_MAGIC;   //  init memory
    gRstMem.count      = 0;      //  loop counter
    gRstMem.poweron    = 1;      //  poweron
    // --- Hardware ---
    PINMODE_PB2_OUTPUT;          //  led
    PINMODE_PB3_INPUT;           //  ldr
    DIGITALWRITE_PB2_LOW;        //  led off
  } else {
    gRstMem.count++;             //  loop count
  }
}

#define blink_delay    _delay_ms(140)  /* 70m delay */
#define blink_adcgap   16              /* gap from adc average */
#define blink_adcavg   116             /* minimum light */
#define blink_cntmin   3               /* minimum blink counts */

void main_work_blink() 
{
  int16_t avg_adc = 0;
  for (uint8_t b=7; b>0; b--) {
    gRstMem.blinkstream[b] = gRstMem.blinkstream[b-1];
    avg_adc += gRstMem.blinkstream[b];
  }
  gRstMem.blinkstream[0] = 255;
  if (gRstMem.adc_ldr < 255) {
    gRstMem.blinkstream[0] = gRstMem.adc_ldr;
  }
  avg_adc += gRstMem.blinkstream[0];
  avg_adc = (avg_adc / 8);
  gRstMem.blinkcount = 0;
  for (int8_t b=7; b>-1; b--) {
    int8_t a = b-1;
    if (a==0) { a = 7; }
    if (avg_adc < blink_adcavg) { // Mindesthelligkeit
      if ( (gRstMem.blinkstream[b]+blink_adcgap<avg_adc) & (gRstMem.blinkstream[a]>avg_adc+blink_adcgap) ) {
        gRstMem.blinkcount++;
      }
      if ( (gRstMem.blinkstream[a]+blink_adcgap<avg_adc) & (gRstMem.blinkstream[b]>avg_adc+blink_adcgap) ) {
        gRstMem.blinkcount++;
      }
    }
  }
}

void main_work() 
{
  ADC_ENABLE;
  ADC3_PB3_SELECT;
  gRstMem.adc_ldr = readADC(4);
  ADC_DISABLE;
  main_work_blink();
  DIGITALWRITE_PB2_LOW;
  if (gRstMem.blinkcount >= blink_cntmin) {
    DIGITALWRITE_PB2_HIGH;
  }
}

void main_delay() 
{
  blink_delay;
}

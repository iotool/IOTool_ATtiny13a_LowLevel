// ATtiny13 - Gluecksrad mit 10 LEDs
// 
//           ATtiny13
// BTN --- RST (1()8) VCC --- VCC
// LED --- PB3 (2  7) PB2 --- ADC
// LED --- PB4 (3  6) PB1 --- LED
// GND --- GND (4  5) PB0 --- LED
// 
// Konfiguration Anzahl und Ziffern
// (+) drehen 1 (-) kurz  // Pin 145
// (+) drehen 4 (-) kurz  // Pin 145
// (+) drehen 5 (-) kurz  // Pin 145
// (+) drehen 0 (-) kurz  // Adr 01
// (+) drehen 1 (-) kurz  // Adr 01
// (+) drehen 0 (-) kurz  // Val 02
// (+) drehen 2 (-) kurz  // Val 02
// (+) drehen 9 (-) lang  // speichern
// 
// Adr[00] = Anzahl Ziffern
// Adr[01..15] = Ziffer

// #include "iotool_attiny13a_lowlevel.h"

#include <avr/sleep.h>       // sleep to reduce power
#include <avr/eeprom.h>      // eeprom data
#include <util/delay.h>      // low level delay

#define ADC_ENABLE               ADCSRA |= (1 << ADEN); ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC))
#define ADC1_PB2_SELECT          ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (0 << MUX1)|(1 << MUX0)
#define ANALOG_READ_ADC1_PB2     ADCSRA |= (1 << ADSC); /* _delay_ms(1); */ while((ADCSRA & 0x40) !=0){}

#define PINMODE_PB0_OUTPUT       DDRB |= (1 << PB0)
#define DIGITALWRITE_PB0_TOGGLE  PORTB ^= (1 << PB0)
#define DIGITALWRITE_PB0_HIGH    PORTB |= (1 << PB0)
#define DIGITALWRITE_PB1_HIGH    PORTB |= (1 << PB1)
#define DIGITALWRITE_PB2_HIGH    PORTB |= (1 << PB2)
#define DIGITALWRITE_PB3_HIGH    PORTB |= (1 << PB3)
#define DIGITALWRITE_PB4_HIGH    PORTB |= (1 << PB4)
#define DIGITALWRITE_PB5_HIGH    PORTB |= (1 << PB5)

#define SLEEPTIMER_CLEAR         WDTCR &= ~((1 << WDP3)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0))
#define SLEEPTIMER_125MS         WDTCR |= (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_250MS         WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_1S            WDTCR |= (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_START         WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN)

void ledSelect(int8_t led) 
{
  // --- charlieplexing --- 340
  uint8_t static const LED_PINOUT[] = {
    //043104301 Output High
    0b00110010, // 0  Y1
    0b00110001, // 1  R1
    0b01010001, // 2  B1
    0b01010100, // 3  W1
    0b10010001, // 4  G1
    0b10011000, // 5  Y2
    0b11001000, // 6  R2
    0b11000100, // 7  B2
    0b01100010, // 8  W2
    0b01100100, // 9  G2
    0b00000000  // 10 off
  };
  DDRB  &= ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4)); // pinModeInput
  PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4)); // digitalWriteLow
  DDRB  |= ((LED_PINOUT[led] & 0b11000000) >> 3) | ((LED_PINOUT[led] & 0b00110000) >> 4); // pinModeOutput
  PORTB |= ((LED_PINOUT[led] & 0b00001100) << 1) | ((LED_PINOUT[led] & 0b00000011) ); // digitalWriteHigh
}

#define RESET_MAGIC  0xDE49
typedef struct {                 //
  uint16_t crc;                  //
  uint8_t  cnt;                  // Rebootzaehler (Zeitscheibe)
  uint8_t  randomled;            // Zufallszahl 0..8 = Low ADC Bits 
  uint8_t  currentled;           // Aktuelle LED = Start 9
  uint8_t  eeprom[16];           // Koordinaten
  uint8_t  showdigit;            // Aktuelle Anzeige
  uint8_t  progstep;             //
  uint8_t  bytestream[8];        // Nop[0] Val[1,2] Adr[3,4] Pin[5,6,7]
  // unsigned rst:1;             //
  // unsigned :7;                //
} tRstMem;                       //
tRstMem gRstMem __attribute__    //
  ((__section__ (".noinit")))    //
;

int main() {

  // --- input ---
  ADC_ENABLE;
  ADC1_PB2_SELECT;
  ANALOG_READ_ADC1_PB2;

  // --- reduce current ---
  ADCSRA &= ~(1<<ADEN);   // disable ADC to reduce current
  ACSR = (1<<ACD);        // disable ACD to reduce current
  DIDR0 = 0x3F;           // disable digital input buffers on all ADC0-ADC5 pins

  // --- boot --- 358
  if (gRstMem.crc != RESET_MAGIC) {
    gRstMem.crc = RESET_MAGIC;
    gRstMem.cnt = 0;
    gRstMem.randomled = (ADC & 0b00000111) + ((ADC & 0b01000000) >> 6); // 0..8
    gRstMem.currentled = 9;
    gRstMem.showdigit = 0;
    gRstMem.progstep = 0;
    eeprom_read_block((void*)&gRstMem.eeprom, (const void*)0, 16);
  } else {
    gRstMem.cnt++;
  }
  uint8_t adc = ADC >> 2;

  // 452,474

  if (adc <= 32) { // if (adc < 130) {
    // --- battery off --
    // keep memory
    gRstMem.cnt = 0;
    gRstMem.showdigit = 0;
    ledSelect(gRstMem.currentled);
    _delay_ms(1);
    ledSelect(10);
    SLEEPTIMER_CLEAR;
    SLEEPTIMER_250MS;
  }
  else if (adc > 127) { // if (adc > 530) {
    // --- battery +9V --
    gRstMem.progstep = 1;
    // rotate led wheel (336ms = 16x21ms)
    if (gRstMem.cnt > 16) {
      gRstMem.cnt = 0;
      gRstMem.currentled++;
      if (gRstMem.currentled > 9) {
        gRstMem.currentled = 0; 
      }
    }
    ledSelect(gRstMem.currentled);
    SLEEPTIMER_CLEAR;
  }
  else {
    // --- battery -9V --
    if (gRstMem.progstep == 1) {
      // shift bytestream
      for (uint8_t b=7; b>0; b--) {
        gRstMem.bytestream[b] = gRstMem.bytestream[b-1];
      }
      gRstMem.bytestream[0] = gRstMem.currentled;
    }
    gRstMem.progstep = 2;
    // display result
    if (gRstMem.randomled != gRstMem.currentled) {
      // wrong - display double red led
      ledSelect(1+5*(gRstMem.cnt&1));
      if (gRstMem.cnt > 250) {
        // reset random after 5 seconds
        gRstMem.crc = 0;
        // check bytestream
        uint8_t eeprom_val =  10*gRstMem.bytestream[2]+gRstMem.bytestream[1];
        uint8_t eeprom_adr =  10*gRstMem.bytestream[4]+gRstMem.bytestream[3];
        uint16_t eeprom_pin = 100*gRstMem.bytestream[7]+10*gRstMem.bytestream[6]+gRstMem.bytestream[5];
        if (eeprom_pin == 145) {
          for (uint8_t b=4; b>0; b--) {
            ledSelect(gRstMem.bytestream[b] % 10); _delay_ms(700); ledSelect(10); _delay_ms(300);
          }
          gRstMem.eeprom[eeprom_adr] = eeprom_val;
          eeprom_write_block(&gRstMem.eeprom,(void*)0x00,16);  
          ledSelect(3); _delay_ms(1000);
        }
        // display yellow
        ledSelect(0);
        // next programmable mode
        _delay_ms(100);
      }
    } else {
      // rigth - display number
      if (gRstMem.showdigit == 0) {
        // double green led
        ledSelect(4+5*(gRstMem.cnt&1));
      } else {
        // show digit
        ledSelect( gRstMem.eeprom[gRstMem.showdigit]%11 );
      }
      if (gRstMem.cnt > 55) {
        // next
        gRstMem.cnt = 0;
        gRstMem.showdigit++;
        if (gRstMem.showdigit > gRstMem.eeprom[0]) {
          // first
          gRstMem.showdigit = 0;
        }
      }
      if (gRstMem.cnt > 40) {
        // blink
        ledSelect(10);
      }
    }
    SLEEPTIMER_CLEAR;
  }

  // --- restart ---
  SLEEPTIMER_START;
  while(1) { sleep_mode();}

}

// ATtiny13a PB4 Vref LED Button
//         __
//    +----||---+                  +--u--+
//    |  Button |               PB5|     |VCC
//    |         |               PB3| AT  |PB2    
//   GND--[|<]--+--[1M]--VCC    PB4| tiny|PB1
//        LED   |               GND| 13a |PB0
//            [3K3]                +-----+
//              |
//             PB4
// 
// Change EEPROM by Button
// 1. press superlong (>20 seconds) for beginn [blink: slow...fast]
// 2. press long (>2 seconds) to set number 0 [blink: long]
// 3. press short to add 1 to number [blink: short]
// 4. press long for next value [blink: long * number]
//    first number  = address [0..9]
//    second number = value high [0..9]
//    third number  = value low [0..9]
// 5. press superlong (>20 seconds) for save [blink: fast...fast]
// 
// Example (L=Long, S=Short, X=Superlong)
// EEPROM[0]=12  X-L-L-SL-SSL-X
// EEPROM[1]=00  X-L-SL-L-L-X
// EEPROM[2]=99  X-L-SSL-SSSSSSSSSL-SSSSSSSSSL-X
// 
// 2020-07-18  RoHa  PoC-V4 (574b ROM, 20b RAM)

#include <avr/sleep.h>         // sleep to reduce power
#include <avr/eeprom.h>        // eeprom data
#include <util/delay.h>        // low level delay

#define ADC_ENABLE             ADCSRA |= (1 << ADEN); ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC))
#define ADC2_PB4_SELECT        ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (1 << MUX1)|(0 << MUX0)
#define ANALOG_READ_ADC        ADCSRA |= (1 << ADSC); /* _delay_ms(1); */ while((ADCSRA & 0x40) !=0){}

#define PINMODE_PB4_INPUT      DDRB &= ~(1 << PB4)
#define PINMODE_PB4_OUTPUT     DDRB |= (1 << PB4)
#define DIGITALWRITE_PB4_HIGH  PORTB |= (1 << PB4)
#define DIGITALWRITE_PB4_LOW   PORTB &= ~(1 << PB4)

#define SLEEPTIMER_CLEAR       WDTCR &= ~((1 << WDP3)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0))
#define SLEEPTIMER_125MS       WDTCR |= (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_250MS       WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_500MS       WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_1S          WDTCR |= (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_START       WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN)

#define DELAY_10MS             _delay_ms(12)
#define DELAY_50MS             _delay_ms(60)
#define DELAY_100MS            _delay_ms(120)
#define DELAY_250MS            _delay_ms(300)
#define DELAY_500MS            _delay_ms(600)
#define DELAY_1S               _delay_ms(1200)

#define RESET_MAGIC  0xDE49
typedef struct {                 //
  uint16_t crc;                  //
  uint16_t cnt;                  // Rebootzaehler (Zeitscheibe)
  uint16_t pb4_adc;              // ADC
  uint8_t  pb4_button;           // Button pressed cycles
  uint8_t  pb4_count;            // Button Number
  uint8_t  pb4_mode;             // Button Number
  uint8_t  eeprom[10];           // EEPROM[0..9]
  uint8_t  bytestream[2];        // Val[0,1] Adr[2]
  // unsigned rst:1;             //
  // unsigned :7;                //
} tRstMem;                       //
tRstMem gRstMem __attribute__    //
  ((__section__ (".noinit")))    //
;

void pb4BlinkLed(uint8_t delay_on,uint8_t delay_off) {
  PINMODE_PB4_OUTPUT;
  DIGITALWRITE_PB4_HIGH;
  while(delay_on>0) {
    DELAY_10MS;
    delay_on--;
  }
  DIGITALWRITE_PB4_LOW;
  while(delay_off>0) {
    DELAY_10MS;
    delay_off--;
  }
  PINMODE_PB4_INPUT;
}

int main() {

  // --- boot ---
  if (gRstMem.crc != RESET_MAGIC) {
    gRstMem.crc = RESET_MAGIC;
    gRstMem.cnt = 0;
    gRstMem.pb4_adc = 0;
    gRstMem.pb4_mode = 0;
    // gRstMem.pb4_button = 0;
    // gRstMem.pb4_count = 0;
    eeprom_read_block((void*)&gRstMem.eeprom, (const void*)0, 10);
    // display eeprom[0] at boot
    gRstMem.pb4_count = gRstMem.eeprom[0]+1;
    gRstMem.pb4_button = 4;
    
  } else {
    gRstMem.cnt++;
  }

  // --- input ---
  ADC_ENABLE;
  PINMODE_PB4_INPUT;
  ADC2_PB4_SELECT;
  ANALOG_READ_ADC;
  gRstMem.pb4_adc = ADC;

  // --- reduce current ---
  ADCSRA &= ~(1<<ADEN);   // disable ADC to reduce current
  ACSR = (1<<ACD);        // disable ACD to reduce current
  DIDR0 = 0x3F;           // disable digital input buffers on all ADC0-ADC5 pins

  // --- controll ---

  if (/** BUTTON_DOWN **/ gRstMem.pb4_adc < 100) {
    gRstMem.pb4_button++;
    PINMODE_PB4_INPUT;
    DIGITALWRITE_PB4_LOW;
  } 
  else if (/** BUTTON_RELEASED **/ gRstMem.pb4_button > 0) {    
    if (/** BUTTON_SUPERLONG >=10s **/ gRstMem.pb4_button > 19 ) {
      if (/** BUTTON_PROG_START **/ gRstMem.pb4_mode == 0) {
        gRstMem.pb4_mode = 1;
      } else /** BUTTON_PROG_END **/ {
        gRstMem.pb4_mode = 0;
        uint8_t eeprom_val = 10*gRstMem.bytestream[1]+gRstMem.bytestream[0];
        uint8_t eeprom_adr = gRstMem.bytestream[2];
        gRstMem.eeprom[eeprom_adr] = eeprom_val;
        eeprom_write_block(&gRstMem.eeprom,(void*)0x00,10);
        gRstMem.pb4_button = 9;
      }
      while(gRstMem.pb4_button>0) {
        pb4BlinkLed(gRstMem.pb4_button,gRstMem.pb4_button);      
        gRstMem.pb4_button--;
      }
    }
    else if (/** BUTTON_LONG >=2s **/ gRstMem.pb4_button > 3 ) {
      /* >>> add to bytestream */
      for (uint8_t b=2; b>0; b--) {
        gRstMem.bytestream[b] = gRstMem.bytestream[b-1];
      }
      gRstMem.bytestream[0] = gRstMem.pb4_count;
      /* <<< */
      pb4BlinkLed(100,100);
      while(gRstMem.pb4_count > 0) {
        pb4BlinkLed(50,50);
        gRstMem.pb4_count--;
      }
      gRstMem.pb4_count = 0;
    } 
    else /** BUTTON_SHORT **/ {
      pb4BlinkLed(5,0);
      gRstMem.pb4_count++;
      if (gRstMem.pb4_count>9) {
        gRstMem.pb4_count = 0;
      }
    }
    gRstMem.pb4_button = 0;
  } 
  else /** BUTTON_NONE **/ {
  }
  
  // --- restart ---
  SLEEPTIMER_CLEAR;
  SLEEPTIMER_500MS;
  SLEEPTIMER_START;
  while(1) { sleep_mode();}

}

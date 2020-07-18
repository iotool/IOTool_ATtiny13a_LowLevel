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
// Note: 2 Minutes left for pause between button pressed.
// 
// Example (L=Long, S=Short, X=Superlong)
// EEPROM[0]=12  X-L-L-SL-SSL-X
// EEPROM[1]=00  X-L-SL-L-L-X
// EEPROM[2]=99  X-L-SSL-SSSSSSSSSL-SSSSSSSSSL-X
// 
// 2020-07-18  RoHa  PoC-V4 (574b ROM, 20b RAM)
// 2020-07-18  RoHa  PoC-V5: Timeout 127s (590b ROM, 22b RAM)

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
#define SLEEPTIMER_500MS       WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_START       WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN)

#define DELAY_10MS             _delay_ms(12)

#define RESET_MAGIC  0xDE49
typedef struct {                 // do not clear memory after deep sleep
  uint16_t crc;                  // magic value to detect first boot
  uint16_t cnt;                  // count reboots for timeslots
  uint16_t pb4_adc;              // pb4: adc value 
  uint8_t  pb4_button;           // pb4: button pressed timeslots
  uint8_t  pb4_count;            // pb4: number iterated 0..9 by push
  uint8_t  pb4_timeout;          // pb4: timeout for eeprom mode
  uint8_t  eeprom[10];           // eeprom[0..9]
  uint8_t  bytestream[3];        // val[0,1] adr[2]
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
    gRstMem.pb4_timeout = 0;
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
    // 0V at PB4, if button pressed (otherwise LED provides 1.8V)
    gRstMem.pb4_button++;   // count timeslots
    PINMODE_PB4_INPUT;      // disable output
    DIGITALWRITE_PB4_LOW;   // turn LED off
  }
  else if (/** BUTTON_RELEASED **/ gRstMem.pb4_button > 0) {    
    // button released after more than one timeslot it was down
    if (/** BUTTON_SUPERLONG >=10s **/ gRstMem.pb4_button > 19 ) {
      // button was more than 20 seconds pressed (20x 500ms sleep)
      if (/** BUTTON_PROG_START **/ gRstMem.pb4_timeout == 0) {
        // begin program mode, set timeout to 127 seconds and blink slower
        gRstMem.pb4_timeout = 255;
      } else /** BUTTON_PROG_END **/ {
        // end program mode, save to EEPROM and blink faster
        gRstMem.pb4_timeout = 0;
        uint8_t eeprom_val = 10*gRstMem.bytestream[1]+gRstMem.bytestream[0];
        uint8_t eeprom_adr = gRstMem.bytestream[2];
        gRstMem.eeprom[eeprom_adr] = eeprom_val;
        eeprom_write_block(&gRstMem.eeprom,(void*)0x00,10);
        gRstMem.pb4_button = 9;
      }
      // signal program mode by flash LED with reducing blink time
      while(gRstMem.pb4_button>0) {
        pb4BlinkLed(gRstMem.pb4_button,gRstMem.pb4_button);      
        gRstMem.pb4_button--;
      }
    }
    else if (/** BUTTON_LONG >=2s **/ gRstMem.pb4_button > 3 ) {
      // button was more than 2 seconds pressed (4x 500ms sleep)
      // stream of numbers
      for (uint8_t b=2; b>0; b--) {
        gRstMem.bytestream[b] = gRstMem.bytestream[b-1];
      }
      gRstMem.bytestream[0] = gRstMem.pb4_count;
      // display current number
      pb4BlinkLed(150,150);
      while(gRstMem.pb4_count > 0) {
        pb4BlinkLed(50,50);
        gRstMem.pb4_count--;
      }
      gRstMem.pb4_count = 0;
    } 
    else /** BUTTON_SHORT **/ {
      // button was less than 2 seconds pressed (4x 500ms sleep)
      // short blink to signal push event
      pb4BlinkLed(5,0);
      gRstMem.pb4_count++;
      if (gRstMem.pb4_count>9) {
        gRstMem.pb4_count = 0;
      }
    }
    gRstMem.pb4_button = 0;
  } 
  else /** BUTTON_NONE **/ {
    // button was note pressed
    if (gRstMem.pb4_timeout > 0) {
      // reduce timeout for program mode
      gRstMem.pb4_timeout--;
    }
  }
  
  // --- restart ---
  // deep sleep for 500ms and reduce current
  SLEEPTIMER_CLEAR;
  SLEEPTIMER_500MS;
  SLEEPTIMER_START;
  while(1) { sleep_mode();}

}

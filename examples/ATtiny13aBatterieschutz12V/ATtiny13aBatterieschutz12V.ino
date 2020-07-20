// 12V-Battery-Protect
//  
// Proof-of-Concept for a programmable car battery protection.
// You set the power-on and power-off value by button.
// 
// Plugin device to 12V the LED blink three times.
// Short push (<2 seconds) add one digit to the input number.
// Longer push (>2 seconds) confirm the number and set it to zero.
// Medium long push (>8 seconds) display the value of ADC or EEPROM.
// 
// Note: the calculated values of ADC and resistor don't match whit reality!
// Messure the voltage with a multimeter and display the real ADC value.
// Calculate the average value of ADC for the corresponding voltage.
// 
// To change the ADC value at EEPROM split the number into HI und LO part.
// EEPROM[1] = HI_POWER_ON  and EEPROM[2] = LO_POWER_ON
// EEPROM[3] = HI_POWER_OFF and EEPROM[4] = LO_POWER_OFF
// Example: ADC of 0810 mean 13.05 voltage
//  
//                    +--u--+
//                 PB5|     |VCC -- 5V(+)
//                 PB3| AT  |PB2 -- Vbattery
// LEB/Button ---- PB4| tiny|PB1
//     12V(-) ---- GND| 13a |PB0 -- Relais
//                    +-----+
// 
//    LED+Button:          V-Battery:  Relais:  
//   
//         __               12VBAT     +--5VCC
//    +----||---+             |        |   |
//    |  Button |           [680k]     |  R:In+
//    |         |             |        |   | 
// GND--[|<]--+--[1M]--VCC   PB2    |1mF| R:Sign--[3k3]--PB0
//      LED   |               |        |   |
//          [3K3]           [300k]     |  R:In-
//            |               |        |   |
//           PB4             GND       +--GND
// 
// Display values
// 1. press long (>2 seconds) to set number 0 [blink: long]
// 2. press short to add 1 to number [blink: short]
//    0 display adc value for 10 minutes
//    1..9 display eeprom value
// 3. press mediumlong (>8 seconds) to start display
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
// ADC    Vbat
// 809.70  13.38V  engine running
// 802     13.20V  > motor on  (777.87+31.83/0.79V*0.61V)
// 790     12.90V  < motor off (777.87+31.83/0.79V*0.31V)
// 777.87  12.59V  engine stopped
// 753.88  12.06V  battery low
//
// 827.5  13.38V  motor on
// 810~   13.05V  > motor on  (809.5=784.5+43/0.79V*0.46V)
// 804~   12.95V  > motor on (804.0=784.5+43/0.79V*0.36V)
// 793~   12.75V  < motor off (804.0=784.5+43/0.79V*0.16V)
// 784.5  12.59V  motor off
// 771~   12.40   battery half
// 749.0  12.09   battery empty
// 746.0  12.00   < start engine
// 737.2  11.75
// Note: 300k/680k = 0.306*12.00V = 3.67V/5V = 0.7347*1023 = 751 adc
// The theoretical ADC is 751) and the real ADC 746 for 12.00 voltage.
// 
// 2020-07-18  RoHa  EEPROM PoC-V4 (574b ROM, 20b RAM)
// 2020-07-18  RoHa  EEPROM PoC-V5: Timeout 127s (590b ROM, 22b RAM)
// 2020-07-20  RoHa  12V-Protect-V03 (926b ROM, 24b RAM)

#include <avr/sleep.h>         // sleep to reduce power
#include <avr/eeprom.h>        // eeprom data
#include <util/delay.h>        // low level delay

#define ADC_ENABLE             ADCSRA |= (1 << ADEN); ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC))
#define ADC1_PB2_SELECT        ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (0 << MUX1)|(1 << MUX0)
#define ADC2_PB4_SELECT        ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (1 << MUX1)|(0 << MUX0)
#define ANALOG_READ_ADC        ADCSRA |= (1 << ADSC); /* _delay_ms(1); */ while((ADCSRA & 0x40) !=0){}

#define PINMODE_PB0_OUTPUT     DDRB |= (1 << PB0)
#define DIGITALWRITE_PB0_HIGH  PORTB |= (1 << PB0)
#define DIGITALWRITE_PB0_LOW   PORTB &= ~(1 << PB0)
#define PINMODE_PB2_INPUT      DDRB &= ~(1 << PB2)
#define PINMODE_PB4_INPUT      DDRB &= ~(1 << PB4)
#define PINMODE_PB4_OUTPUT     DDRB |= (1 << PB4)
#define DIGITALWRITE_PB4_HIGH  PORTB |= (1 << PB4)
#define DIGITALWRITE_PB4_LOW   PORTB &= ~(1 << PB4)

#define SLEEPTIMER_CLEAR       WDTCR &= ~((1 << WDP3)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0))
#define SLEEPTIMER_500MS       WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_START       WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN)

#define DELAY_10MS             _delay_ms(12)
#define DELAY_1S               _delay_ms(1200)

#define RESET_MAGIC  0xDE49
typedef struct {                 // do not clear memory after deep sleep
  uint16_t crc;                  // magic value to detect first boot
  uint16_t pb2_adc;              // pb2: adc value V-Battery
  uint16_t pb4_adc;              // pb4: adc value Button
  uint8_t  pb4_button;           // pb4: button pressed timeslots
  uint8_t  pb4_count;            // pb4: number iterated 0..9 by push
  uint8_t  pb4_timeout;          // pb4: timeout for eeprom mode
  uint8_t  eeprom[10];           // eeprom[0..9]
  uint8_t  bytestream[3];        // val[0,1] adr[2]
  uint8_t  timeout;              // timeout counter
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

uint16_t pb4BlinkLedNumber(uint16_t number,uint16_t level) {
  while(number >= level) {
    pb4BlinkLed(25,25);  
    number -= level;
  }
  pb4BlinkLed(25,250);
  return number;
}

void pb4BlinkLedNumber1000(uint16_t number) {
  number = pb4BlinkLedNumber(number,1000);
  number = pb4BlinkLedNumber(number,100);
  number = pb4BlinkLedNumber(number,10);
  number = pb4BlinkLedNumber(number,1);
}

int main() {

  // --- input (a) ---
  ADC_ENABLE;             // enable ADC
  PINMODE_PB2_INPUT;      // PB2 V-Battery
  ADC1_PB2_SELECT;        // enable ADC on PB2
  ANALOG_READ_ADC;        // read ADC

  // --- boot ---
  if (gRstMem.crc != RESET_MAGIC) {
    gRstMem.crc = RESET_MAGIC;
    gRstMem.timeout = 0;
    gRstMem.pb2_adc = ADC;
    gRstMem.pb4_adc = 0;
    gRstMem.pb4_timeout = 0;
    gRstMem.pb4_button = 0;
    gRstMem.pb4_count = 0;
    eeprom_read_block((void*)&gRstMem.eeprom, (const void*)0, 10);
    // --- init eeprom ---
    if (gRstMem.eeprom[0]==255) {
      gRstMem.eeprom[0] = 111;
      gRstMem.eeprom[1] = 8; gRstMem.eeprom[2] =  2; /* 0802 = 13.20V */
      gRstMem.eeprom[3] = 7; gRstMem.eeprom[4] = 90; /* 0790 = 12.90V */
      gRstMem.pb4_button = 40;
      gRstMem.pb4_timeout = 1;
    }
    // --- display boot ---
    PINMODE_PB0_OUTPUT;
    DIGITALWRITE_PB0_LOW;
    pb4BlinkLedNumber(2,1);
    // display eeprom[0] at boot
    // gRstMem.pb4_count = gRstMem.eeprom[0]+1;
    // gRstMem.pb4_button = 4;    
    // } else {
    //   gRstMem.cnt++;
  }

  // --- input (b) ---  
  gRstMem.pb2_adc += ADC; // add new to old value 
  gRstMem.pb2_adc /= 2;   // average value
  PINMODE_PB4_INPUT;      // PB4 V-Button
  ADC2_PB4_SELECT;        // enable ADC on PB4
  ANALOG_READ_ADC;        // read ADC
  gRstMem.pb4_adc = ADC;  // relace value

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
    if (/** BUTTON_SUPERLONG >=20s **/ gRstMem.pb4_button >= 40 ) {
      // button was more than 20 seconds pressed (40x 500ms sleep)
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
      while(gRstMem.pb4_button > 0) {
        pb4BlinkLed(gRstMem.pb4_button,gRstMem.pb4_button);      
        gRstMem.pb4_button--;
      }
    }
    else if (/** BUTTON_LONG >=8s **/ gRstMem.pb4_button >= 16 ) {
      // button was more than 8 seconds pressed (18x 500ms sleep)
      pb4BlinkLed(150,150);
      pb4BlinkLed(150,150);
      if (gRstMem.pb4_count == 0) {
        // show adc value
        gRstMem.timeout = 255; /* 0001.xxxx.xxxx.xxxx */
      } else {
        // show eeprom value
        pb4BlinkLedNumber1000(gRstMem.eeprom[gRstMem.pb4_count]);
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
        pb4BlinkLed(25,25);  
        gRstMem.pb4_count--;
      }
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
    /** >>> LOOP >>> **/

    if (gRstMem.timeout > 0) {
      gRstMem.timeout--;
      pb4BlinkLedNumber1000(gRstMem.pb2_adc);
    }

    uint16_t eeprom_powerconf = 100*gRstMem.eeprom[1]+gRstMem.eeprom[2];
    if (gRstMem.pb2_adc >= eeprom_powerconf) {
      DIGITALWRITE_PB0_HIGH;
    }
    eeprom_powerconf = 100*gRstMem.eeprom[3]+gRstMem.eeprom[4];
    if (gRstMem.pb2_adc <= eeprom_powerconf) {
      DIGITALWRITE_PB0_LOW;
    }

    /** <<< LOOP <<< **/
  }
  
  // --- restart ---
  SLEEPTIMER_CLEAR;
  SLEEPTIMER_500MS;
  SLEEPTIMER_START;
  while(1) { sleep_mode();}

}

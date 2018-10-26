// ATtiny13 - Gluecksrad mit 10 LEDs
// 
// Dieses Programm zeigt 14 Ziffern ueber 10x LED fuer 0..9 an.
// Vorher muss eine zufaellig ausgewaehlte LED auf dem Gluecksrad gefunden werden.
// 
// Ablauf:
// 9V(+) anschliessen = Rad dreht sich
// 9V(+) entfernen = Rad stoppt / letzten LED blinkt
// 9V(-) anschliessen = 2x LED rot bei falscher Zahl
// 9V(+) anschliessen = Rad dreht sich
// 9V(+) entfernen = Rad stoppt / letzten LED blinkt
// 9V(-) anschliessen = 2x LED gruen / danach 14 Ziffern
// 9V(-) entfernen = letzten LED blinkt
// 9V(-) anschliessen = 2x LED gruen / danach 14 Ziffern
// 9V(+) anschliessen = Rad dreht sich / Neustart
// 
//           ATtiny13
// BTN --- RST (1()8) VCC --- VCC
// LED --- PB3 (2  7) PB2 --- ADC
// LED --- PB4 (3  6) PB1 --- LED
// GND --- GND (4  5) PB0 --- LED

#include "iotool_attiny13a_lowlevel.h"

void ledSelect(int8_t led) 
{
  // --- charlieplexing ---
  /* PINMODE_PB0_INPUT; PINMODE_PB1_INPUT; PINMODE_PB3_INPUT; PINMODE_PB4_INPUT; */
  /* DIGITALWRITE_PB0_LOW; DIGITALWRITE_PB1_LOW; DIGITALWRITE_PB3_LOW; DIGITALWRITE_PB4_LOW; */
  uint8_t ledList  = ~((1 << PB0) | (1 << PB1) | (1 << PB3) | (1 << PB4));
  DDRB  &= ledList; 
  PORTB &= ledList;   
  ledList = 0;
  if (led == 0) {ledList = (1 << PB0) | (1 << PB1); /* PINMODE_PB0_OUTPUT; PINMODE_PB1_OUTPUT; */ DIGITALWRITE_PB1_HIGH; } // Y1
  if (led == 1) {ledList = (1 << PB0) | (1 << PB1); /* PINMODE_PB0_OUTPUT; PINMODE_PB1_OUTPUT; */ DIGITALWRITE_PB0_HIGH; } // R1
  if (led == 2) {ledList = (1 << PB0) | (1 << PB3); /* PINMODE_PB0_OUTPUT; PINMODE_PB3_OUTPUT; */ DIGITALWRITE_PB0_HIGH; } // B1
  if (led == 3) {ledList = (1 << PB0) | (1 << PB3); /* PINMODE_PB0_OUTPUT; PINMODE_PB3_OUTPUT; */ DIGITALWRITE_PB3_HIGH; } // W1
  if (led == 4) {ledList = (1 << PB0) | (1 << PB4); /* PINMODE_PB0_OUTPUT; PINMODE_PB4_OUTPUT; */ DIGITALWRITE_PB0_HIGH; } // G1
  if (led == 5) {ledList = (1 << PB0) | (1 << PB4); /* PINMODE_PB0_OUTPUT; PINMODE_PB4_OUTPUT; */ DIGITALWRITE_PB4_HIGH; } // Y2
  if (led == 6) {ledList = (1 << PB3) | (1 << PB4); /* PINMODE_PB3_OUTPUT; PINMODE_PB4_OUTPUT; */ DIGITALWRITE_PB4_HIGH; } // R2
  if (led == 7) {ledList = (1 << PB3) | (1 << PB4); /* PINMODE_PB3_OUTPUT; PINMODE_PB4_OUTPUT; */ DIGITALWRITE_PB3_HIGH; } // B2
  if (led == 8) {ledList = (1 << PB1) | (1 << PB3); /* PINMODE_PB1_OUTPUT; PINMODE_PB3_OUTPUT; */ DIGITALWRITE_PB1_HIGH; } // W2
  if (led == 9) {ledList = (1 << PB1) | (1 << PB3); /* PINMODE_PB1_OUTPUT; PINMODE_PB3_OUTPUT; */ DIGITALWRITE_PB3_HIGH; } // G2
  DDRB |= ledList;
}

// reset safe memory
#define RESET_MAGIC 0xDE49
typedef struct {                          \
  uint16_t crc;                           \
  uint8_t  cnt;                           \
  unsigned flg_coords_digit:4;            \
  unsigned flg_coords_show:1;             \
  unsigned flg_second:1;                  \
  unsigned led_random:2;                  \
  uint32_t dat;                           \
} tRstMem;                                \
tRstMem gRstMem __attribute__             \
  ((__section__ (".noinit")))             \
;

int main()
{
  // --- setup ---
  PINMODE_PB2_INPUT;

  // --- battery ---
  ADC_ENABLE;
  ADC1_PB2_SELECT;
  uint16_t adc = readADC(4);

  // --- power safing ---
  ADCSRA &= ~(1<<ADEN);   // disable ADC
  ACSR = (1<<ACD);        // disable ACD

  // --- reset ---
  if (gRstMem.crc != RESET_MAGIC) {
    gRstMem.crc = RESET_MAGIC;
    gRstMem.cnt = 0;
    gRstMem.flg_second = 0;
    gRstMem.flg_coords_show = 0;
    gRstMem.flg_coords_digit = 0;
    gRstMem.led_random = adc & 3;
    gRstMem.dat = gRstMem.led_random + 1;
  } else {
    gRstMem.cnt++;                          
    gRstMem.flg_second ^= 1;
  }

  int8_t led = gRstMem.dat % 10;
  uint8_t coords[] = {4, 4,8,0,5,1,2,3,1,1,3,9,1,2,3, -1};

  // --- no battery ---
  if (adc < 130) {
    gRstMem.flg_coords_show = 0;
    if (led == gRstMem.led_random) {
      // found LED
      gRstMem.flg_coords_show = 1;
    }
    // reset coords to first digit
    gRstMem.flg_coords_digit = 0;
    // short led signal
    ledSelect(led);
    _delay_ms(1);
    ledSelect(-1);
    SLEEPTIMER_250MS;
  } 
  
  // --- (+)9V battery ---
  else if (adc > 530) {
    // turn wheel to next led
    led = (gRstMem.cnt >> 3) % 10;
    gRstMem.dat = led;
    ledSelect(led);
    if (gRstMem.cnt == 250) {
      gRstMem.cnt = 0;
    }
    // 16ms timer overflow interrupt
    WDTCR &= ~((1 << WDP3)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0));
  } 

  // --- (-)9V battery ---
  else {
    if (gRstMem.flg_coords_show == 1) {
      // led found (show coords 1..14 Nxx xx.xxx EExx.xx.xxx)
      led = coords[gRstMem.flg_coords_digit-1];
      if (gRstMem.flg_coords_digit == 1) {
        // led found (2x green led on)
        led += 5*gRstMem.flg_second;
      } 
      if (gRstMem.flg_coords_digit == 0) {
        // signal green led
        gRstMem.cnt = 180;
        gRstMem.flg_coords_digit++;
      }
      else if (gRstMem.cnt == 0) {
        // next coord (overflow)
        gRstMem.cnt = 205;
        gRstMem.flg_coords_digit++;
      }
      else if (gRstMem.cnt >= 235) {
        // led off (overflow)
        led = -1;
      }
      ledSelect(led);
    } else {
      // wrong led found (2x red led on)
      ledSelect(1+5*gRstMem.flg_second);
    }
    WDTCR &= ~((1 << WDP3)|(1 << WDP2)|(1 << WDP1)|(1 << WDP0));
  }
  
  // --- sleep ---
  DIDR0 = 0x3F;           // disable digital input buffers on all ADC0-ADC5 pins
  SLEEPTIMER_START;       // start timer interrupt
  while(1)                // forever
  {
    sleep_mode();         // deep sleep to reduce power
  }
  
}

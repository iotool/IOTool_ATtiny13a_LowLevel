// ATtiny13a Software-PWM 50 Hz to dimm a led or motor
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// Timer based pulse wide modulation (7-Bit, 30-50 Hz) to dimm led at any pin.
// At 1.2 MHz CPU the limit for PWM is 50 Hz as a timer background task.
// Under 30 Hz a dimmed LED flickers for human eyes.
//
// 2018-04-11 RoHa POC

#define F_CPU 1200000UL          // 1.2 MHz cpu clock
#include <avr/io.h>              // low level pinmode
#include <util/delay.h>          // low level delay

#define CONFIG_CPU               cli();CLKPR=(1<<CLKPCE);CLKPR=3;sei() // 1.2 MHz
#define TIMER_OVERFLOW_PRESCALE  TCCR0B |= (1<<CS01) // 150 kHz
#define TIMER_OVERFLOW_ENABLE    TIMSK0 |= (1<<TOIE0) 
#define TIMER_OVERFLOW_START     sei()
#define TIMER_ON_OVERFLOW        ISR(TIM0_OVF_vect)
#define TIMER_OVERFLOW_50KHZ     TCNT0 = 237 // 51.177 Hz
#define TIMER_OVERFLOW_40KHZ     TCNT0 = 231 // 40.241 Hz
#define TIMER_OVERFLOW_30KHZ     TCNT0 = 222 // 30.425 Hz
#define DELAY_1S                 _delay_ms(1200) 
#define DELAY_2S                 _delay_ms(2400) 
#define DELAY_100MS              _delay_ms(120)
#define PINMODE_PB0_OUTPUT       DDRB |= (1 << PB0)
#define PINMODE_PB1_OUTPUT       DDRB |= (1 << PB1)
#define PINMODE_PB2_OUTPUT       DDRB |= (1 << PB2)
#define PINMODE_PB3_OUTPUT       DDRB |= (1 << PB3)
#define PINMODE_PB4_OUTPUT       DDRB |= (1 << PB4)
#define PINMODE_PB5_OUTPUT       DDRB |= (1 << PB5)

volatile static uint8_t vTimerBit=0; 
static uint8_t gTimerSoftPwmPB0 = 0;
static uint8_t gTimerSoftPwmPB1 = 0;
static uint8_t gTimerSoftPwmPB2 = 0;
static uint8_t gTimerSoftPwmPB3 = 0;
static uint8_t gTimerSoftPwmPB4 = 0;
static uint8_t gTimerSoftPwmPB5 = 0;

TIMER_ON_OVERFLOW
{ 
  TIMER_OVERFLOW_40KHZ;
  vTimerBit+=2; // 7-Bit
  if (vTimerBit == 0) {
    uint8_t lPortB = 0;
    if ((DDRB & (1 << PB0)) > 0) {if (gTimerSoftPwmPB0 > 0) {lPortB |= (1 << PB0);} else {PORTB &= ~(1 << PB0);}}
    if ((DDRB & (1 << PB1)) > 0) {if (gTimerSoftPwmPB1 > 0) {lPortB |= (1 << PB1);} else {PORTB &= ~(1 << PB1);}}
    if ((DDRB & (1 << PB2)) > 0) {if (gTimerSoftPwmPB2 > 0) {lPortB |= (1 << PB2);} else {PORTB &= ~(1 << PB2);}}
    if ((DDRB & (1 << PB3)) > 0) {if (gTimerSoftPwmPB3 > 0) {lPortB |= (1 << PB3);} else {PORTB &= ~(1 << PB3);}}
    if ((DDRB & (1 << PB4)) > 0) {if (gTimerSoftPwmPB4 > 0) {lPortB |= (1 << PB4);} else {PORTB &= ~(1 << PB4);}}
    if ((DDRB & (1 << PB5)) > 0) {if (gTimerSoftPwmPB5 > 0) {lPortB |= (1 << PB5);} else {PORTB &= ~(1 << PB5);}}
    PORTB |= lPortB;
  } else if ((vTimerBit>>1) < 127) {
    if ((vTimerBit>>1) == (gTimerSoftPwmPB0>>1)) {PORTB &= ~(1 << PB0);}
    if ((vTimerBit>>1) == (gTimerSoftPwmPB1>>1)) {PORTB &= ~(1 << PB1);}
    if ((vTimerBit>>1) == (gTimerSoftPwmPB2>>1)) {PORTB &= ~(1 << PB2);}
    if ((vTimerBit>>1) == (gTimerSoftPwmPB3>>1)) {PORTB &= ~(1 << PB3);}
    if ((vTimerBit>>1) == (gTimerSoftPwmPB4>>1)) {PORTB &= ~(1 << PB4);}
    if ((vTimerBit>>1) == (gTimerSoftPwmPB5>>1)) {PORTB &= ~(1 << PB5);}
  }
}

int main() {
  CONFIG_CPU;
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;  
  TIMER_OVERFLOW_PRESCALE;
  TIMER_OVERFLOW_ENABLE;
  TIMER_OVERFLOW_START;

  while(1) {
    loop();
  }
}

void loop() {
  DELAY_100MS; gTimerSoftPwmPB4 = 0;    // off
  DELAY_100MS; gTimerSoftPwmPB4 = 15;   // 6.25% duty
  DELAY_2S;    gTimerSoftPwmPB4 = 31;   // 12.5% duty
  DELAY_2S;    gTimerSoftPwmPB4 = 63;   // 25% duty
  DELAY_2S;    gTimerSoftPwmPB4 = 127;  // 50% duty
  DELAY_2S;    gTimerSoftPwmPB4 = 191;  // 75% duty
  DELAY_2S;    gTimerSoftPwmPB4 = 255;  // on
}

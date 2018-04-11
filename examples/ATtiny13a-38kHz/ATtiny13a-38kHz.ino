// ATtiny13a 38 kHz for infrared led
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// This is a proof-of-concept to generate a 38 kHz PWM signal to drive a infrared led.
// There are different variants to do this with ATtiny13A. With 4.8 MHz and 
// no timer prescale you can generate a stable 38 kHz signal and 42% duty cycle.
// The minimum cpu frequency is 4.8 MHZ to get a stable signal close to 38 kHz.
// 
// 4.8 MHz 2.7mA@5V (4.8MHz) / TCNT0: 49.3% = 220+216 / 42.2% = 230+206 / Delay: 100ms => 293ms
// 4.8 MHz 2.7mA@5V (600kHz) / TCNT0: 47.3% = 251+250 / 39.5% = 253+249 / Delay: 100ms => 302ms
// 9.6 MHz 3.9mA@5V (4.8MHz) / TCNT0: 50.1% = 158+157 / 39.8% = 188/127
// 
// If you drive PWM by the background timer (multitasking), than loop process did not get all cpu time!
// You have to adjust your delay timing to the effective cpu time that can use inside loop void.
// With a timer prescale of 8 there is less cpu for the loop process (100ms takes 302ms).
//
// 2018-04-11 RoHa POC

#define F_CPU 4800000UL          // 4.8 MHz cpu clock
#include <avr/io.h>              // low level pinmode
#include <util/delay.h>          // low level delay

#define CONFIG_CPU               cli();CLKPR=(1<<CLKPCE);CLKPR=1;sei() // 1:4.8(2.7mA) 0:9.6(3.9mA)
#define DELAY_100MS              _delay_ms(389)
#define DELAY_100MS_ADJUST       _delay_ms(134)
#define TIMER_OVERFLOW_PRESCALE  TCCR0B |= (1<<CS00) // CS00:4.8MHz CS01:600kHz 
#define TIMER_OVERFLOW_ENABLE    TIMSK0 |= (1<<TOIE0) 
#define TIMER_OVERFLOW_START     sei()
#define TIMER_ON_OVERFLOW        ISR(TIM0_OVF_vect)
#define TIMER_OVERFLOW_1TICK     TCNT0 = 255
#define TIMER_OVERFLOW_3TICK     TCNT0 = 252
#define PINMODE_PB3_OUTPUT       DDRB |= (1 << PB3)
#define PINMODE_PB4_OUTPUT       DDRB |= (1 << PB4)
#define DIGITALWRITE_PB3_LOW     PORTB &= ~(1 << PB3)
#define DIGITALWRITE_PB4_LOW     PORTB &= ~(1 << PB4)
#define DIGITALWRITE_PB3_HIGH    PORTB |= (1 << PB3)
#define DIGITALWRITE_PB4_HIGH    PORTB |= (1 << PB4)
#define DIGITALWRITE_PB3_TOGGLE  PORTB ^= (1 << PB3)
#define DIGITALWRITE_PB4_TOGGLE  PORTB ^= (1 << PB4)

volatile static uint8_t vTimerSlot=0; 

TIMER_ON_OVERFLOW
{ 
  if (vTimerSlot == 0) {
    TCNT0 = 230; // 42% duty
    DIGITALWRITE_PB4_HIGH;
    vTimerSlot=1;
  } else {
    TCNT0 = 206; // 58% off
    DIGITALWRITE_PB4_LOW;
    vTimerSlot=0;
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
  DELAY_100MS_ADJUST;
  DIGITALWRITE_PB3_TOGGLE;
}

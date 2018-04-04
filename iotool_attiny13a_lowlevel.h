// Low level api to program ATtiny13a at Arduino IDE 1.6.x
// to reduce the binary code (maximum 1024 bytes).
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// A0/PB5 (1)  (8) VCC
// A3/PB3 (2)  (7) PB2/A1 
// A2/PB4 (3)  (6) PB1/PWM
//    GND (4)  (5) PB0/PWM
// 
// 2017-04-07  RoHa  v1.0 read/write pins, deep sleep
// 2017-04-24  RoHa  v1.1 pwm
// 2017-11-13  RoHa  v1.2 pb5/adc0
// 2018-04-04  RoHa  v1.3 timer, toggle

#ifndef IOTOOL_ATTINY13A_LOWLEVEL_H_
#define IOTOOL_ATTINY13A_LOWLEVEL_H_

#define _AVR_ATTINY13A_H_ 1  // iotn13a.h
#define F_CPU 1200000UL      // delay cpu clock
#include <avr/io.h>          // low level pinmode
#include <avr/interrupt.h>   // timer interrupt for sleep
#include <avr/sleep.h>       // sleep to reduce power
#include <util/delay.h>      // low level delay

// interrupt timer
#define SLEEPTIMER_16MS      WDTCR |= (0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_32MS      WDTCR |= (0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_64MS      WDTCR |= (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_125MS     WDTCR |= (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_250MS     WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_500MS     WDTCR |= (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_1S        WDTCR |= (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_2S        WDTCR |= (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_4S        WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0)
#define SLEEPTIMER_8S        WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0)
#define SLEEPTIMER_START     WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN)
#define SLEEPTIMER_SLEEP     sleep_mode()
#define OVERFLOWTIMER_213US  TCCR0B |= (1<<CS00) /* 256x 0.833us @ 1.2Mhz */ 
#define OVERFLOWCOUNT_25PCT  TCNT0 = 63          /* 160.00 us overflow interval */
#define OVERFLOWCOUNT_50PCT  TCNT0 = 127         /* 106.66 us overflow interval */
#define OVERFLOWCOUNT_75PCT  TCNT0 = 191         /*  53.33 us overflow interval */
#define OVERFLOWCOUNT_10US   TCNT0 = 12          /*  10.00 us overflow interval */
#define OVERFLOWCOUNT_13US   TCNT0 = 16          /*  13.33 us overflow interval */
#define OVERFLOWCOUNT_100US  TCNT0 = 120         /* 100.00 us overflow interval */
#define OVERFLOWCOUNT_125US  TCNT0 = 150         /* 125.00 us overflow interval */
#define TIMER_CLEAR          cli()
#define TIMER_START          sei()
#define TIMER_USE_OVERFLOW   TIMSK0 |= (1<<TOIE0) /* TIMSK0 |= _BV(TOIE0) */
#define TIMER_ON_WAKEUP      ISR(WDT_vect)
#define TIMER_ON_OVERFLOW    ISR(TIM0_OVF_vect)

// delay
#define DELAY_1MS    _delay_ms(1)
#define DELAY_5MS    _delay_ms(6)
#define DELAY_10MS   _delay_ms(12)
#define DELAY_50MS   _delay_ms(60)
#define DELAY_100MS  _delay_ms(120)
#define DELAY_320MS  _delay_ms(384)
#define DELAY_500MS  _delay_ms(600)
#define DELAY_1S     _delay_ms(1200)

// pinMode(PBx,INPUT/OUTPUT)
#define PINMODE_PB0_INPUT   DDRB &= ~(1 << PB0)
#define PINMODE_PB1_INPUT   DDRB &= ~(1 << PB1)
#define PINMODE_PB2_INPUT   DDRB &= ~(1 << PB2)
#define PINMODE_PB3_INPUT   DDRB &= ~(1 << PB3)
#define PINMODE_PB4_INPUT   DDRB &= ~(1 << PB4)
#define PINMODE_PB5_INPUT   DDRB &= ~(1 << PB5)
#define PINMODE_PB0_OUTPUT  DDRB |= (1 << PB0)
#define PINMODE_PB1_OUTPUT  DDRB |= (1 << PB1)
#define PINMODE_PB2_OUTPUT  DDRB |= (1 << PB2)
#define PINMODE_PB3_OUTPUT  DDRB |= (1 << PB3)
#define PINMODE_PB4_OUTPUT  DDRB |= (1 << PB4)
#define PINMODE_PB5_OUTPUT  DDRB |= (1 << PB5)

// digitalWrite(PBx,LOW/HIGH)
#define DIGITALWRITE_PB0_LOW     PORTB &= ~(1 << PB0)
#define DIGITALWRITE_PB1_LOW     PORTB &= ~(1 << PB1)
#define DIGITALWRITE_PB2_LOW     PORTB &= ~(1 << PB2)
#define DIGITALWRITE_PB3_LOW     PORTB &= ~(1 << PB3)
#define DIGITALWRITE_PB4_LOW     PORTB &= ~(1 << PB4)
#define DIGITALWRITE_PB5_LOW     PORTB &= ~(1 << PB5)
#define DIGITALWRITE_PB0_HIGH    PORTB |= (1 << PB0)
#define DIGITALWRITE_PB1_HIGH    PORTB |= (1 << PB1)
#define DIGITALWRITE_PB2_HIGH    PORTB |= (1 << PB2)
#define DIGITALWRITE_PB3_HIGH    PORTB |= (1 << PB3)
#define DIGITALWRITE_PB4_HIGH    PORTB |= (1 << PB4)
#define DIGITALWRITE_PB5_HIGH    PORTB |= (1 << PB5)
#define DIGITALWRITE_PB0_TOGGLE  PORTB ^= (1 << PB0)
#define DIGITALWRITE_PB1_TOGGLE  PORTB ^= (1 << PB1)
#define DIGITALWRITE_PB2_TOGGLE  PORTB ^= (1 << PB2)
#define DIGITALWRITE_PB3_TOGGLE  PORTB ^= (1 << PB3)
#define DIGITALWRITE_PB4_TOGGLE  PORTB ^= (1 << PB4)
#define DIGITALWRITE_PB5_TOGGLE  PORTB ^= (1 << PB5)

// digitalRead(PBx) == LOW
#define DIGITALREAD_PB0_LOW   (PINB & (1 << PB0))
#define DIGITALREAD_PB1_LOW   (PINB & (1 << PB1))
#define DIGITALREAD_PB2_LOW   (PINB & (1 << PB2))
#define DIGITALREAD_PB3_LOW   (PINB & (1 << PB3))
#define DIGITALREAD_PB4_LOW   (PINB & (1 << PB4))
#define DIGITALREAD_PB5_LOW   (PINB & (1 << PB5))
#define DIGITALREAD_PB0_HIGH  !(PINB & (1 << PB0))
#define DIGITALREAD_PB1_HIGH  !(PINB & (1 << PB1))
#define DIGITALREAD_PB2_HIGH  !(PINB & (1 << PB2))
#define DIGITALREAD_PB3_HIGH  !(PINB & (1 << PB3))
#define DIGITALREAD_PB4_HIGH  !(PINB & (1 << PB4))
#define DIGITALREAD_PB5_HIGH  !(PINB & (1 << PB5))

// analog digital converter
#define ADC_DISABLE       ADCSRA &= ~(1 << ADEN)
#define ADC_ENABLE        ADCSRA |= (1 << ADEN); ADCSRA |= (1 << ADSC); while (ADCSRA & (1 << ADSC))
#define ADC_CLEAR         ADMUX &= ~((1 << MUX1)|(1 << MUX0))
#define ADC0_PB5_SELECT   ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (0 << MUX1)|(0 << MUX0)
#define ADC1_PB2_SELECT   ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (0 << MUX1)|(1 << MUX0)
#define ADC2_PB4_SELECT   ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (1 << MUX1)|(0 << MUX0)
#define ADC3_PB3_SELECT   ADMUX |= (0 << REFS0); ADMUX &= ~((1 << MUX1)|(1 << MUX0)); ADMUX |= (1 << MUX1)|(1 << MUX0)

// analog write pwm
#define PWM_ENABLE       TCCR0B |= (1 << CS01) ; TCCR0A |= (1 << WGM01) | (1 << WGM00)
#define PWM0_PB0_SELECT  TCCR0A |= (1 << COM0A1)
#define PWM1_PB1_SELECT  TCCR0A |= (1 << COM0B1)

// analog read
uint16_t readADC(uint8_t n) 
{
  uint16_t rcAdc = 0;
  for (uint8_t i=0; i<n; i++) 
  {                                // max 8 rounds
    ADCSRA |= (1 << ADSC);         // start converstion
    DELAY_1MS;                     // wait for Vref to settle
    while((ADCSRA & 0x40) !=0){};  // wait for conv complete
    rcAdc += ADC;
  }
  rcAdc /= n;
  return rcAdc;
}

// analog write
void writePWM0(int val)
{
    OCR0A = val;
}

void writePWM1(int val)
{
    OCR0B = val;
}

#endif // IOTOOL_ATTINY13A_LOWLEVEL_H_

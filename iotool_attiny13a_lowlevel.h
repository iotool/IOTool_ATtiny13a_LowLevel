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
// 2018-04-08  RoHa  v1.3b cpu frequency
// 2018-04-13  RoHa  v1.3c reset memory
// 2018-09-29  RoHa  v1.4 pwm on/off
// 2019-03-21  RoHa  v1.5 pwm ovrfl 600kHz cpu
// 2019-03-21  RoHa  v1.5b timer comp

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
#define OVERFLOWTIMER_1X2MHZ TCCR0B |= (1<<CS00)             /* 177.50 us 256x 0.833us @ 1.2Mhz */ 
#define OVERFLOWTIMER_150KHZ TCCR0B |= (1<<CS01)             /* 1706.6 us 256x 6.666us @ 1.2Mhz */ 
#define OVERFLOWCOUNT_25PCT  TCNT0 = 63                      /* 160.00 us overflow interval */
#define OVERFLOWCOUNT_50PCT  TCNT0 = 127                     /* 106.66 us overflow interval */
#define OVERFLOWCOUNT_75PCT  TCNT0 = 191                     /*  53.33 us overflow interval */
#define OVERFLOWCOUNT_10US   TCNT0 = 244                     /*  10.00 us overflow interval 255-12+1  */
#define OVERFLOWCOUNT_13US   TCNT0 = 240                     /*  13.33 us overflow interval 255-16+1  */
#define OVERFLOWCOUNT_20US   TCNT0 = 232                     /*  20.00 us overflow interval 255-24+1  */
#define OVERFLOWCOUNT_20US   TCNT0 = 226                     /*  25.00 us overflow interval 255-30+1  */
#define OVERFLOWCOUNT_100US  TCNT0 = 136                     /* 100.00 us overflow interval 255-120+1 */
#define OVERFLOWCOUNT_125US  TCNT0 = 106                     /* 125.00 us overflow interval 255-150+1 */
#define OVRFLTI600KHZ_2X3KHZ TCCR0B |= (1<<CS00)             /* 426.66 us 256x 1.666us @ 600kHz 001 = 1[1] */ 
#define OVRFLTI600KHZ_586HZ  TCCR0B |= (1<<CS01)             /*   1706 us 256x 6.666us @ 600kHz 010 = 2[8] */ 
#define OVRFLCNT2X3KHZ_10US  TCNT0 = 250                     /*     10 us overflow interval 255-6+1  (6x1.6666) */
#define OVRFLCNT2X3KHZ_15US  TCNT0 = 247                     /*     15 us overflow interval 255-9+1  (9x1.6666) */
#define OVRFLCNT2X3KHZ_20US  TCNT0 = 244                     /*     20 us overflow interval 255-12+1 (12x1.6666) */
#define OVRFLCNT2X3KHZ_25US  TCNT0 = 241                     /*     25 us overflow interval 255-15+1 (15x1.6666) */
#define OVRFLCNT2X3KHZ_50US  TCNT0 = 226                     /*     50 us overflow interval 255-30+1 (30x1.6666) */
#define OVRFLCNT2X3KHZ_100US TCNT0 = 196                     /*    100 us overflow interval 255-60+1 (60x1.6666) */
#define OVRFLCNT2X3KHZ_125US TCNT0 = 181                     /*    125 us overflow interval 255-75+1 (75x1.6666) */
#define TIMER_CLEAR          cli()                           /* timer stop */
#define TIMER_START          sei()                           /* timer start */
#define TIMER_USE_OVERFLOW   TIMSK0 |= (1<<TOIE0) /* TIMSK0 |= _BV(TOIE0) */
#define TIMER_USE_COMP_CLEAR TCCR0A = (1<<WGM01)             /* Clear on Compare Match */
#define TIMER_USE_COMP_KEEP  TCCR0A = ((1<<WGM01)|(1<<WGM00))/* Set on Compare Match  */
#define TIMER_USE_COMPARE_A  TIMSK0 |= (1 << OCIE0A)
#define TIMER_USE_COMPARE_B  TIMSK0 |= (1 << OCIE0B)
#define TIMER_ON_WAKEUP      ISR(WDT_vect)
#define TIMER_ON_OVERFLOW    ISR(TIM0_OVF_vect)
#define TIMER_ON_COMPARE_A   ISR(TIM0_COMPA_vect)
#define TIMER_ON_COMPARE_B   ISR(TIM0_COMPB_vect)
#define TIMER_SET_COUNTER    TCNT0
#define TIMER_SET_COMPARE_A  OCR0A
#define TIMER_SET_COMPARE_B  OCR0B

// delay @ 1.2 MHz
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
#define PWM0_PB0_SELECT  TCCR0A |= (1 << COM0A1)
#define PWM_OFF          DDRB &= ~((1 << PB1)|(1 << PB0))
#define PWM0_PB0_OFF     DDRB &= ~(1 << PB0)
#define PWM1_PB1_OFF     DDRB &= ~(1 << PB1)
#define PWM_ON           DDRB |= (1 << PB1)|(1 << PB0)
#define PWM0_PB0_ON      DDRB |= (1 << PB0)
#define PWM1_PB1_ON      DDRB |= (1 << PB1)

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

// cpu frequency
#define CHANGE_CPU_9MHZ6  cli();CLKPR=(1<<CLKPCE);CLKPR=0;sei(); // 4.7 mA @5V
#define CHANGE_CPU_4MHZ8  cli();CLKPR=(1<<CLKPCE);CLKPR=1;sei(); // 3.6 mA @5V
#define CHANGE_CPU_2MHZ4  cli();CLKPR=(1<<CLKPCE);CLKPR=2;sei(); // 2.3 mA @5V
#define CHANGE_CPU_1MHZ2  cli();CLKPR=(1<<CLKPCE);CLKPR=3;sei(); // 1.7 mA @5V
#define CHANGE_CPU_600KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=4;sei(); // 1.2 mA @5V
#define CHANGE_CPU_300KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=5;sei(); // 1.0 mA @5V
#define CHANGE_CPU_150KHZ cli();CLKPR=(1<<CLKPCE);CLKPR=6;sei(); // 0.9 mA @5V

// adc frequency 150 kHz
#define CHANGE_ADC_9MHZ6  ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=(1<<ADPS0)
#define CHANGE_ADC_4MHZ8  ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=(1<<ADPS1)
#define CHANGE_ADC_2MHZ4  ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=((1<<ADPS1)|(1<<ADPS0))
#define CHANGE_ADC_1MHZ2  ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=(1<<ADPS2)
#define CHANGE_ADC_600KHZ ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=((2<<ADPS2)|(0<<ADPS0))
#define CHANGE_ADC_300KHZ ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=((2<<ADPS2)|(1<<ADPS1))
#define CHANGE_ADC_150KHZ ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=((2<<ADPS1)|(1<<ADPS1)|(1<<ADPS0))

// delay
#define DELAY_1S_9MHZ6      _delay_ms(9600)
#define DELAY_1S_4MHZ8      _delay_ms(4800)
#define DELAY_1S_2MHZ4      _delay_ms(2400)
#define DELAY_1S_1MHZ2      _delay_ms(1200)
#define DELAY_1S_600KHZ     _delay_ms(600)
#define DELAY_1S_300KHZ     _delay_ms(300)
#define DELAY_1S_150KHZ     _delay_ms(150)
#define DELAY_100MS_9MHZ6   _delay_ms(960)
#define DELAY_100MS_4MHZ8   _delay_ms(480)
#define DELAY_100MS_2MHZ4   _delay_ms(240)
#define DELAY_100MS_1MHZ2   _delay_ms(120)
#define DELAY_100MS_600KHZ  _delay_ms(60)
#define DELAY_100MS_300KHZ  _delay_ms(30)
#define DELAY_100MS_150KHZ  _delay_ms(15)

#define RESET_BUTTON_MAGIC  0xDE49

#define USE_RESET_BUTTON                    \
  typedef struct {                          \
    uint16_t crc;                           \
    uint8_t  cnt;                           \
    unsigned rst:1;                         \
    unsigned :7;                            \
    uint32_t dat;                           \
  } tRstMem;                                \
  tRstMem gRstMem __attribute__             \
    ((__section__ (".noinit")))             \
  ;

#define INIT_RESET_BUTTON                   \
  if (gRstMem.crc != RESET_BUTTON_MAGIC) {  \
    gRstMem.crc = RESET_BUTTON_MAGIC;       \
    gRstMem.cnt = 0;                        \
    gRstMem.rst = 0;                        \
    gRstMem.dat = 0;                        \
  } else {                                  \
    gRstMem.cnt++;                          \
  }

#endif // IOTOOL_ATTINY13A_LOWLEVEL_H_


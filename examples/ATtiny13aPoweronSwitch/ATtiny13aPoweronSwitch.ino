// ATtiny13a PowerOn-Switch
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// Poweron run the last selected mode (write flag 01 to eeprom).
// If you turn off turing the first second the next mode selected.
// After 1 second the timeout reached (update flag to 11 at eeprom).
// This sketch use software pwm to dimm all pins pb[0..4].
// 
//        1  8-- VCC
// LED1 --2  7
// LED2 --3  6
//  GND --4  5
// 
// Change mode by poweron switch.
// LED1 dimmed 0%, 50%, 90% by software pwm
// LED2 display toogle mode.
//
// 2018-04-10 RoHa POC

#define F_CPU 1200000UL         // 1.2 MHz cpu clock
#include <avr/io.h>             // low level pinmode
#include <util/delay.h>         // low level delay
#include <EEPROM.h>             // persist data

#define CONFIG_CPU               cli();CLKPR=(1<<CLKPCE);CLKPR=3;sei()
#define CONFIG_ADC               ADCSRA|=((1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0));ADCSRA^=(1<<ADPS2)
#define DELAY_1S                 _delay_ms(1200) 
#define DELAY_100MS              _delay_ms(120)
#define DELAY_20MS               _delay_ms(24)
#define PINMODE_PB3_OUTPUT       DDRB |= (1 << PB3)
#define PINMODE_PB4_OUTPUT       DDRB |= (1 << PB4)
#define DIGITALWRITE_PB3_LOW     PORTB &= ~(1 << PB3)
#define DIGITALWRITE_PB4_LOW     PORTB &= ~(1 << PB4)
#define DIGITALWRITE_PB3_HIGH    PORTB |= (1 << PB3)
#define DIGITALWRITE_PB4_HIGH    PORTB |= (1 << PB4)
#define DIGITALWRITE_PB3_TOGGLE  PORTB ^= (1 << PB3)
#define DIGITALWRITE_PB4_TOGGLE  PORTB ^= (1 << PB4)
#define TIMER_OVERFLOW_PRESCALE  TCCR0B |= (1<<CS01)  // 150 kHz
#define TIMER_OVERFLOW_ENABLE    TIMSK0 |= (1<<TOIE0) 
#define TIMER_OVERFLOW_START     sei()
#define TIMER_OVERFLOW_65HZ      TCNT0 = 247          // 65 HZ
#define TIMER_ON_OVERFLOW        ISR(TIM0_OVF_vect)

// --- Software PWM ---

volatile static uint32_t vTimerMicros = 0;
volatile static uint8_t  vTimerSoftPwmCount = 0;
volatile static uint8_t  vTimerSoftPwmPB3 = 0;
volatile static uint8_t  vTimerSoftPwmPB4 = 0;

TIMER_ON_OVERFLOW
{  
  TIMER_OVERFLOW_65HZ;
  vTimerMicros += 60;
  vTimerSoftPwmCount++;
  if (vTimerSoftPwmCount == 0) {
    if (vTimerSoftPwmPB3 > 0) {
      DIGITALWRITE_PB3_HIGH;
    } else {
      DIGITALWRITE_PB3_LOW;    
    }
    if (vTimerSoftPwmPB4 > 0) {
      DIGITALWRITE_PB4_HIGH;
    } else {
      DIGITALWRITE_PB4_LOW;
    }
  } else {
    if (vTimerSoftPwmCount == vTimerSoftPwmPB3) {
      DIGITALWRITE_PB3_LOW;
    }
    if (vTimerSoftPwmCount == vTimerSoftPwmPB4) {
      DIGITALWRITE_PB4_LOW;
    }
  }
}

// --- Persist EEPROM ---

static uint8_t  gEepromMode = 0;
static uint8_t  gEepromState = 0;
static uint32_t gEepromCounter = 0;
static uint32_t gEepromTimeout = 0;

void ReadEepromMode() 
{
  // signal led on
  vTimerSoftPwmPB4 = 100;
  // read latest values
  do {} while (!eeprom_is_ready());
  gEepromCounter = eeprom_read_dword((uint32_t*)0);
  gEepromTimeout = eeprom_read_dword((uint32_t*)4);
  // next mode if timeout exit was not reached
  if (gEepromTimeout == 0x00000001) {
    gEepromCounter++;
    eeprom_write_dword((uint32_t*)0,(uint32_t)gEepromCounter);
  }
  gEepromMode = (gEepromCounter % 3);
  // timeout flag = init
  gEepromTimeout = 0x00000001;
  eeprom_write_dword((uint32_t*)4,(uint32_t)gEepromTimeout);
}

void WriteEepromMode() 
{
  if (gEepromState == 0) {
    if (vTimerMicros > 1000000) {
      // end of timeout after 1 second
      gEepromState = 1;
      // signal led off
      vTimerSoftPwmPB4 = 0;
      // timeout flag = exit
      do {} while (!eeprom_is_ready());
      gEepromTimeout = 0x00000002;
      eeprom_write_dword((uint32_t*)4,(uint32_t)gEepromTimeout);
    }
  }
}

int main() 
{
  // --- Setup ---

  CONFIG_CPU;
  CONFIG_ADC;
  PINMODE_PB3_OUTPUT;
  PINMODE_PB4_OUTPUT;
  TIMER_OVERFLOW_PRESCALE;
  TIMER_OVERFLOW_ENABLE;
  TIMER_OVERFLOW_START;

  // --- Loop ---

  ReadEepromMode();
  while(1) 
  {
    loop();
    WriteEepromMode();
  }
}

void loop() 
{
  switch(gEepromMode) {
    case 0:
      // 100%
      vTimerSoftPwmPB3 = 100;
      break;
    case 1:
      // dimmer 50%
      vTimerSoftPwmPB3 = 50;
      break;
    case 2:
      // dimmer 10%
      vTimerSoftPwmPB3 = 10;
      break;
  }
  DELAY_100MS;
}

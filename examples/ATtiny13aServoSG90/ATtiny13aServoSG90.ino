// Example for usage low level api to write pins
// to control a SG90 stepper motor.
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// PIN2 --[200]-- LED -- GND----(-)
// PIN4 ---GND--- SG90.brown
// PIN5 --[220]-- SG90.orange
// PIN8 ---VCC--- SG90.red -----(+)
// 
// 2017-04-25  RoHa  240 byte ROM (23%), 0 byte RAM (0%) 

#include "iotool_attiny13a_lowlevel.h"

int main() 
{
  // --- setup ---
  PINMODE_PB0_OUTPUT; DIGITALWRITE_PB0_LOW;
  PINMODE_PB1_OUTPUT; DIGITALWRITE_PB1_LOW;
  PINMODE_PB2_OUTPUT; DIGITALWRITE_PB2_LOW;
  PINMODE_PB3_OUTPUT; DIGITALWRITE_PB3_LOW;
  PINMODE_PB4_OUTPUT; DIGITALWRITE_PB4_LOW;
  ADC_DISABLE;

  // --- loop ---
  while(1)
  {
    loop();
  }
}

void loop() 
{
  // blink led
  DIGITALWRITE_PB4_HIGH;
  DELAY_1S; 
  DIGITALWRITE_PB4_LOW;
  
  // 0 degree 20000-1.2MHz(2000us)
  for (byte i = 0; i < 30; i++) { 
    DIGITALWRITE_PB0_HIGH;
    _delay_us(2400);
    DIGITALWRITE_PB0_LOW;
    _delay_us(17600);
  }
  DELAY_1S; 
  
  // 90 degree 20000-1.2MHz(1500us)
  for (byte i = 0; i < 30; i++) { 
    DIGITALWRITE_PB0_HIGH;
    _delay_us(1500);
    DIGITALWRITE_PB0_LOW;
    _delay_us(18500);
  }
  DELAY_1S; 
  
  // 180 degree 20000-1.2MHz(1000us)
  for (byte i = 0; i < 30; i++) { 
    DIGITALWRITE_PB0_HIGH;
    _delay_us(600);
    DIGITALWRITE_PB0_LOW;
    _delay_us(19400);
  }
  DELAY_1S; 
}
// Example for usage low level api to write pins
// to control a 28BYJ-48 stepper motor by UNL2003-driver.
// 
// https://github.com/iotool/IOTool_ATtiny13a_LowLevel
// 
// PIN2(3) ---UNL-Stepper.IN4
// PIN3(4) ---LED
// PIN4    ---UNL-Stepper.GND
// PIN5(0) ---UNL-Stepper.IN1
// PIN6(1) ---UNL-Stepper.IN2
// PIN7(2) ---UNL-Stepper.IN3
// PIN8    ---UNL-Stepper.VCC
// 
// 2000us = 17.4 rpm (minimum 1800us half step)
// 
// 2017-04-25  RoHa  332 byte ROM (23%), 0 byte RAM (0%) 

#include "iotool_attiny13a_lowlevel.h"
#define STEPPER_SLEEP _delay_us(2000)

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

void stepperOff() {
  DIGITALWRITE_PB0_LOW;
  DIGITALWRITE_PB1_LOW;
  DIGITALWRITE_PB2_LOW;
  DIGITALWRITE_PB3_LOW;  
}

void stepper1() {
  DIGITALWRITE_PB3_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB3_LOW;
}

void stepper2() {
  DIGITALWRITE_PB3_HIGH;
  DIGITALWRITE_PB2_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB3_LOW;
  DIGITALWRITE_PB2_LOW;
}

void stepper3() {
  DIGITALWRITE_PB2_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB2_LOW;
}

void stepper4() {
  DIGITALWRITE_PB1_HIGH;
  DIGITALWRITE_PB2_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB1_LOW;
  DIGITALWRITE_PB2_LOW;
}

void stepper5() {
  DIGITALWRITE_PB1_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB1_LOW;
}

void stepper6() {
  DIGITALWRITE_PB0_HIGH;
  DIGITALWRITE_PB1_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB0_LOW;
  DIGITALWRITE_PB1_LOW;
}

void stepper7() {
  DIGITALWRITE_PB0_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB0_LOW;
}

void stepper8() {
  DIGITALWRITE_PB3_HIGH;
  DIGITALWRITE_PB0_HIGH;
  STEPPER_SLEEP;
  DIGITALWRITE_PB3_LOW;
  DIGITALWRITE_PB0_LOW;
}

void loop() {
  // blink led
  DIGITALWRITE_PB4_HIGH;
  DELAY_1S; 
  DIGITALWRITE_PB4_LOW;

  // half step (double speed)
  for (uint16_t i = 0; i < 512; i++) { 
    stepper2();
    stepper4();
    stepper6();
    stepper8();
  }
  DELAY_1S; 

  // full step (regular speed)
  for (uint16_t i = 0; i < 512; i++) { 
    stepper1();
    stepper2();
    stepper3();
    stepper4();
    stepper5();
    stepper6();
    stepper7();
    stepper8();
  }
  DELAY_1S; 
}
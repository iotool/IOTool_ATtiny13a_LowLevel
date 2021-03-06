# IOTool_ATtiny13a_LowLevel

Low level api to program ATtiny13a at Arduino IDE 1.6.x
Reduce binary code and current consumption

## ATtiny13a Examples

* gpio input and output pins (ATtiny13aPins / 402 byte)
* blink led (ATtiny13aBlink / 94 byte)
* blink led without delay (ATtiny13aTimer0 / 168 byte)
* NEC infrared receiver (ATtiny13aIRrecv / 784 byte)
* led morse code output (ATtiny13aMorse / 448 byte)
* servo motor SG90 (ATtiny13aServoSG90 / 240 byte)
* stepper motor 28BYJ-48 (ATtiny13aStepper28BYJ48 / 332 byte)
* 38 kHz pulse generator to drive infrared led
* software pwm with 50 Hz for multi channel gpio pin
* poweron switch (toggle mode by power on/off)

## Arduino IDE

https://www.instructables.com/id/Updated-Guide-on-How-to-Program-an-Attiny13-or-13a/
https://raw.githubusercontent.com/sleemanj/optiboot/master/dists/package_gogo_diy_attiny_index.json

## ATtiny13A Specs

* 20mA @5V (10mA @3V) continuous current PB0:1:5 (PWM,RST)
* 10mA @5V (5mA @3V) continuous current PB2:3:4 (ADC)
* 40mA @5V (20mA @3V) maximum peak 100ns current PB0:1:5 (PWM,RST)
* 20mA @5V (10mA @3V) maximum peak 100ns current PB2:3:4 (ADC)
* 60mA @5V output_lo sum current PB0+1+2+3+4+5
* 60mA @5V output_hi sum current PB0+1+2+3+4+5
* 120mA @5V output_hi + output_lo sum current

Example 40mA : PB0(HI) = 20mA LED to GND + PB1(LO) = 20mA LED to VCC

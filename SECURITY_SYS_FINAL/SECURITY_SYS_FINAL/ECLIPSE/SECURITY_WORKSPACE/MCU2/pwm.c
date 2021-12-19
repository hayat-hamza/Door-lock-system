/*
 * pwm.c
 *
 *  Created on: Oct 15, 2021
 *      Author: Hayat Hussein
 */

#include "pwm.h"
#include "common_macros.h" /* To use the macros like SET_BIT */
#include "avr/io.h" /* To use the IO Ports Registers */
#include "gpio.h"
void PWM_Timer0_Start(uint8 duty_cycle){
	/*
	 * initial value of the counter in TCNT0=0
	 */
	TCNT0=0;
	/*in reg TCCR0 using timer1
	 *FOC0=0 to specify pwm mode
	 *WGM00 and WGM01 are both=1 to specify fast pwm
	 *COM01=1 AND COM00=0 to specify non inverting mode
	 *CS00=0 CS01=1 CS02=0 TO specify 8 prescaler
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
	/*
	 * set port b pin 3 as output as this is where oc0 is generated
	 */
	GPIO_setupPinDirection(PORTB_ID,PIN3_ID,PIN_OUTPUT);
	/*
	 * set duty cycle by taking it as a percintage then
	 * putting the count value in ocr0
	 */
	duty_cycle=(float)duty_cycle/100*MAX_TICKS;
	OCR0=duty_cycle;
}

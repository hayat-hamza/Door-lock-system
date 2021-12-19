/*
 * buzzer.c
 *
 *  Created on: Nov 6, 2021
 *      Author: Hayat Hussein
 */

#include "buzzer.h"
#include "gpio.h"
#include "timer0.h"

void BUZZER_init(void){
	GPIO_setupPinDirection(BUZZER_PORT,BUZZER_PIN,PIN_OUTPUT);
}

void BUZZER_on(void)		/*buzzer on*/
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,1);
}

void BUZZER_off(void)		/*buzzer off*/
{
	GPIO_writePin(BUZZER_PORT,BUZZER_PIN,0);
}

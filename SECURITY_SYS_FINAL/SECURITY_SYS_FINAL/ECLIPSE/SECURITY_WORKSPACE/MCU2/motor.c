/*
 * motor.c
 *
 *  Created on: Oct 15, 2021
 *      Author: Hayat Hussein
 */


#include "motor.h"
//#include "lcd.h"
#include "gpio.h"

void DcMotor_Init(void){
	/*
	 * Description
 	 The Function responsible for setup the direction for the two
	 motor pins through the GPIO driver.
	 */
	GPIO_setupPinDirection(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_OUTPUT_PORT,MOTOR_SECOND_PIN,PIN_OUTPUT);
/*
 *  	 Stop at the DC-Motor at the beginning through the GPIO driver
 *  	 by writing 0 at both bins
 */
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,LOGICAL_ZERO);
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,LOGICAL_ZERO);

}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	/*
	 * determine the direction of the fan
	 */
	if (state==CW){
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,LOGICAL_ONE);
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_SECOND_PIN,LOGICAL_ZERO);
	}
	else if(state==A_CW){
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,LOGICAL_ZERO);
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_SECOND_PIN,LOGICAL_ONE);
	}
	else if(state==STOP){
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_FIRST_PIN,LOGICAL_ZERO);
	GPIO_writePin(MOTOR_OUTPUT_PORT,MOTOR_SECOND_PIN,LOGICAL_ZERO);
	}
	/*
	 * determine the speed of the fan and display if its on or off
	 */
	if(speed<25){
		//PWM_Timer0_Start(0);
		//LCD_displayStringRowColumn(0,10,"OFF");
	}

	else if((speed>=25)&&(speed<50)){
		//PWM_Timer0_Start(25);
		//LCD_displayStringRowColumn(0,10,"ON");
		//LCD_displayStringRowColumn(0,12," ");		/*the space is here to prevent displaying "ONF"*/

	}

	else if((speed>=50)&&(speed<75)){
		//PWM_Timer0_Start(50);
		//LCD_displayStringRowColumn(0,10,"ON");
		//LCD_displayStringRowColumn(0,12," ");

	}

	else if((speed>=75)&&(speed<100)){
	//	PWM_Timer0_Start(75);
		//LCD_displayStringRowColumn(0,10,"ON");
		//LCD_displayStringRowColumn(0,12," ");

	}
	else if(speed>=100){
		//PWM_Timer0_Start(100);
		//LCD_displayStringRowColumn(0,10,"ON");
	//	LCD_displayStringRowColumn(0,12," ");
		GPIO_writePin(PORTB_ID,PIN3_ID,1);
	}

}

/*
 * pwm.h
 *
 *  Created on: Oct 15, 2021
 *      Author: Hayat Hussein
 */

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"
/*******************************************************************************
 *                            Definitions and Enums                               *
 *******************************************************************************/

#define 	MAX_TICKS			 255
/*******************************************************************************
 *                     		 Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the PWM using timer0.
 */
void PWM_Timer0_Start(uint8 duty_cycle);
#endif /* PWM_H_ */

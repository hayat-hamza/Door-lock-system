/*
 * motor.h
 *
 *  Created on: Oct 15, 2021
 *      Author: Hayat Hussein
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include "std_types.h"
/*******************************************************************************
 *                                Definitions and Enums                               *
 *******************************************************************************/
#define MOTOR_OUTPUT_PORT PORTC_ID
#define MOTOR_FIRST_PIN   PIN2_ID
#define MOTOR_SECOND_PIN   PIN3_ID
#define LOGICAL_ONE 1
#define LOGICAL_ZERO 0

enum STATE {STOP,CW,A_CW};

/*******************************************************************************
 *                    			  new data types                                 *
 *******************************************************************************/
typedef uint8  DcMotor_State;

/*******************************************************************************
 *                     		 Functions Prototypes                                   *
 *******************************************************************************/
/*
 * Description :
 * Function responsible for initialize the motor and making it initially stopped.
 */
void DcMotor_Init(void);
/*
 * Description :
 * Function responsible for rotating the motor and derterminig the direction of rotation
 * and its speed based on temp
 */
void DcMotor_Rotate(DcMotor_State state,uint8 speed);
#endif /* MOTOR_H_ */

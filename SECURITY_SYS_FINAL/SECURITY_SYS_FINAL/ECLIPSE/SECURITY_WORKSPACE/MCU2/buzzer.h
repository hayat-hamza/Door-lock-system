/*
 * buzzer.h
 *
 *  Created on: Nov 6, 2021
 *      Author: Hayat Hussein
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "std_types.h"
/*******************************************************************************
 *                      Definitions                                   *
 *******************************************************************************/
#define BUZZER_PORT PORTA_ID
#define BUZZER_PIN PIN5_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/
void BUZZER_init(void);		/*sets the pin as output*/
void BUZZER_on(void);		/*buzzer on*/
void BUZZER_off(void);		/*buzzer off*/



#endif /* BUZZER_H_ */

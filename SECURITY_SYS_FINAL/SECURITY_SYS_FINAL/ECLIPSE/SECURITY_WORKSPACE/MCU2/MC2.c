/*
 ================================================================================================
 Name        : Exercise1.c
 Author      : Mohamed Tarek
 Description : Exercise to test the UART functionality
 Date        : 4/11/2014
 ================================================================================================
 */

#include "uart.h"
#include "buzzer.h"
#include "timer0.h"
#include "motor.h"
#include "external_eeprom.h"
#include "twi.h"
#include <avr/interrupt.h>
#include <util/delay.h>

/*******************************************************************************
 *                      	Structures and Enums                                   *
 *******************************************************************************/
#define MC2_READY 0x10
#define F_CPU 8000000
typedef enum
{
	WRONG,RIGHT
}pass_correctness;

typedef enum
{
	OPEN_DOOR='+',CHANGE_PASSWORD='-'
}user_choices;
/*******************************************************************************
 *                      	Global Variables                                   *
 *******************************************************************************/
uint8 g_received_password[5];	/*this will hold 5 digit password*/
uint8 g_received_re_entered_password[5];	/*HOLDS THE DATA after entering it twice*/

uint32 count1=0;
Timer0_ConfigType timer0_configration;		/*timer0 configration*/

/*******************************************************************************
 *                    	  Functions Prototypes                                   *
 *******************************************************************************/
/*
 * **********************function discription****************
 * cheks if the password entered the first and second time is the same
 */
uint8 checkIfCorrect(uint8 pass[],uint8 re_entered_pass[]);
/*
 * **********************function discription****************
 * receives the password from MCU1
 */
void receivePasswordUsingUart(uint8 received_pass[]);
/*
 * **********************function discription****************
 * gits the password back from the eeprom
 */
void retreivePassword(uint8 password[]);
/*
 * **********************function discription****************
 * saves the password in eeprom
 */
void savePassword(uint8 password[]);
/*
 * **********************function discription****************
 * if the password is wrong it asks MCU1 to send it again
 * if it's right it calls savePassword function to save it and do some proccessing
 * it also tells MC2 if the password is right or wrong
 * and oens the door and turns on and off the buzzer
 */
void rightOrWrongProccessing(void);
/*
 * **********************function discription****************
 * incriments the count
 */
void Timer0_count1(void);
int main(void)
{
	uint8 i=0;

	UART_ConfigType uart_configration;
	uart_configration.baud_rate=9600;		/*uart configration*/
	uart_configration.data_bits_number=Eight;
	uart_configration.parity=Disabled;
	uart_configration.stop_bit=One;

	TWI_ConfigType twi_configration;		/*i2c configration*/
	twi_configration.address=0b00000010;
	twi_configration.bit_rate=400000;


	UART_init(&uart_configration);		/*uart configration*/
	TWI_init(&twi_configration);

	timer0_configration.initial_value=0;
	timer0_configration.timer_mode=NORMAL_MODE;
	timer0_configration.prescaler=FCPU_1024;
	Timer0_setCallBack(Timer0_count1);

	DcMotor_Init();
	BUZZER_init();

	while(1)
	{

		/*
		 * receive password using uart
		 */
		i=0;
		while(i<5){
			g_received_password[i]=UART_recieveByte();
			i++;
		}
		/*
		 * receive re entered password using uart
		 * and put into another array
		 */
		i=0;
		while(i<5){
			g_received_re_entered_password[i]=UART_recieveByte();
			i++;
		}
		rightOrWrongProccessing();

	}
	return 0;
}


uint8 checkIfCorrect(uint8 g_password[],uint8 re_entered_password[])
{
	uint8 count=0;
	uint8 correctness=RIGHT;
	/*
	 * if one of the chars is not correct then the password is wrong
	 */
	while(count<5){
		if(g_received_re_entered_password[count]!=g_received_password[count])
		{
			correctness= WRONG;
		}
		count++;
	}

	return correctness;
}
void receivePasswordUsingUart(uint8 received_pass[])
{
	uint8 count;
	while(count<5){
		received_pass[count]=UART_recieveByte();
		count++;
	}
}

void savePassword(uint8 password[])
{
	/*
	 * password is saved byte by byte
	 */
	uint8 i=0;
	while(i<5)
	{
		EEPROM_writeByte(0x0311, password[i]); /* Write password in the external EEPROM */
		i++;
	}


}
void retreivePassword(uint8 password[])
{
	uint8 i=0;
	/*
	 * password is retrieved byte by byte
	 */
	while(i<5)
	{
		EEPROM_readByte(0x0311, &password[i]);  /* Read password from the external EEPROM */
		i++;
	}
}

void rightOrWrongProccessing(void)
{
	uint8 pass_check;			/*checks if passwrod is right or wrong*/
	static uint8 trials=1;		/*the number of password trials that the user is allowed to make*/
	uint8 password_from_mem;			/*password retrieve from memory is stored here*/
	pass_check=checkIfCorrect(g_received_password,g_received_re_entered_password);	//compare

	if(pass_check==WRONG)
	{
		/*
		 * W indicates wrong answer and that the password iis not saved in eeprom yet
		 */
		UART_sendByte('W');
	}
	if(pass_check==RIGHT)
	{
		savePassword(g_received_password);		/*save the password in eeprom*/
		UART_sendByte('R');			/*R indicates that the password is right*/
		_delay_ms(500);
		/*
		 * tell MCU1 to display this strings and take input from user
		 */
		UART_sendString("+:OpenDoor#");
		_delay_ms(500);
		UART_sendString("-:ChangePass#");
		uint8 byte;
		byte=UART_recieveByte();		/*receive + or - */

		/*
		 * if user wants to open door get saved pass from eeprom and
		 *  take the password from him
		 * the user gits to make only one mistake
		 */
		if(byte=='+')
		{
			retreivePassword(&password_from_mem);
			UART_sendByte(MC2_READY);
			_delay_ms(10000);
			receivePasswordUsingUart(g_received_password);
			pass_check=checkIfCorrect(g_received_password,password_from_mem);
			if(pass_check==WRONG)
			{
				/*
				 * P indicates that password is wrong and
				 * the user can have one trial to enter corrent password
				 * also user will be asked to enter the password
				 * and wont be asked to re-enter it to confirm it
				 */
				UART_sendByte('P');
				UART_sendByte(MC2_READY);
				_delay_ms(10000);
				receivePasswordUsingUart(g_received_password);
				pass_check=checkIfCorrect(g_received_password,password_from_mem);
				/*
				 * if password is entered wrong on first trial then
				 * right on second trial open the door
				 * by turning the motor on cw and sending and indication'R ' to MC1
				 */
				 if(pass_check==RIGHT){
					DcMotor_Rotate(CW,100);

					 UART_sendByte('R');
				}
			}
			/*
			 * if password is right on first trial open the door
			 * by turning the motor on cw
			 */
			else if(pass_check==RIGHT){
				DcMotor_Rotate(CW,100);
				_delay_ms(5000);
				/*
				 * used timer0 to make delay then turn motor off
				 * */
				/*
				Timer0_Init(&timer0_configration);
				while(count1!=250){}
				count1=0;
				Timer0_DeInit();
				*/
				DcMotor_Rotate(A_CW,100);
				UART_sendByte('R');


			}
		}
		/*in case user wants to change password
		 * user gets 3 trials before error is displayed then step 2 is displayed again
		 *
		 * */
		if(byte=='-')
		{
			retreivePassword(&password_from_mem);
			while(trials<=3){
				UART_sendByte(MC2_READY);
				_delay_ms(10000);
				receivePasswordUsingUart(g_received_password);
				pass_check=checkIfCorrect(g_received_password,password_from_mem);
				if(pass_check==WRONG)
				{
					/*
					 * O indicated that password is wrong and user gets 3 trials
					 * and re-enter password wont be diplayed
					 */
					UART_sendByte('O');		/*tell MCI1 to take the password again*/
					trials++;

				}
				else if(pass_check==RIGHT){
					UART_sendByte('R');
					break;
				}
			}
			/*
			 * display error if trials =4 and turn buzzer on
			 */
			if(trials==4){
				UART_sendString("Error #");
				_delay_ms(500);
				BUZZER_on();
				_delay_ms(5000);
				BUZZER_off();

			}

		}
	}
}
void Timer0_count1(void)
{
	count1++;
}

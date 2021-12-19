/*
 ================================================================================================
 Name        : Exercise1.c
 Author      : Mohamed Tarek
 Description : Exercise to test the UART functionality
 Date        : 4/11/2014
 ================================================================================================
 */

#include "uart.h"
#include "lcd.h"
#include "keypad.h"
#include "timer0.h"
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
	ENTER,RE_ENTER
}enter_or_reEnter;

/*******************************************************************************
 *                      	Global Variables                                   *
 *******************************************************************************/
uint8 g_password[10];	/*this will hold 5 digit password*/
uint8 g_re_entered_password[10];
uint8 g_trials =1;
uint8 g_open_door[30];		/*MCU2 send the sentence "+: open door"*/
uint8 g_Change_pass[30];		/*MCU2 send the sentence "-: change password haree" */
uint8 g_thief[20];			/*MCU2 send the word error in this string*/
uint32 count1=0;
Timer0_ConfigType timer0_configration;		/*timer0 configration*/

/*******************************************************************************
 *                    	  Functions Prototypes                                   *
 *******************************************************************************/
/*
 * this function displays "enter password on the screen
 * and displays "*" when any char is typed
 */
uint8 displayEnterPass(uint8 password[],uint8 which_time);
void sendPasswordUsingUart(uint8 sent_pass[]);
/*
 * turn inters into the equivlant asci value to be able to use uart
 */
uint8 intTOStringManually(uint8 num);
/*
 * Displays according to the state that it gets from MCU2
 * and transmits and receives
 */
uint8 userWishes(uint8 pass_check);
void Timer0_count1(void);
int main(void)
{
	uint8 trials =1;		/*static to keep track of how many times the password was entered wrong*/

	uint8 pass_check;
	uint8 key;		/*holds the user choice whether to open door or change password*/
	LCD_init();
	UART_ConfigType uart_configration;
	uart_configration.baud_rate=9600;
	uart_configration.data_bits_number=Eight;
	uart_configration.parity=Disabled;
	uart_configration.stop_bit=One;
	UART_init(&uart_configration);

	timer0_configration.initial_value=0;
	timer0_configration.timer_mode=NORMAL_MODE;
	timer0_configration.prescaler=FCPU_1024;
	Timer0_setCallBack(Timer0_count1);


	displayEnterPass(g_password,ENTER);
	displayEnterPass(g_re_entered_password,RE_ENTER);

	_delay_ms(100);
	sendPasswordUsingUart(g_password);		/*send pass and re entered pass using uart */
	sendPasswordUsingUart(g_re_entered_password);
	while(1)
	{
		key=userWishes(pass_check);		/*returns user wishes in case pass is correct*/
		if(key=='+')
		{
			displayEnterPass(g_password,ENTER);		/*ask user to enter password again to compare it with the saved one*/
			while(UART_recieveByte() != MC2_READY){}
			sendPasswordUsingUart(g_password);		/*send it to MCU2 to compare it with the saved one*/
			pass_check=UART_recieveByte();
			if(pass_check=='P')
			{
				displayEnterPass(g_password,ENTER);

				_delay_ms(100);
				sendPasswordUsingUart(g_password);
			}

		}
		if(key=='-')
		{
			displayEnterPass(g_password,ENTER);		/*ask user to enter password again to compare it with the saved one*/
			while(UART_recieveByte() != MC2_READY){}
			sendPasswordUsingUart(g_password);		/*send it to MCU2 to compare it with the saved one*/
			pass_check=UART_recieveByte();
			if(pass_check=='R')
			{
				displayEnterPass(g_password,ENTER);
				displayEnterPass(g_re_entered_password,RE_ENTER);

				_delay_ms(100);
				sendPasswordUsingUart(g_password);		/*send pass and re entered pass using uart */
				sendPasswordUsingUart(g_re_entered_password);

			}
			if(pass_check=='O')			/*this indicates that i want to ask for password once*/
			{
				while(trials<3)
				{
					displayEnterPass(g_password,ENTER);
					//displayEnterPass(g_re_entered_password,RE_ENTER);
					_delay_ms(100);
					sendPasswordUsingUart(g_password);
					//sendPasswordUsingUart(g_re_entered_password);
					trials++;
				}

				if(trials==3){
					UART_receiveString(g_thief);
					_delay_ms(500);
					LCD_moveCursor(1,0);
					LCD_displayString("              ");
					LCD_moveCursor(0,0);
					LCD_displayString("              ");
					LCD_moveCursor(0,0);
					LCD_displayString(g_thief);
					_delay_ms(2000);	/*clear screen after 5 seconds*/
					LCD_moveCursor(0,0);
					LCD_displayString("              ");
					LCD_moveCursor(1,0);
					LCD_displayString("              ");
					LCD_moveCursor(0,0);

					LCD_displayString(g_open_door);
					LCD_moveCursor(1,0);
					LCD_displayString(g_Change_pass);
					key=KEYPAD_getPressedKey();
					UART_sendByte(key);		/*send the user choice + or -*/
					LCD_moveCursor(0,0);
				}

				}


		}
	}
	return 0;
}

uint8 displayEnterPass(uint8 password[],uint8 which_time)
{
	uint8 count=0;	/*used to take five keys*/
	uint8 button;
	if(which_time==ENTER){
		LCD_displayString("Enter Pass:");
	}
	else if(which_time==RE_ENTER){
		LCD_displayString("Re-enter Pass:");
	}
	LCD_moveCursor(1,0);		/*move cursor to second line*/
	while(count<5)
	{
		button=KEYPAD_getPressedKey();
		if((button <= 9) && (button >= 0))
		{
			LCD_intgerToString(button); /* display the pressed keypad switch */
		}
		else
		{
			LCD_displayCharacter(button); /* display the pressed keypad switch */
		}
		_delay_ms(500); /* Press time */

		//LCD_displayString("*");
		password[count]=button;
		count++;
	}
	LCD_moveCursor(1,0);
	LCD_displayString("              ");
	LCD_moveCursor(0,0);
	LCD_displayString("              ");
	LCD_moveCursor(0,0);

	return 1;
}
/*
 * turn inters into the equivlant asci value to be able to use uart
 */
uint8 intTOStringManually(uint8 num)
{
	if(num==0)return '0';
	else if(num==1)return '1';
	else if(num==2)return '2';
	else if(num==3)return '3';
	else if(num==4)return '4';
	else if(num==5)return '5';
	else if(num==6)return '6';
	else if(num==7)return '7';
	else if(num==8)return '8';
	else if(num==9)return '9';
	else if (num=='+')return '+';
	else if (num=='-')return '-';
	else if (num=='*')return '*';
	else if (num=='%')return '%';

	return '0';
}
void sendPasswordUsingUart(uint8 sent_pass[])
{
	uint8 i=0;
	uint8 modified;
	while(i<5)
	{
		modified=intTOStringManually(sent_pass[i]);
		//while(UART_recieveByte() != MC2_READY){}
		UART_sendByte(modified);
		_delay_ms(500);
		i++;
	}
}
/*
 * displayes acording to the stste that it gets from MCU2
 */
uint8 userWishes(uint8 pass_check)
{

	uint8 key='*';
	uint8 W_trials=1;
	static uint8 R_trials=1;
	pass_check=UART_recieveByte();
	if(pass_check=='R'){
		/*
		 * if password is correct ask if the user wants to open door or change pass
		 * then send the key that the user choose to MCU to MCU2
		 */
			UART_receiveString(g_open_door);
			LCD_moveCursor(0,0);
			LCD_displayString(g_open_door);

			UART_receiveString(g_Change_pass);
			LCD_moveCursor(1,0);
			LCD_displayString(g_Change_pass);
			key=KEYPAD_getPressedKey();
			UART_sendByte(key);		/*send the user choice + or -*/
			LCD_moveCursor(1,0);
			LCD_displayString("              ");
			LCD_moveCursor(0,0);
			LCD_displayString("              ");
			LCD_moveCursor(0,0);
			R_trials++;

	}
	if(pass_check=='P')
	{
		displayEnterPass(g_password,ENTER);
		_delay_ms(100);
		sendPasswordUsingUart(g_password);
		LCD_moveCursor(1,0);
		LCD_displayString("              ");
		LCD_moveCursor(0,0);
		LCD_displayString("              ");
		LCD_moveCursor(0,0);
	}
	if(pass_check=='W')
	{
		displayEnterPass(g_password,ENTER);
		displayEnterPass(g_re_entered_password,RE_ENTER);
		_delay_ms(100);
		sendPasswordUsingUart(g_password);
		sendPasswordUsingUart(g_re_entered_password);
		W_trials++;
		if(W_trials==4){
			UART_receiveString(g_thief);
			LCD_moveCursor(1,0);
			LCD_displayString("              ");
			LCD_moveCursor(0,0);
			LCD_displayString("              ");
			LCD_moveCursor(0,0);
			LCD_displayString(g_thief);
			_delay_ms(10000);

		}
	}


	return key;
}
void Timer0_count1(void)
{
	count1++;
}

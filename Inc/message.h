/*
 * message.h
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *  	Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 */

#include <stdio.h>

/************* Define *************/

#define MAX_NUMBER_MSG		10

/*** Message codes ***/

#define ERROR_LED_TIMER		0
#define LCD_TIMER				1

/* End message codes */

/**********************************/


/************* Global Variables ***********/
/******************************************/


/************* Function ***********/

void Init_message (void);
void Send_message (uint16_t Msg);
uint8_t Get_message (uint16_t Msg);
void Process_message(void);


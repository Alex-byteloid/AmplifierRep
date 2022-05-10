/*
 * message.c
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 *  Brief: Файл содержит реализацию механизма обмена сообщениями для конечных автоматов.
 *		   Гарантируется приём сообщения на следующем цикле после отправки сообщения.
 *		   Три состояния сделано для возможности приёма сообщения не только сверху вниз по списку автоматов.
 *		   Функция отправки сообщения защищена от повторной отправки сообщения условием, но стоит обратить внимание,что она вызывается
 *		   и проверяется условие(Стоит обратить особое внимание при отправки сообщений из часто возникающего прерывания).
 */

#include <message.h>


/************* Define *************/

#define NOT_ACTIVE			'0'
#define SET_BUT_NOT_ACTIVE	'1'
#define ACTIVE_MSG			'2'

/**********************************/


/************* Global Variables ***********/

volatile static char Message [MAX_NUMBER_MSG];

/******************************************/


/************* Function ***********/

void Init_message (void){
	for(uint32_t i = 0; i < MAX_NUMBER_MSG; i++){
		Message[i] = NOT_ACTIVE;
	}
}

void Send_message (uint16_t Msg){
	if (Message[Msg] == NOT_ACTIVE)
		Message[Msg] = SET_BUT_NOT_ACTIVE;
}

uint8_t Get_message (uint16_t Msg){
	if (Message[Msg] == ACTIVE_MSG){
		Message[Msg] = NOT_ACTIVE;
		return 1;
	}
	return 0;
}

void Process_message(void){
	for(uint32_t i = 0; i < MAX_NUMBER_MSG; i++){
		if (Message[i] == ACTIVE_MSG) Message[i] = NOT_ACTIVE;
		if (Message[i] == SET_BUT_NOT_ACTIVE) Message[i] = ACTIVE_MSG;
	}
}

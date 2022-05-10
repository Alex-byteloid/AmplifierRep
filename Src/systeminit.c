/*
 * systeminit.c
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 *		Brief: Файл содержит функции инициализации генератора тактовой частоты для всех шин, ядра, и портов ввода-вывода.
 *
 */

#include <stm32f4xx.h>
#include <system_stm32f4xx.h>


/************* Define *************/

/**********************************/


/************* Global Variables ***********/

static int8_t SysInitState;
static int8_t _SysInitState;
static int8_t entrySysInit;

/******************************************/


/************* Function ***********/


void Process_initialization (void){

	if (_SysInitState != SysInitState)
		entrySysInit = 1;
	else entrySysInit = 0;

	_SysInitState = SysInitState;

	switch (SysInitState) {
		case 0:

			break;
		default:
			break;
	}
}

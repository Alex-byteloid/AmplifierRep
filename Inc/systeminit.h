/*
 * systeminit.h
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <system_stm32f4xx.h>
#include <message.h>
#include <systemtimer.h>

/************* Define *************/

/*** Message codes ***/
// START_MK_CLOCK_INIT
/* End message codes */

/**********************************/


/************* Global Variables ***********/

/******************************************/


/************* Function ***********/

void Init_sys_init_FSM (void);
void Process_initialization_FSM (void);

/*
 * systemtimer.h
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 */

#include <stdint.h>
#include <stm32f4xx.h>
#include <system_stm32f4xx.h>

/************* Define *************/

#define MAX_NUMB_G_TIMERS	4

/*** Timers codes ***/

#define ERROR_LED_TIMER		0

/* End timers codes */

/**********************************/


/************* Global Variables ***********/

/******************************************/


/************* Function ***********/

void Init_sys_tick_timer(void);
void SysTick_Handler (void);

void Init_g_timers(void);
void Start_g_timer(uint8_t TimerID);
void Stop_g_timer(uint8_t TimerID);
void Pause_g_timer(uint8_t TimerID);
uint32_t Get_g_timer_val(uint8_t TimerID);

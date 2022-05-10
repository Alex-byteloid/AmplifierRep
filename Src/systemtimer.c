/*
 * systemtimer.c
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 *      Brief: Файл содержит функции инициализации, запуска, остановки, сброса без остановки и
 *      	   получения значения используемых в приложении таймеров. Для реализации используется SysTickTimer.
 */

#include <systemtimer.h>

/************* Define *************/
#define LAUNCHED			'L'
#define STOPPED			'S'
#define PAUSED				'P'

#define MILLISECONDS
//#define MICROSECONDS

#ifdef MILLISECONDS
#define DIFF 1000
#endif

#ifdef MICROSECONDS
#define DIFF 1000000
#endif

/**********************************/


/************* Global Variables ***********/

static struct Timer{
uint32_t TimerValue;
uint8_t  Status;
}GlobalTimer[MAX_NUMB_G_TIMERS];

/******************************************/


/************* Function ***********/

void Init_sys_tick_timer(void){

	uint32_t ReloadValue;
	SystemCoreClockUpdate();
	ReloadValue = SystemCoreClock / DIFF;

	SysTick->LOAD = ReloadValue - 1;
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	__enable_irq();
	__NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler (void){
	for (uint8_t t = 0; t < MAX_NUMB_G_TIMERS; t++){
		if (GlobalTimer[t].Status == LAUNCHED)
			++GlobalTimer[t].TimerValue;
	}
}

void Init_g_timers(void){
	for (uint8_t t = 0; t < MAX_NUMB_G_TIMERS; t++){
		GlobalTimer[t].Status = STOPPED;
		GlobalTimer[t].TimerValue = 0;
	}
}

void Start_g_timer(uint8_t TimerID){
	if (GlobalTimer[TimerID].Status == STOPPED)
		GlobalTimer[TimerID].Status = LAUNCHED;
	else if (GlobalTimer[TimerID].Status == PAUSED)
		GlobalTimer[TimerID].Status = LAUNCHED;
}

void Stop_g_timer(uint8_t TimerID){
	GlobalTimer[TimerID].Status = STOPPED;
	GlobalTimer[TimerID].TimerValue = 0;
}

void Pause_g_timer(uint8_t TimerID){
	if (GlobalTimer[TimerID].Status == LAUNCHED)
		GlobalTimer[TimerID].Status = PAUSED;
}

uint32_t Get_g_timer_val (uint8_t TimerID){
	return GlobalTimer[TimerID].TimerValue;
}

/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 *
 *
 *
 ******************************************************************************
 */

#include <main.h>

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{

	Init_sys_init_FSM();
	Init_message();
	Send_message(START_MK_CLOCK_INIT);

    /* Loop forever */
	while(1){

		Process_initialization_FSM();
		Process_message();
	}
}

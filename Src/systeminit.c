/*
 * systeminit.c
 *
 *  Created on: May 10, 2022
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 *		MK: STM32F401CCU6
 *
 *		Brief: Файл содержит функции инициализации генератора тактовой частоты для всех шин, ядра, и портов ввода-вывода.
 *			   А также соответствующий конечный автомат.
 *			   В данном примере частота внешнего кварца 25 MHz, контроллер настраивается на частоту 72 MHz.
 *			   Для данной частоты задержка для флэш памяти равна 2 WS. "Масштабирование напряжения" иначе Voltage scaling
 *			   для значения частот (60 < F <= 84)MHz равно 2 (Voltage scaling -> Scale 2).
 */

#include <systeminit.h>


/************* Define *************/
#define MCO1
/**********************************/


/************* Global Variables ***********/

static int8_t sysInitState;
static int8_t _sysInitState;
static int8_t entrySysInit;

/******************************************/


/************* Function ***********/


void Process_initialization (void){

	if (_sysInitState != sysInitState)
		entrySysInit = 1;
	else entrySysInit = 0;

	_sysInitState = sysInitState;

	switch (sysInitState) {
		case 0:
			if (Get_message(START_MK_CLOCK_INIT))
				sysInitState = 1;
			break;

		case 1:
			if (entrySysInit == 1){
				SysTick->LOAD = SystemCoreClock - 1;					// Настройка таймера (работает без прерываний)
				SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
			}

			uint8_t TimeOutFlag = DISABLE;

			FLASH->ACR &= ~FLASH_ACR_LATENCY;
			FLASH->ACR |= FLASH_ACR_LATENCY_2WS;						// Настройка задержки чтения флеш памяти в зависимости от частоты МК

			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;					// Запуск таймера для контроля таймаута ожидания (1 сек)
			RCC->CR |= RCC_CR_HSEON;									// Включение HSE генератора

			while(!(RCC->CR & RCC_CR_HSERDY)){							// Ожидание готовности HSE
				if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){			// Проверка "переполнения" таймера
					TimeOutFlag = ENABLE;
					SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;			// Отключение таймера
					break;
				}
			}
			if (TimeOutFlag == ENABLE){									// Переход в режим работы 2(неисправность) если достигнут
				sysInitState = 2;										// таймаут ожидания готовности HSE
				break;
			}

			RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;						// Выбор генератора HSE в качестве источника тактирования PLL

			RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;							// Обнуляем регистр делителя PLL
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0;							// Значение делителя PLL равно 25 (PLLM = 25)
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_3;							// На выходе делителя 1 MHz
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_4;

			RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN;							// Обнуляем регистр множителя PLLN
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_5;							// Значение множителя PLLN -> 288
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLN_8;							// На выходе VCO -> 288 MHz

			RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;							// Значение делителя на выходе PLL -> 4
			RCC->PLLCFGR |= RCC_PLLCFGR_PLLP_0;							// На выходе PLL 72 MHz

			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;					// Запуск таймера для контроля таймаута ожидания (1 сек)
			RCC->CR |= RCC_CR_PLLON;									// Включение PLL

			while(!(RCC->CR & RCC_CR_PLLRDY)){							// Ожидание готовности PLL
				if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){			// Проверка "переполнения" таймера
					TimeOutFlag = ENABLE;
					SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;			// Отключение таймера
					break;
				}
			}
			if (TimeOutFlag == ENABLE){
				sysInitState = 2;										// Переход в режим работы 2(неисправность) если достигнут
				break;													// таймаут ожидания готовности PLL
			}

			RCC->CFGR &= ~RCC_CFGR_HPRE_3;								// Частота для шины AHB не делится и равна PLL

			RCC->CFGR &= ~RCC_CFGR_PPRE1;
			RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;							// Частота для шины APB1 делится на 2 (APB1 -> 36 MHz)

			RCC->CFGR &= ~RCC_CFGR_PPRE2;
			RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;							// Частота для шины APB2 не делится (APB2 -> 72 MHz)

			RCC->CFGR &= ~RCC_CFGR_SW;									// Обнуляем регистр выбора источника тактирования системной шины
			RCC->CFGR |= RCC_CFGR_SW_PLL;								// Выбор в качестве источника тактирования сист. шины PLL генератора



			break;

		default:
			break;
	}
}

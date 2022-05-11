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
//#define MCO1
/**********************************/


/************* Global Variables ***********/

static volatile int8_t sysInitState;
static volatile int8_t _sysInitState;
static volatile int8_t entrySysInit;

/******************************************/


/************* Function ***********/

void Init_sys_init_FSM (void){
	sysInitState = 0;
	_sysInitState = 0;
	entrySysInit = 0;
}

void Process_initialization_FSM (void){

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

#ifdef MCO1
			RCC->CFGR |= RCC_CFGR_MCO1;									// На выход MCO1 подаётся тактовый сигнал с PLL
			RCC->CFGR &= ~RCC_CFGR_MCO1PRE;
			RCC->CFGR |= RCC_CFGR_MCO1PRE_1 |							// Частота PLL на выходе MCO1 делится на 4
						 RCC_CFGR_MCO1PRE_2;

			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;						// Включение тактирования портов A
			GPIOA->MODER &= ~GPIO_MODER_MODE8;
			GPIOA->MODER |= GPIO_MODER_MODE8_1;							// Выход PA8 в режиме 'Alternate function'
			GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8;					// Максимальная скорость работы выхода
#endif

			uint8_t TimeOutFlag = DISABLE;

			FLASH->ACR &= ~FLASH_ACR_LATENCY;
			FLASH->ACR |= FLASH_ACR_LATENCY_2WS;						// Настройка задержки чтения флеш памяти в зависимости от частоты МК

			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;					// Запуск таймера для контроля таймаута ожидания (1 сек)
			RCC->CR |= RCC_CR_HSEON;									// Включение HSE генератора

			while(!(RCC->CR & RCC_CR_HSERDY)){							// Ожидание готовности HSE
				if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){			// Проверка "переполнения" таймера
					TimeOutFlag = ENABLE;
					break;
				}
			}
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;			// Отключение таймера
			if (TimeOutFlag == ENABLE){									// Переход в режим работы '2'(неисправность) если достигнут
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
					break;
				}
			}
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;			// Отключение таймера
			if (TimeOutFlag == ENABLE){
				sysInitState = 2;										// Переход в режим работы '2'(неисправность) если достигнут
				break;													// таймаут ожидания готовности 'PLL'
			}

			RCC->CFGR &= ~RCC_CFGR_HPRE_3;								// Частота для шины 'AHB' не делится и равна 'PLL'

			RCC->CFGR &= ~RCC_CFGR_PPRE1;
			RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;							// Частота для шины 'APB1' делится на 2 (APB1 -> 36 MHz)

			RCC->CFGR &= ~RCC_CFGR_PPRE2;
			RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;							// Частота для шины 'APB2' не делится (APB2 -> 72 MHz)

			SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;					// Запуск таймера для контроля таймаута ожидания (1 сек)
			RCC->CFGR &= ~RCC_CFGR_SW;									// Обнуляем регистр выбора источника тактирования системной шины
			RCC->CFGR |= RCC_CFGR_SW_PLL;								// Выбор в качестве источника тактирования сист. шины 'PLL' генератора

			while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){
				if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk){
					TimeOutFlag = ENABLE;
					break;
				}
			}
			SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;					// Отключение таймера
			if (TimeOutFlag == ENABLE){
				sysInitState = 2;										// Переход в режим работы '2'(неисправность) если достигнут
				break;													// таймаут переключения источника частоты сист. шины
			}

			RCC->APB1ENR |= RCC_APB1ENR_PWREN;							// Включение тактирования "менеджера питания"(?)
			PWR->CR &= ~PWR_CR_VOS;
			PWR->CR |= PWR_CR_VOS_1;									// 'Voltage scaling -> Scale 2'

			/* Инициализация порта для работы индикатора неисправностей */
			RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
			GPIOC->OTYPER &= ~GPIO_OTYPER_OT13;							// Настройка порта 'PC13' на выход push-pull
			GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR13;
			GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR13;					// Настройка на высокую выходную скорость
			GPIOC->PUPDR &= ~GPIO_PUPDR_PUPD13;							// 'No pull, no down' (без подтяжки к плюсу или минусу питания)
			GPIOC->MODER &= ~GPIO_MODER_MODE13;
			GPIOC->MODER |= GPIO_MODER_MODE13_0;						// Настройка порта 'PC13' на выход
			GPIOC->BSRR |= GPIO_BSRR_BS13;								// Выключение индикатора на 'PC13'
			/* Конец инициализации индикатора */

			SystemCoreClockUpdate();									// Обновление значения переменной частоты системной шины
			sysInitState = 3;											// При успешной инициализации перифирии переход в состояние '3'
			break;

		case 2:
			if(entrySysInit == 1){
				GPIOC->BSRR |= GPIO_BSRR_BR13;							// Включение индикатора на 'PC13'
			}
			break;

		case 3:
			if(entrySysInit == 1){
				/* Здесь рассылаются сообщения инициализации для нормального режима работы,
				 * а также инициализируется необходимая переферия для данного режима работы */

				Init_sys_tick_timer();
				Init_g_timers();
			}
			if(Get_g_timer_val(ERROR_LED_TIMER) == 0){
				GPIOC->BSRR |= GPIO_BSRR_BR13;
				Start_g_timer(ERROR_LED_TIMER);
			}
			if(Get_g_timer_val(ERROR_LED_TIMER) >= 400)
				GPIOC->BSRR |= GPIO_BSRR_BS13;
			if(Get_g_timer_val(ERROR_LED_TIMER) >= 1100)
				Stop_g_timer(ERROR_LED_TIMER);
			break;

		default:
			break;
	}
}

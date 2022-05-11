/*
 * lcd16xx.c
 *
 *  Created on: 11 мая 2022 г.
 *      Author: oktyabr
 *      Version	->	'1.0a1'	(В разработке, только для внутреннего использования)
 *
 *		MK: STM32F401CCU6
 *
 *		Brief:	0x02 -> STX (начало текста)
 *				0x03 -> ETX (конец текста)
 *				0x05 -> ENQ (начало команд)
 *				0x06 -> ACK (конец команд)
 */

#include <lcd16xx.h>

/************* Define *************/

/**********************************/


/************* Global Variables ***********/

volatile struct RingBuffer{
	uint8_t Data[256];
	uint8_t InputPointer;
	uint8_t OutputPointer;
}InputBuffer;

/******************************************/


/************* Function ***********/

void Write_command_to_input_buffer (uint8_t Command){

	uint8_t Up = Command & 0xF0;
	uint8_t Low = (Command << 4) & 0xF0;

		if (InputBuffer.InputPointer >= 256)
			InputBuffer.InputPointer = 0;
		InputBuffer.Data[InputBuffer.InputPointer] = (Up | 0x0C);
        ++InputBuffer.InputPointer;

        if (InputBuffer.InputPointer >= 256)
        	InputBuffer.InputPointer = 0;
		InputBuffer.Data[InputBuffer.InputPointer] = (Up | 0x08);
        ++InputBuffer.InputPointer;


        if (InputBuffer.InputPointer >= 256)
        	InputBuffer.InputPointer = 0;
        InputBuffer.Data[InputBuffer.InputPointer] = (Low | 0x0C);
        ++InputBuffer.InputPointer;

        if (InputBuffer.InputPointer >= 256)
        	InputBuffer.InputPointer = 0;
        InputBuffer.Data[InputBuffer.InputPointer] = (Low | 0x08);
        ++InputBuffer.InputPointer;
}

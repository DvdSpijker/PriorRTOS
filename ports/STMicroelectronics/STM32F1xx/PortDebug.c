/*
 * PortDebug.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortDebug.h"

#include "usart.h"
#include "stm32f1xx_hal.h"

#include <string.h>

#define UART_RTX_CHAR_TIMEOUT_MS 100
#define UART_MAX_STRING_LENGTH 1000

void PortDebugUartCallbackOnRx(uint8_t byte);

void PortDebugUartInit(U32_t baud_rate)
{
	UartDebugCallbackSetOnRx(PortDebugUartCallbackOnRx);
	UartDebugInit(baud_rate);
	UartIntEnableRxDebug(1);
}

typedef void (*UartCallbackRx_t)(uint8_t byte);

/* USER CODE END Private defines */

U32_t PortDebugUartWriteString(char *str)
{
	U32_t str_len = (U32_t)strnlen((const char *)str, UART_MAX_STRING_LENGTH + 1);

	for(U32_t i = 0; i < str_len; i++)
	{
		if(PortDebugUartWriteChar(str[i]) == -1) {
			break;
		}
	}

	return str_len;
}

U8_t PortDebugUartWriteChar(char c)
{
	HAL_StatusTypeDef res =	HAL_UART_Transmit(&huart_debug, (uint8_t *)&c, 1, UART_RTX_CHAR_TIMEOUT_MS);

	return (res == HAL_OK ? 0 : 1);
}

char PortDebugUartReadChar(void)
{
	char c = '\0';

	HAL_UART_Receive(&huart_debug, (uint8_t *)&c, 1, UART_RTX_CHAR_TIMEOUT_MS);

	return c;
}

void PortDebugUartCallbackOnRx(uint8_t byte)
{
	PortDebugCallbackReadChars((char *)&byte, 1);
}


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

int PortDebugUartInit(uint32_t baud_rate)
{
	 MX_USART1_UART_Init(baud_rate);

	 return 0;
}

int PortDebugUartWriteString(char *str)
{


	size_t str_len = strnlen((const char *)str, UART_MAX_STRING_LENGTH + 1);

	for(size_t i = 0; i < str_len; i++)
	{
		if(PortDebugUartWriteChar(str[i]) == -1) {
			break;
		}
	}

	return (int)str_len;
}

int PortDebugUartWriteChar(char c)
{
	HAL_StatusTypeDef res =	HAL_UART_Transmit(&huart1, (uint8_t *)&c, 1, UART_RTX_CHAR_TIMEOUT_MS);

	return (res == HAL_OK ? 0 : -1);
}

char PortDebugUartReadChar(void)
{
	char c = '\0';

	HAL_UART_Receive(&huart1, (uint8_t *)&c, 1, UART_RTX_CHAR_TIMEOUT_MS);

	return c;
}

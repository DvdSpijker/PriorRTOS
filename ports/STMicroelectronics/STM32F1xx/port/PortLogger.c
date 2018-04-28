/*
 * PortLogger.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortLogger.h"

#include "usart.h"
#include "stm32f1xx_hal_uart.h"

int PortDebugUartInit(uint32_t baud_rate)
{
	 MX_USART1_UART_Init(baud_rate);
}

int PortDebugUartWriteString(char *str)
{
}

int PortDebugUartWriteChar(char c)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)&c, 1, 100);
}

char PortDebugUartReadChar(void)
{
	char c = '\0';
	HAL_UART_Receive(&huart1, &c, 1, 100);
	return c;
}

/*
 * PortLogger.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortLogger.h"
#include <HalUart.h>
#include <stdlib.h>

struct  HalUartInstance DebugUartInstance;

int PortDebugUartInit(uint32_t baud_rate)
{
	DebugUartInstance.baud_rate = baud_rate;
	DebugUartInstance.channel = HAL_UART_CHANNEL_LOGGER;
	DebugUartInstance.n_stop_bits = HAL_UART_STOP_BITS_2;
	DebugUartInstance.rx_callback = NULL;
	HalUartInit(&DebugUartInstance);
}

int PortDebugUartWriteString(char *str)
{
	HalUartSendString(&DebugUartInstance, str);
}

int PortDebugUartWriteChar(char *c)
{
	HalUartSendChar(&DebugUartInstance, c);
}

char PortDebugUartReadChar(void)
{
	char c = '\0';
	HalUartReceive(&HalUartInstance, &c, 1);
	return c;
}

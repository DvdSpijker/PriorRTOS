/*
 * PortLogger.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortLogger.h"
#include <stdlib.h>

#include "uart.h"

int PortDebugUartInit(uint32_t baud_rate)
{
    UART_Init(baud_rate);
}

int PortDebugUartWriteString(char *str)
{
	UART_PutString(str);
}

int PortDebugUartWriteChar(char c)
{
	UART_PutChar(c);
}

char PortDebugUartReadChar(void)
{
	char c = '\0';
	if(UART_AvaliableRx()) {
        UART_GetString(&c, 1);
    }
	return c;
}

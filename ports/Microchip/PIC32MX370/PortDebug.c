/*
 * PortDebug.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortDebug.h"
#include <stdlib.h>

//#include "uart.h"

void PortDebugUartInit(uint32_t baud_rate)
{
    //UART_Init(baud_rate);
}

U32_t PortDebugUartWriteString(char *str)
{
	//UART_PutString(str);
}

U8_t PortDebugUartWriteChar(char c)
{
	//UART_PutChar(c);
    return 1;
}

/*
 * PortDebug.c
 *
 * Created: 7-3-2018 22:24:37
 *  Author: Dorus
 */ 

#include "PortDebug.h"
#include <stdlib.h>

#include "uart.h"

static void UART_OnRx(char *data, unsigned char n);

void PortDebugUartInit(U32_t baud_rate)
{
    UART_Init(baud_rate, UART_OnRx);
}

U32_t PortDebugUartWriteString(char *str)
{
	UART_PutString(str);
}

U8_t PortDebugUartWriteChar(char c)
{
	UART_PutChar(c);
    
    return 1;
}

static void UART_OnRx(char *data, unsigned char n)
{
    PortDebugCallbackReadChars(data, (U32_t)n);
}

/* Hooks into printf. Needed because internal logger hook does not work
 * on PIC32. */
 void _mon_putc (char c);
 void _mon_putc (char c)
 {
   PortDebugUartWriteChar(c);
 }
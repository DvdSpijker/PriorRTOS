/*
 * PortDebug.h
 *
 * Created: 4-3-2018 1:29:41
 *  Author: Dorus
 */ 


#ifndef PORT_DEBUG_H_
#define PORT_DEBUG_H_

#include <stdint.h>

/* Debug UART Port. */

int PortDebugUartInit(uint32_t baud_rate);

int PortDebugUartWriteString(char *str);

int PortDebugUartWriteChar(char c);

char PortDebugUartReadChar();



#endif /* PORT_DEBUG_H_ */

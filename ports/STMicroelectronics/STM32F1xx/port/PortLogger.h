/*
 * PortLogger.h
 *
 * Created: 4-3-2018 1:29:41
 *  Author: Dorus
 */ 


#ifndef PORT_LOGGER_H_
#define PORT_LOGGER_H_

#include <stdint.h>

/* Debug UART Port. */

int PortDebugUartInit(uint32_t baud_rate);

int PortDebugUartWriteString(char *str);

int PortDebugUartWriteChar(char *c);

char PortDebugUartReadChar();



#endif /* LOGGER_PORT_H_ */
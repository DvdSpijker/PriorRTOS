/*
 * PortLog.h
 *
 * Created: 4-3-2018 1:29:41
 *  Author: Dorus
 */ 


#ifndef PORT_LOG_H_
#define PORT_LOG_H_


/* Debug UART Port. */

int PortDebugUartInit(uint32_t baud_rate);

int PortDebugUartWriteString(char *str);

int PortDebugUartWriteChar(char *c);

char PortDebugUartReadChar(void);



#endif /* PORT_LOG_H_ */
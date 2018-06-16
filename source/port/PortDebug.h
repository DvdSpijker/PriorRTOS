/*
 * PortDebug.h
 *
 * Created: 4-3-2018 1:29:41
 *  Author: Dorus
 */ 


#ifndef PORT_DEBUG_H_
#define PORT_DEBUG_H_

#include <OsTypes.h>

/******************************************************************************
 * @func: void PortDebugUartInit(U32_t baud_rate)
 *
 * @desc: Initializes the Debug serial port at the specified baud rate.
 *
 * @argin: (U32_t) baud_rate; Debug serial baud rate in bits per second.
 ******************************************************************************/
void PortDebugUartInit(U32_t baud_rate);

/******************************************************************************
 * @func: U32_t PortDebugUartWriteString(char *str)
 *
 * @desc: Writes a null-terminated string to the Debug serial port.
 *
 * @argin: (char *) str; Null-terminated char array.
 *
 * @rettype: (U32_t); number of chars written.
 * @retval: 0; if no chars were written due to an error or busy port.
 * @retval: Other; number of chars written.
 ******************************************************************************/
U32_t PortDebugUartWriteString(char *str);

/******************************************************************************
 * @func: U8_t PortDebugUartWriteChar(char c)
 *
 * @desc: Writes a single char to the Debug serial port.
 *
 * @argin: (char) c; Character.
 *
 * @rettype: (U8_t); Characters written.
 * @retval: 0; if no char was written due to an error or busy port.
 * @retval: 1; character was written.
 ******************************************************************************/
U8_t PortDebugUartWriteChar(char c);

/******************************************************************************
 * @func: void PortDebugCallbackReadChars(char *c, U32_t n)
 *
 * @desc: Reads the received characters. Must be called when characters were
 * received on the Debug serial port.
 *
 * @argin: (char *) chars; Character array.
 * @argin: (U32_t) n; Number of chars received.
 ******************************************************************************/
void PortDebugCallbackReadChars(char *chars, U32_t n);

/* Weak implementation of the ReadChars callback in case the Shell module in use. */
void __attribute__((weak)) PortDebugCallbackReadChars(char *c, U32_t n)
{
	OS_ARG_UNUSED(c);
	OS_ARG_UNUSED(n);
}


#endif /* PORT_DEBUG_H_ */

/******************************************************************************************************************************************
 *  File: Logger.h
 *  Description: Logger API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/

#ifndef LOGGER_H_
#define LOGGER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <PriorRTOSConfig.h>
#include <OsTypes.h>

#ifdef PRTOS_CONFIG_USE_LOGGER_MODE_RINGBUFFER
Id_t LoggerRingbuf;
#endif

#define LOG_LINE_NEW    0x00
#define LOG_LINE_APPEND 0x01

#define LOG_FILE_NAME(filename) static const char SourceFileName[] = filename;

#if PRTOS_CONFIG_ENABLE_LOG_ERROR==1
void LogError(U8_t log_line_opt, const char* source, const char* message, ...);


/******************************************************************************
 * @func:  LOG_ERROR_NEWLINE(message, ...)
 *
 * @desc: Logs an error message on a new line. A new line includes the
 * timestamp, tag, function name and line number.
 ******************************************************************************/
#define LOG_ERROR_NEWLINE(message, ...) \
LogError(LOG_LINE_NEW, __FUNCTION__, message, ##__VA_ARGS__);\

/******************************************************************************
 * @func:  LOG_ERROR_APPEND(message, ...)
 *
 * @desc: Appends the error message to the current logging line.
 * This appended message will NOT contain a timestamp, tag, function name
 * and line number.
 ******************************************************************************/
#define LOG_ERROR_APPEND(message, ...) \
LogError(LOG_LINE_APPEND, NULL, message, ##__VA_ARGS__);\

#else

#define LOG_ERROR_NEWLINE(message, ...) ;
#define LOG_ERROR_APPEND(message, ...) ;

#endif


#if PRTOS_CONFIG_ENABLE_LOG_DEBUG==1
void LogDebug(U8_t log_line_opt, const char* source, const int line_nr, const char* message, ...);

/******************************************************************************
 * @func:  LOG_DEBUG_NEWLINE(message, ...)
 *
 * @desc: Logs a debug message on a new line. A new line includes the
 * timestamp, tag, source and line number.
 ******************************************************************************/
#define LOG_DEBUG_NEWLINE(message, ...) \
LogDebug(LOG_LINE_NEW, SourceFileName, __LINE__, message, ##__VA_ARGS__);\

/******************************************************************************
 * @func:  LOG_DEBUG_APPEND(message, ...)
 *
 * @desc: Appends the debug message to the current logging line.
 * This appended message will NOT contain a timestamp, tag, source and line number.
 ******************************************************************************/
#define LOG_DEBUG_APPEND(message, ...) \
LogDebug(LOG_LINE_APPEND, NULL, 0, message, ##__VA_ARGS__);\

#else

#define LOG_DEBUG_NEWLINE(message, ...) ;
#define LOG_DEBUG_APPEND(message, ...) ;

#endif


#ifdef __cplusplus
}
#endif
#endif /* LOGGER_H_ */

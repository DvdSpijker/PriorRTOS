/*
 * LoggerDef.h
 *
 * Created: 28-9-2017 19:55:23
 *  Author: Dorus
 */


#ifndef LOGGER_DEF_H_
#define LOGGER_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Logger.h>
#include <Event.h>

/******************************************************************************
 * @func:  OsResult_t KLogInit(void)
 *
 * @desc: Initializes the logger module.
 *
 * Arguments:
 * N/A
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_RES_OK if the logging module was successfully initialized.
 * @retval:   OS_RES_ERROR if the initialization failed.
 ******************************************************************************/
OsResult_t KLogInit(void);

#if PRTOS_CONFIG_ENABLE_LOG_INFO==1
void KLogInfo(U8_t log_line_opt, const char* message, ...);
#define LOG_INFO_NEWLINE(message, ...) \
KLogInfo(LOG_LINE_NEW, message, ##__VA_ARGS__);\

#define LOG_INFO_APPEND(message, ...) \
KLogInfo(LOG_LINE_APPEND, message, ##__VA_ARGS__);\

#else

#define LOG_INFO_NEWLINE(message, ...) ;
#define LOG_INFO_APPEND(message, ...) ;

#endif


#if PRTOS_CONFIG_ENABLE_LOG_EVENT==1
void KLogEvent(pEvent_t event);

#define LOG_EVENT(evt)   \
KLogEvent((pEvent_t)evt);  \

#else
#define LOG_EVENT(evt);
#endif


#ifdef __cplusplus
}
#endif

#endif /* LOGGER_DEF_H_ */

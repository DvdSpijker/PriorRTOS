/*
 * Prior_logger.c
 *
 * Created: 3-3-2017 12:53:32
 *  Author: Dorus
 */

#include <Logger.h>
#include <LoggerDef.h>
#include <PortLogger.h>
#include <stdio.h>

#if PRTOS_CONFIG_USE_NEWLIB==1
#include <unistd.h>
#include <errno.h>
#endif

static void ILogFormat(U8_t log_line_opt, const char *tag, const char *source, const int line_nr, const char *message, va_list args);

#if PRTOS_CONFIG_USE_NEWLIB==0
static int ILogPutChar(char c, FILE *stream);
static FILE mystdout = FDEV_SETUP_STREAM(ILogPutChar, NULL, _FDEV_SETUP_WRITE);
#endif

extern OsResult_t OsRunTimeGet(U32_t* target);

OsResult_t LogInit(void)
{
	PortDebugUartInit(PRTOS_CONFIG_LOGGER_UART_BAUD_RATE_BPS);
	
#if PRTOS_CONFIG_USE_NEWLIB==0
    stdout = &mystdout;
#endif

    LOG_INFO_APPEND("\nLOGGING FORMATS\n");
    LOG_INFO_APPEND("[h:s:ms] [tag] [<additional>] : message");
    return OS_RES_OK;
}

static void ILogFormat(U8_t log_line_opt, const char *tag, const char *source, const int line_nr, const char *message, va_list args)
{
    if(log_line_opt == LOG_LINE_NEW) {
        OsRunTime_t runtime;
        OS_RUN_TIME_INIT(runtime);
        OsRunTimeGet(runtime);
        U32_t runtime_h = runtime[OS_RUN_TIME_HOURS];
        U32_t runtime_s = runtime[OS_RUN_TIME_MICROS] / 1e6;
        U32_t runtime_ms = (runtime[1] / 1e3) - (runtime_s * 1e3);

        printf("\n[%lu : ", runtime_h);
        printf("%lu : ", runtime_s);
        printf("%lu] ",  runtime_ms);

        if(!strcmp(tag, "info") ) {
            printf("[%s] : ", tag);
        } else if(!strcmp(tag, "error") || !strcmp(tag, "event")) {
            printf("[%s] [%s] : ", tag, source);
        } else {
            printf("[%s] [%s:%d] : ", tag, source, line_nr);
        }

    }

    vprintf(message, args);
}

#if PRTOS_CONFIG_ENABLE_LOG_ERROR==1
void LogError(U8_t log_line_opt, const char *source, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    ILogFormat(log_line_opt, "error", source, 0, message, args);
    va_end(args);

}
#endif

#if PRTOS_CONFIG_ENABLE_LOG_INFO==1
void KLogInfo(U8_t log_line_opt, const char* message, ...)
{
#if PRTOS_CONFIG_ENABLE_LOG_INFO==1
    va_list args;
    va_start(args, message);
    ILogFormat(log_line_opt, "info", NULL, 0, message, args);
    va_end(args);
#endif
}
#endif

#if PRTOS_CONFIG_ENABLE_LOG_DEBUG==1
void LogDebug(U8_t log_line_opt, const char *source, const int line_nr, const char* message, ...)
{
    va_list args;
    va_start(args, message);
    ILogFormat(log_line_opt, "debug", source, line_nr, message, args);
    va_end(args);
}
#endif

#if PRTOS_CONFIG_ENABLE_LOG_EVENT==1
void KLogEvent(pEvent_t event)
{
//    char src_buffer[10];
//    char msg_buffer[100];
//
//    snprintf(src_buffer, sizeof(src_buffer), "%04x", event->source_id);
//    snprintf(msg_buffer, sizeof(msg_buffer), "Event Code: 0x%08x | Occurrence Count: %u | Lifetime (set/cnt): %u/%u", event->event_code,
//             event->occurrence_cnt, event->life_time_us, event->life_time_us_cnt);
//    ILogFormat(LOG_LINE_NEW, "event", src_buffer, 0, msg_buffer, NULL);
}
#endif


#if PRTOS_CONFIG_USE_NEWLIB==0
static int ILogPutChar(char c, FILE *stream)
{
    PortDebugUartWriteChar(c);
    return 1;
}

#else


/* Newlib hooks to redirect printf to UART. */
int _write(int file, char *ptr, int len);
int _write(int file, char *ptr, int len)
{
    char cr = '\r';
    int i;

    if (file == STDOUT_FILENO || file == STDERR_FILENO) {
        for (i = 0; i < len; i++) {
            if (ptr[i] == '\n') {
				PortDebugUartWriteChar(cr);
            }
            PortDebugUartWriteChar(ptr[i]);
        }
        return i;
    }
    errno = EIO;
    return -1;
}

#endif



/*
 * ShellTest.c
 *
 * Created: 27-8-2017 11:30:57
 *  Author: Dorus
 */

#include "ShellTest.h"
#include <PriorRTOS.h>

LOG_FILE_NAME("ShellTest.c");

void ShellTest(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    const RingbufBase_t command[] = {'h', 'e', 'l', 'p', ' ', '-', 'c', ' ', 'c', 'f', 'g', '\n'};
    U32_t command_size = sizeof(command);

    TASK_INIT_BEGIN();
    RingbufWrite(ShellRxRingbuf, (RingbufBase_t *)command, &command_size, OS_TIMEOUT_INFINITE);
    TASK_INIT_END();

    OsResult_t result = TaskPoll(ShellTxRingbuf, RINGBUF_EVENT_DATA_IN, 1000, true);
    if(result == OS_EVENT) {
        RingbufBase_t reply [100];
        U32_t read = 0;
        LOG_DEBUG_APPEND("Shell reply: ");
        read = RingbufDataCountGet(ShellTxRingbuf);
        if(read >= sizeof(reply)) {
            read = sizeof(reply);
        }
        RingbufRead(ShellTxRingbuf, reply, &read, OS_TIMEOUT_INFINITE);

        if(read) {
            U32_t i = 0;
            reply[read] = '\n';
            do {
                HalUartSendChar(&LoggerUartHandle, reply[i]);
                i++;
            } while (--read);
        }
    } else if(result == OS_TIMEOUT) {
        LOG_DEBUG_NEWLINE("Shell reply timed out");
    }

}

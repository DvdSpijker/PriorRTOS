/*
 * EventTimeoutTest.c
 *
 * Created: 30-8-2017 18:01:45
 *  Author: Dorus
 */


#include "EventTimeoutTest.h"
#include "../SysInit.h"
#include <PriorRTOS.h>

LOG_FILE_NAME("EventTimeoutTest.c");

#define FAKE_EVENT_0 EVENT_TYPE_CREATE | 0x0000CEEF
#define FAKE_EVENT_1 EVENT_TYPE_CREATE | 0x0000CEEB


void EventTimeoutTest(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);

    TASK_INIT_BEGIN() {
        //TaskJoin(TskSysInit, 1000);
    }
    TASK_INIT_END();

    //if(TaskPoll(INVALID_ID, FAKE_EVENT_0, 500) == OS_TIMEOUT) {
    //TaskPoll(INVALID_ID, FAKE_EVENT_0, 500);
    //LOG_DEBUG_NEWLINE("Event 0 timed out");
    //}
//
    //if(TaskPoll(INVALID_ID, FAKE_EVENT_1, 1000) == OS_TIMEOUT) {
    //TaskPoll(INVALID_ID, FAKE_EVENT_1, 1000);
    //LOG_DEBUG_NEWLINE("Event 1 timed out");
    //}

}
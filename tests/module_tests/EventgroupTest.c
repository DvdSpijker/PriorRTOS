/*
 * EventgroupTest.c
 *
 * Created: 27-7-2017 13:22:47
 *  Author: Dorus
 */


#include "EventgroupTest.h"
#include <PriorRTOS.h>

LOG_FILE_NAME("EventgroupTest.c");


#if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1

void EventgroupTestSet(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);

    static Id_t SharedEventgroup = OS_ID_INVALID;
    TASK_INIT_BEGIN() {
        SharedEventgroup = EventgroupCreate();
        TskEventgroupTestListen = TaskCreate(EventgroupTestListen, TASK_CAT_MEDIUM, 2, TASK_PARAM_NONE, 0, &SharedEventgroup, 0);
        TaskResumeWithVarg(TskEventgroupTestListen, 0);
    }
    TASK_INIT_END();

    EventgroupFlagsSet(SharedEventgroup, EVENTGROUP_FLAG_MASK_1);
// LOG_DEBUG_NEWLINE("Setting eventgroup flags");

    TaskSleep(1000);
}


void EventgroupTestListen(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    static Id_t SharedEventgroup = OS_ID_INVALID;
    TASK_INIT_BEGIN() {
        SharedEventgroup = *(Id_t *)p_arg;

    }
    TASK_INIT_END();


    OsResult_t result = TaskPoll(SharedEventgroup, EVENTGROUP_EVENT_FLAG_SET(EVENTGROUP_FLAG_MASK_1), 1500, true);
    if(result == OS_EVENT) {
        EventgroupFlagsGet(SharedEventgroup, EVENTGROUP_FLAG_MASK_1);
        EventgroupFlagsClear(SharedEventgroup, EVENTGROUP_FLAG_MASK_1);
    } else if(result == OS_TIMEOUT) {
        LOG_DEBUG_NEWLINE("EventgroupFlagSet Event timed out.");
    }


}

#endif
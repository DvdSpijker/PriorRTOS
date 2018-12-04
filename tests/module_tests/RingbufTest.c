/*
 * RingbufTest.c
 *
 * Created: 29-7-2017 13:29:53
 *  Author: Dorus
 */

#include "RingbufTest.h"
#include <PriorRTOS.h>

LOG_FILE_NAME("RingbufTest.c")

const U8_t TestData[] = {'t', 'e', 's', 't', 'm', 's', 'g'};
RingbufBase_t Buffer[40];

void RingbufTestItc(const void *p_arg, U32_t v_arg);

void RingbufTest(const void *p_arg, U32_t v_arg)
{

    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    static U8_t test_data_index = 0;
    static Id_t tsk_ringbuf_test_itc = OS_ID_INVALID;
    static Id_t ringbuffer = OS_ID_INVALID;
    static Id_t evt_ringbuffer_data_out = OS_ID_INVALID;
    static U32_t amount = 4;

    TASK_INIT_BEGIN() {
        ringbuffer = RingbufCreate(Buffer, 40);
        if(ringbuffer == OS_ID_INVALID) {
            LOG_ERROR_NEWLINE("Failed to created Test Ringbuffer.");
        } else {
            LOG_DEBUG_NEWLINE("Test Ringbuffer created: id %04x", ringbuffer);
        }
        tsk_ringbuf_test_itc = TaskCreate(RingbufTestItc, TASK_CAT_HIGH, 2, TASK_PARAM_NONE, 0, (void **)&ringbuffer, 0);

        if(tsk_ringbuf_test_itc == OS_ID_INVALID) {
            LOG_ERROR_NEWLINE("Failed to created Task RingbufTestItc.");
        } else {
            TaskResumeWithVarg(tsk_ringbuf_test_itc, 0);
            LOG_DEBUG_NEWLINE("Task RingbufTestItc created: id %04x", tsk_ringbuf_test_itc);
        }

        init_done = 1;
        TaskSuspendSelf();
    }
    TASK_INIT_END();



    static U8_t iter = 0;
    OsResult_t result = TaskPoll(ringbuffer, RINGBUF_EVENT_DATA_OUT, 500, true);
    if(result == OS_EVENT) {
        LOG_DEBUG_NEWLINE("Received data out event.");
        if(iter == 1) {
            amount = 3;
            RingbufWrite(ringbuffer, (RingbufBase_t *)&TestData[4], &amount, OS_TIMEOUT_INFINITE);
        } else if(iter == 2) {
            LOG_DEBUG_NEWLINE("Test ended. Cleaning up...");
            TaskDelete(&tsk_ringbuf_test_itc);
            RingbufDelete(&ringbuffer);
            TaskDelete(&TskRingbufTest);
            LOG_DEBUG_NEWLINE("Done.");
        }
    } else if(result == OS_TIMEOUT) {
        LOG_DEBUG_NEWLINE("Read timed out.");
    }


}

void RingbufTestItc(const void *p_arg, U32_t v_arg)
{

    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    static Id_t ringbuffer = OS_ID_INVALID;
    static Id_t evt_ringbuffer_data_in = OS_ID_INVALID;
    static U8_t iter = 0;

    TASK_INIT_BEGIN() {
        ringbuffer = *((Id_t *)p_arg);
        TaskPoll(ringbuffer, RINGBUF_EVENT_DATA_IN, 2000,true);
    }
    TASK_INIT_END();

}
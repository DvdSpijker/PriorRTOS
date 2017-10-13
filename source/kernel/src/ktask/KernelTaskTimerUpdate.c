/*
 * KernelTaskTimerUpdate.c
 *
 * Created: 29-9-2017 9:28:23
 *  Author: Dorus
 */

#include <KernelTaskTimerUpdate.h>
#include <PriorRTOS.h>

#include <List.h>
#include <TaskDef.h>
#include <TimerDef.h>
#include <CoreDef.h>


/* This kernel task is created by the Timer module upon initialization.
 * It is responsible for updating all timers created by the Timer module.
 */
void KernelTaskTimerUpdate(const void *p_arg, U32_t v_arg)
{
    static U32_t sleep_time = 0;
    static U32_t last_micros = 0;

    TASK_INIT_BEGIN() {
        /* Sleep time depends on the timer resolution.
         * This value is passed upon creation of the task by TimerInit. */
        sleep_time = v_arg;
    }
    TASK_INIT_END();

    U32_t t_accu_us = 0;
    U32_t curr_micros = OsRuntimeMicrosGet();
    if(curr_micros == 0) {
        goto sleep;
    } else {
        if(curr_micros >= last_micros) {
            t_accu_us = curr_micros - last_micros;
        } else {
            t_accu_us = last_micros - curr_micros;
        }
        last_micros = curr_micros;
    }

    TimerUpdateAll(t_accu_us);
    ListIdBufferFillCycle(&TimerList);

sleep:
    TaskSleep(sleep_time);
}
/*
 * KernelTaskTimerUpdate.c
 *
 * Created: 29-9-2017 9:28:23
 *  Author: Dorus
 */

#include "kernel/inc/ktask/KernelTaskTimerUpdate.h"
#include "kernel/inc/TimerDef.h"

#include "include/PriorRTOS.h"


/* This kernel task is created by the Timer module upon initialization.
 * It is responsible for updating all timers created by the Timer module.
 */
void KernelTaskTimerUpdate(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    
    static U32_t sleep_time = 0;
    static U32_t micros = 0;

    TASK_INIT_BEGIN() {
        /* Sleep time depends on the timer resolution.
         * This value is passed upon creation of the task by TimerInit. */
        sleep_time = v_arg;
    }
    TASK_INIT_END();
   
    U32_t t_accu_us = OsRunTimeMicrosDelta(micros);
    if(t_accu_us == 0) {
        goto sleep;
    } 
    micros = OsRunTimeMicrosGet();
    KTimerUpdateAll(t_accu_us);

sleep:
    TaskSleep(sleep_time);
}

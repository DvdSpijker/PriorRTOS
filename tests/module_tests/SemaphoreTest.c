/*
 * SemaphoreTest.c
 *
 * Created: 27-7-2017 17:54:06
 *  Author: Dorus
 */

#include "SemaphoreTest.h"
#include <PriorRTOS.h>

Id_t Semaphore;
U32_t SharedMem;

LOG_FILE_NAME("SemaphoreTest.c");

#if PRTOS_CONFIG_ENABLE_SEMAPHORES==1

void SemaphoreTest(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    static Id_t EvtSemaphoreRelease = OS_ID_INVALID;

    TASK_INIT_BEGIN();
    Semaphore = SemaphoreCreate(SEMAPHORE_TYPE_MUTEX_BINARY, 0);
    TASK_INIT_END();

    LOG_DEBUG_NEWLINE("Attempting to acquire Semaphore %x", Semaphore);
    if(SemaphoreAcquire(Semaphore, OS_TIMEOUT_INFINITE) == OS_OK) {
        LOG_DEBUG_NEWLINE("Task: Acquired Semaphore %x", Semaphore);
        EvtSemaphoreRelease = OS_ID_INVALID;
        SharedMem = SharedMem - 1;
        LOG_DEBUG_NEWLINE("Task: SharedMem value: %u", SharedMem);
        SemaphoreRelease(Semaphore);
        LOG_DEBUG_NEWLINE("Task: Going to sleep");
        TaskSleep(500);
    } else {
        TaskPoll(Semaphore, SEM_EVENT_RELEASE, OS_TIMEOUT_INFINITE,true);
        LOG_DEBUG_NEWLINE("Task: Failed to acquire Semaphore %x, waiting for SemaphoreRelease Event %x", Semaphore, EvtSemaphoreRelease);
    }
}


void SemaphoreTestIntCallback(void)
{
    if(SemaphoreAcquire(Semaphore, OS_TIMEOUT_INFINITE) == OS_OK) {
        LOG_DEBUG_NEWLINE("Int: Acquired Semaphore %x", Semaphore);
        SharedMem = SharedMem + 1;
        LOG_DEBUG_NEWLINE("Int: SharedMem value: %u", SharedMem);
        SemaphoreRelease(Semaphore);
    } else {
        LOG_DEBUG_NEWLINE("Int: Failed to acquire Semaphore %x", Semaphore);
    }
}
#endif

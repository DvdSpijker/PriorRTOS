/*
 * KernelTaskIdle.c
 *
 * Created: 25-9-2017 20:40:55
 *  Author: Dorus
 */

#include <KernelTaskIdle.h>
#include <PriorRTOS.h>

#include <List.h>
#include <TaskDef.h>
#include <CoreDef.h>

#include <RecursiveLock.h>

void KernelTaskIdle(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    
    RecursiveLock_t lock;

    CoreFlagSet(CORE_FLAG_IDLE);
    while(CoreFlagGet(CORE_FLAG_IDLE)) {
        
        //RecursiveLockInit(&lock);
        //
        //RecursiveLockLock(&lock, LOCK_MODE_WRITE);
        //
        //RecursiveLockLock(&lock, LOCK_MODE_READ);
        //
        //RecursiveLockUnlock(&lock);
        
        ListIdBufferFillCycle(&TcbList);
    }
}
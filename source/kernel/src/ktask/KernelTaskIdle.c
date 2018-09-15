/*
 * KernelTaskIdle.c
 *
 * Created: 25-9-2017 20:40:55
 *  Author: Dorus
 */

#include "kernel/inc/ktask/KernelTaskIdle.h"

#include "kernel/inc/CoreDef.h"
#include "include/Task.h"

void KernelTaskIdle(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    
    KCoreFlagSet(CORE_FLAG_IDLE);
    while(KCoreFlagGet(CORE_FLAG_IDLE)) {
    
            
        
    }
    
    TaskSuspendSelf();
}

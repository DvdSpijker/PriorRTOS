/*
 * KernelTaskIdle.c
 *
 * Created: 25-9-2017 20:40:55
 *  Author: Dorus
 */

#include "../../inc/ktask/KernelTaskIdle.h"
#include <PriorRTOS.h>

#include <List.h>
#include <TaskDef.h>
#include <CoreDef.h>

void KernelTaskIdle(const void* p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    
    KCoreFlagSet(CORE_FLAG_IDLE);
    while(KCoreFlagGet(CORE_FLAG_IDLE)) {
    
            
        
    }
    
    TaskSuspendSelf();
}

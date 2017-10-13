/*
 * KernelTaskMemCheck.c
 *
 * Created: 26-9-2017 8:41:42
 *  Author: Dorus
 */

#include <KernelTaskIdle.h>
#include <PriorRTOS.h>

#include <List.h>
#include <TaskDef.h>
#include <CoreDef.h>



void KernelTaskMemCheck(const void *p_arg, U32_t v_arg)
{
    /* Check memory integrity of all pools. */
    /* Check memory integrity of all stacks. */
    /* If integrity is breached, throw exception. */
}
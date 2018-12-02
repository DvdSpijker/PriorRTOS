/*
 * SystemCall.c
 *
 * Created: 25-2-2018 13:34:27
 *  Author: Dorus
 */ 
#include "kernel/inc/SystemCall.h"

#include "include/Task.h"

OsResult_t KSysCallPoll(Id_t object_id, U32_t event, U32_t timeout_ms, bool add_poll)
{
	return TaskPoll(object_id, event, timeout_ms, add_poll);
}
OsResult_t KSysCallWait(Id_t object_id, U32_t event, U32_t timeout_ms)
{
#ifndef PRTOS_CONFIG_USE_SCHEDULER_PREEM
	OS_ARG_UNUSED(object_id);
	OS_ARG_UNUSED(event);
	OS_ARG_UNUSED(timeout_ms);

    return OS_RES_ERROR;
#else
	return TaskWait(object_id, event, timeout_ms);
#endif
}

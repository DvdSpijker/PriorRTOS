/*
 * SystemCall.h
 *
 * Created: 20-10-2017 21:25:19
 *  Author: Dorus
 */ 


#ifndef SYSTEM_CALL_H_
#define SYSTEM_CALL_H_

#include "include/OsTypes.h"
#include "include/PriorRTOSConfig.h"

#include "kernel/inc/Event.h"
#include "kernel/inc/TaskDef.h"
#include "kernel/inc/List.h"

#include <stdlib.h>

extern S8_t OsIsrNestCountGet(void);

OsResult_t KSysCallPoll(Id_t object_id, U32_t event, U32_t timeout_ms, bool add_poll);
OsResult_t KSysCallWait(Id_t object_id, U32_t event, U32_t timeout_ms);

#if(defined(PRTOS_CONFIG_USE_SCHEDULER_COOP)||defined(PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK))

/* System Call header for Cooperative handling.
 * Check if the task is listened to the event
 * with given object ID. If this is the case
 * the event is polled and the result is placed in
 * p_res (pointer to a OsResult variable).
 */
#define SYS_CALL_EVENT_HANDLE(obj_id, evt, p_res) \
*p_res = OS_RES_ERROR; \
if(OsIsrNestCountGet() == 0) { \
	*p_res = KSysCallPoll(obj_id, evt, OS_TIMEOUT_INFINITE, false); \
} else { \
	*p_res = OS_RES_FAIL; \
} \
if(*p_res == OS_RES_EVENT) \

#else

/* System Call header for Pre-Emptive handling. 
 * Contains the label 'preem_ret' which is 
 * jumped to when returning from TaskWait and
 * the event has occurred. */
#define SYS_CALL_EVENT_HANDLE(obj_id, evt, p_res) \
wait_ret:
#endif

#define SYS_CALL_EVENT_HANDLE_TIMEOUT(p_res) \
else if(*p_res == OS_RES_TIMEOUT) \

#define SYS_CALL_EVENT_HANDLE_POLL(p_res) \
else if(*p_res == OS_RES_POLL) \


/* Registers to the specified event
 * from the specified object ID with given timeout. 
 * The poll/wait result is placed in p_res. 
 */
#if (defined(PRTOS_CONFIG_USE_SCHEDULER_COOP)||defined(PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK))

#define SYS_CALL_EVENT_REGISTER(ls_node, obj_id, evt, p_res, t_out) \
*p_res = OS_RES_ERROR; \
if(OsIsrNestCountGet() == 0) { \
*p_res = KSysCallPoll(obj_id, evt, t_out, true); \
} \

#else
#define SYS_CALL_EVENT_REGISTER(ls_node, obj_id, evt, p_res, t_out) \
*p_res = OS_RES_ERROR;
if(OsIsrNestCountGet() == 0) { \
ListNodeUnlock(ls_node); \
*p_res = KSysCallWait(obj_id, evt, t_out); \
if(*p_res == OS_RES_EVENT) { \
    goto preem_ret; \
}} \

#endif


#endif /* SYSTEM_CALL_H_ */

/*
 * SystemCall.h
 *
 * Created: 20-10-2017 21:25:19
 *  Author: Dorus
 */ 


#ifndef SYSTEM_CALL_H_
#define SYSTEM_CALL_H_

#include <stdlib.h>
#include <Event.h>
#include <TaskDef.h>
#include <List.h>
#include <OsTypes.h>

#include <PriorRTOSConfig.h>

extern S8_t OsIsrNestCountGet(void);

OsResult_t KSysCallPoll(Id_t object_id, U32_t event, U32_t timeout_ms, bool add_poll);
OsResult_t KSysCallWait(Id_t object_id, U32_t event, U32_t timeout_ms);

/* System Call header for Pre-Emptive handling. 
 * Contains the label 'preem_ret' which is 
 * jumped to when returning from TaskWait and
 * the event has occurred. */
#if (defined(PRTOS_CONFIG_USE_SCHEDULER_PREEM) && !defined(PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK))  

#define SYSTEM_CALL_WAIT_HANDLE_EVENT               \
preem_ret:                                          \

#else   

#define SYSTEM_CALL_WAIT_HANDLE_EVENT ;   
        
#endif                                              

/* System Call header for Cooperative handling. 
 * Check if the task is listened to the event
 * with given object ID. If this is the case 
 * the event is polled and the result is placed in 
 * p_res (pointer to a OsResult variable).
 */
#if (defined(PRTOS_CONFIG_USE_SCHEDULER_COOP)||defined(PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK))

#define SYSTEM_CALL_POLL_HANDLE_EVENT(obj_id, evt, p_res)               \
if(OsIsrNestCountGet() == 0) {                                          \           
    *p_res = KSysCallPoll(obj_id, evt, OS_TIMEOUT_INFINITE, false);         \
} else {                                                                \
    *p_res = OS_RES_FAIL;                                                   \       
}                                                                       \
if(*p_res == OS_RES_EVENT)                                                  \

#define SYSTEM_CALL_POLL_HANDLE_TIMEOUT(p_res) \               
else if(*p_res == OS_RES_TIMEOUT)           \

#define SYSTEM_CALL_POLL_HANDLE_POLL(p_res) \
else if(*p_res == OS_RES_POLL)           \

#else

#define SYSTEM_CALL_POLL_HANDLE_EVENT(obj_id, evt, p_res) ;    
#define SYSTEM_CALL_POLL_HANDLE_TIMEOUT(p_res) ;
#define SYSTEM_CALL_POLL_HANDLE_POLL(p_res) ;           

#endif                                                                          

/* Polls or Waits (depends on the configuration of the OS) for the specified event 
 * from the specified object ID with given timeout. 
 * The poll/wait result is placed in p_res. 
 */
#if (defined(PRTOS_CONFIG_USE_SCHEDULER_COOP)||defined(PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK))

#define SYSTEM_CALL_POLL_WAIT_EVENT(ls_node, obj_id, evt, p_res, t_out)           \
if(OsIsrNestCountGet() == 0) {                                                    \
*p_res = KSysCallPoll(obj_id, evt, t_out, true);                                  \
}                                                                                 \

#else  
     
#define SYSTEM_CALL_POLL_WAIT_EVENT(ls_node, obj_id, evt, p_res, t_out)           \
if(OsIsrNestCountGet() == 0) {                                                    \
ListNodeUnlock(ls_node);                                                          \
*p_res = KSysCallWait(obj_id, evt, t_out);                                        \                   
if(*p_res == OS_RES_EVENT) {                                                      \   
    goto preem_ret;                                                               \
}}                                                                                \
                                                                    
                                                                             
#endif                                                                              



#endif /* SYSTEM_CALL_H_ */
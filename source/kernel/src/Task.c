/**********************************************************************************************************************************************
 *  File: Prior_tasks.c
 *
 *  Description: Prior Task module
 *
 *  OS Version: V0.4
 *  Date: 13/03/2015
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/

/*********OS Task management*********/

#include <LoggerDef.h>
#include <Types.h>
#include <Convert.h>
#include <Task.h>
#include <Event.h>
#include <TaskDef.h>
#include <MemoryDef.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

LOG_FILE_NAME("Task.c");


#define TASK_EVENT_SLEEP (EVENT_TYPE_STATE_CHANGE | 0x00002000)
static OsResult_t ITaskListen(pTcb_t tcb, Id_t object_id, U32_t event, U8_t flags, U32_t timeout_ms, Id_t *event_id);

extern U32_t OsRunTimeMicrosGet(void);

extern void TaskGenericNameSet(Id_t task_id, const char* gen_name);

/* -3 Event does not exist.
 * -1 Event has not occurred.
 * 0  Event has occurred.
 * 1  Event has timed out */
static S8_t ITaskEventStateGet(pEvent_t event);

static S8_t ITaskEventHandle(pEvent_t event);

static OsResult_t ITaskResume(Id_t task_id);

OsResult_t KTaskInit(void)
{
    ListInit(&TcbList, (Id_t)ID_TYPE_TASK);
    ListInit(&TcbWaitList, (Id_t)ID_TYPE_TASK);

    //LOG_INFO_NEWLINE("TcbList: %p", &TcbList);
    //LOG_INFO_NEWLINE("TcbWaitList: %p", &TcbWaitList);

    TcbRunning = TcbIdle = NULL;

    return OS_RES_OK;
}


Id_t TaskCreate(Task_t handler, TaskCat_t category, Prio_t priority, U8_t param,
                U32_t stack_size, const void *p_arg, U32_t v_arg)
{

#ifndef PRTOS_CONFIG_USE_SCHEDULER_PREEM
    OS_ARG_UNUSED(stack_size);
#endif

    if (priority < 1 || priority > 5) {
        return OS_ID_INVALID;
    }
    if (category > TASK_CAT_OS) {
        return OS_ID_INVALID;
    }
    if (category == TASK_CAT_OS && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return OS_ID_INVALID;
    }

    volatile pTcb_t new_TCB;

    /* Use data section of ObjectAlloc for Task stack in the future. */
    new_TCB = (pTcb_t)KMemAllocObject(sizeof(Tcb_t), 0, NULL);
    if (new_TCB == NULL) {
        LOG_ERROR_NEWLINE("Failed to allocate memory for a task.");
        return OS_ID_INVALID;
    }

    OsResult_t result;
    ListNodeInit(&new_TCB->list_node, (void*)new_TCB);
    result = ListNodeAddSorted(&TcbList, &new_TCB->list_node);

    if(result != OS_RES_OK) {
        LOG_ERROR_NEWLINE("A task could not be added to the list.");
        ListNodeDeinit(&TcbList, &new_TCB->list_node);
        KMemFreeObject((void **)&new_TCB, NULL);
        return OS_ID_INVALID;
    }
    ListInit(&new_TCB->event_list, 0);
    new_TCB->handler = handler;
    new_TCB->priority = KCalculatePriority(category, priority);
    new_TCB->category = category;
    new_TCB->p_arg = p_arg;
    new_TCB->v_arg = v_arg;
    new_TCB->state = TASK_STATE_IDLE;
    new_TCB->active_time_us = 0;
    new_TCB->run_time_us = 0;
    new_TCB->deadline_time_us = PRTOS_CONFIG_REAL_TIME_TASK_DEADLINE_DEFAULT_MS;

    new_TCB->flags = 0;

    /* Handle task creation parameters. */
    if(param != TASK_PARAM_NONE)  {
        if(param & TASK_PARAM_ESSENTIAL) {
            KTaskFlagSet(new_TCB, TASK_FLAG_ESSENTIAL);
        }
        if(param & TASK_PARAM_NO_PREEM) {
            KTaskFlagSet(new_TCB, TASK_FLAG_NO_PREEM);
        }
        if(param & TASK_PARAM_START) {
            TaskResumeWithVarg(new_TCB->list_node.id, v_arg);
        }
    }
#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
    EventEmit(new_TCB->list_node.id, TASK_EVENT_CREATE, EVENT_FLAG_NONE);
#endif
    LOG_INFO_NEWLINE("Task created: %04x", new_TCB->list_node.id);
    return new_TCB->list_node.id;
}

OsResult_t TaskDelete(Id_t *task_id)
{

    pTcb_t tcb = NULL;
    OsResult_t result = OS_RES_ERROR;

    if(task_id == NULL) {
        tcb = TcbRunning;
    } else {
        if (*task_id == TcbRunning->list_node.id) {
            tcb = TcbRunning;
        } else {
            tcb = KTcbFromId(*task_id);
        }
        *task_id = OS_ID_INVALID;
    }

    if(tcb != NULL) {
        KTaskFlagSet(tcb, TASK_FLAG_DELETE);

#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
        EventEmit(task_id, TASK_EVENT_DELETE, EVENT_FLAG_NONE);
#endif
        result = OS_RES_OK;
        LOG_INFO_NEWLINE("Deleted task %04x", tcb->list_node.id);
    }

    return result;
}

#if PRTOS_CONFIG_ENABLE_TASKNAMES>0
void TaskGenericNameSet(Id_t task_id, const char* gen_name)
{
    pTcb_t tcb = NULL;
    OsResult_t result = OS_RES_ERROR;

    if (task_id == OS_ID_INVALID) {
        tcb = TcbRunning;
    } else {
        tcb = KTcbFromId(task_id);
    }

    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {
            strcpy(tcb->generic_name,gen_name);
            result = OS_RES_OK;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;
}

#endif


OsResult_t TaskRealTimeDeadlineSet(Id_t rt_task_id, U32_t t_ms)
{
    OsResult_t result = OS_RES_ERROR;
    if(rt_task_id == OS_ID_INVALID) {
        return OS_RES_ID_INVALID;
    }
    pTcb_t tcb = KTcbFromId(rt_task_id);
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {
            result = OS_RES_ID_INVALID;
            if(tcb->category == TASK_CAT_REALTIME) {
                tcb->deadline_time_us = t_ms;
                result = OS_RES_OK;
            }
            ListNodeUnlock(&tcb->list_node);
        }
    }

    return result;
}

OsResult_t TaskResumeWithVarg(Id_t task_id, U32_t v_arg)
{
    if(task_id == OS_ID_INVALID) {
        return OS_RES_ID_INVALID;
    }
    OsResult_t result = OS_RES_ERROR;
    pTcb_t tcb = KTcbFromId(task_id);
    if(tcb != NULL) {
        tcb->v_arg = v_arg;
        result = ITaskResume(task_id);
    }
    return result;
}

OsResult_t TaskResume(Id_t task_id)
{
    if(task_id == OS_ID_INVALID) {
        return OS_RES_ID_INVALID;
    }
    OsResult_t result = ITaskResume(task_id);
    return result;
}


#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM


OsResult_t TaskSuspend(Id_t task_id)
{
    return OS_RES_OK;
}

#endif



Id_t TaskPollAdd(Id_t object_id, U32_t event, U32_t timeout_ms)
{
    OsResult_t result = OS_RES_ERROR;
    Id_t event_id = OS_ID_INVALID;
    result = ITaskListen(TcbRunning, object_id, event, EVENT_FLAG_PERMANENT, timeout_ms, &event_id);
    if(result == OS_RES_OK) {
        return event_id;
    }
    return OS_ID_INVALID;
}

OsResult_t TaskPollRemove(Id_t object_id, U32_t event)
{
    OsResult_t result = OS_RES_ERROR;
    pEvent_t evt = EventListContainsEvent(&TcbRunning->event_list, object_id, event);
    if(evt != NULL) {
        result = EventDestroy(&TcbRunning->event_list, evt);
    }

    return result;
}


OsResult_t TaskPoll(Id_t object_id, U32_t event, U32_t timeout_ms, bool add_poll)
{
    OsResult_t result = OS_RES_ERROR;

    /* Check if the task is already listened to this event. */
    pEvent_t sub_event = EventListContainsEvent(&TcbRunning->event_list, object_id, event);

    if(sub_event == NULL) {
        if(add_poll == true) {
            goto new_poll;
        } else {
            result = OS_RES_FAIL;
        }
    } else {
        S8_t event_state = ITaskEventStateGet(sub_event);
        if(event_state == -1) {
            result = OS_RES_POLL;
        } else if(event_state == 0) {
            ITaskEventHandle(sub_event);
            result = OS_RES_EVENT;
        } else if(event_state == 1) {
            ITaskEventHandle(sub_event);
            result = OS_RES_TIMEOUT;
        } else {
            result = OS_RES_ERROR;
        }
        if(add_poll == true) {
            goto new_poll;
        }
    }

    /* Return without adding a new poll. */
    return result;


new_poll:
    /* Add a new poll and return. */
    result = ITaskListen(TcbRunning, object_id, event, EVENT_FLAG_NONE, timeout_ms, NULL);
    if(result == OS_RES_OK) {
        result = OS_RES_POLL;
    }

    return result;
}


OsResult_t TaskJoin(Id_t task_id, U32 timeout)
{
    OsResult_t result = OS_RES_ERROR;

#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM
#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
    result = TaskWait(task_id, TASK_EVENT_DELETE, timeout);
#endif
#else
#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
    result = TaskPoll(task_id, TASK_EVENT_DELETE, timeout, true);
#endif
#endif

    return result;
}

OsResult_t TaskSleep(U32_t t_ms)
{
    /* Sleeping is achieved by waiting/polling for
     * an event that will never occur with a timeout.
     * The timeout is the sleep-time. */
    OsResult_t result = OS_RES_OK;
    if(t_ms > 0) {
        result = TaskPoll(OS_ID_INVALID, TASK_EVENT_SLEEP, t_ms, true);
    } else {
        result = OS_RES_OUT_OF_BOUNDS;
    }

    return result;
}


OsResult_t TaskPrioritySet(Id_t task_id, Prio_t new_priority)
{
    /* Validate priority range. */
    if (new_priority < 1 || new_priority > 5) {
        return OS_RES_OUT_OF_BOUNDS;
    }

    pTcb_t tcb = NULL;
    OsResult_t result = OS_RES_ERROR;

    if (task_id == OS_ID_INVALID) {
        tcb = TcbRunning;
    } else {
        tcb = KTcbFromId(task_id);
    }

    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {
            /* The priority stored in the TCB is a combined priority of the minor and category.
             * The original minor priority can be extracted. */
            tcb->priority = KCalculatePriority(tcb->category, new_priority);
            result = OS_RES_OK;
            ListNodeUnlock(&tcb->list_node);
        }
    }

    return result;
}

Prio_t TaskPriorityGet(Id_t task_id)
{
    pTcb_t tcb = NULL;
    if (task_id == OS_ID_INVALID) {
        tcb = TcbRunning;
    } else {
        tcb = KTcbFromId(task_id);
    }
    Prio_t priority = 0;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
            priority = tcb->priority;
            ListNodeUnlock(&tcb->list_node);
        }
    }

    return priority;
}

TaskState_t TaskStateGet(Id_t task_id)
{
    pTcb_t tcb = NULL;
    if (task_id == OS_ID_INVALID) {
        tcb = TcbRunning;
    } else {
        tcb = KTcbFromId(task_id);
    }

    TaskState_t state = TASK_STATE_UNDEFINED;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
            state = tcb->state;
            ListNodeUnlock(&tcb->list_node);
        }
    }
    return state;
}


U32_t TaskRunTimeGet(void)
{
    U32_t runtime_us = 0;
    if(ListNodeLock(&TcbRunning->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
        runtime_us = TcbRunning->run_time_us;
        ListNodeUnlock(&TcbRunning->list_node);
    }

    return runtime_us;
}


static S8_t ITaskEventStateGet(pEvent_t event)
{

    S8_t result = 0;
    if(event != NULL) {
        if(ListNodeLock(&event->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
            if(EventHasOccurred(event)) {
                result = 0;
            } else if (EventHasTimedOut(event)) {
                result = 1;
            } else {
                result = -1;
            }
            ListNodeUnlock(&event->list_node);
        }
    } else {
        result = -3;
    }


    return result;
}




static S8_t ITaskEventHandle(pEvent_t event)
{
    S8_t result = -1;
    pTcb_t tcb = TcbRunning;
    LinkedList_t *list = NULL;
    if(tcb != NULL) {
        list = &tcb->event_list;
    }

    if(event != NULL) {
        if(ListNodeLock(&event->list_node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {
            if(EventFlagGet(event, EVENT_FLAG_PERMANENT)) {
                /* If the event is permanent, handle the event (resets flags etc.). */
                EventHandle(event);
            } else { /* Delete event if not permanent. */
                EventDestroy(list, event);
            }
        }
        result = 0;
        ListNodeUnlock(&event->list_node);
    }

    return result;

}

static OsResult_t ITaskResume(Id_t task_id)
{
    /* TODO: Check if the task is blocked by a waiting event. */
    OsResult_t result = EventEmit(task_id, TASK_EVENT_ACTIVATE, EVENT_FLAG_ADDRESSED);
    return result;
}


/********************************/


void KTaskStateSet(pTcb_t tcb_pointer, TaskState_t new_state)
{
    tcb_pointer->state = new_state;
}


void KTaskFlagSet(pTcb_t tcb, TaskFlags_t flag)
{
    tcb->flags |= (U8_t)flag;
}

void KTaskFlagClear(pTcb_t tcb ,TaskFlags_t flag)
{
    tcb->flags &= ~((U8_t)flag);
}

U8_t KTaskFlagGet(pTcb_t tcb, TaskFlags_t flag)
{
    return (tcb->flags & ((U8_t)flag));
}


Prio_t KCalculateInvPriority(Prio_t P, TaskCat_t Mj)
{
    if(Mj > 0) {
        return P/(Mj*5) + 1;
    } else {
        return P+1;
    }
}

Prio_t KCalculatePriority(TaskCat_t Mj,Prio_t Mi)
{
    return (Mj * 5 + Mi - 1);
}


LinkedList_t* KTcbLocationGet(pTcb_t tcb)
{
    LinkedList_t *list = &TcbList;

    if(ListSearch(list, tcb->list_node.id) != NULL) {
        return list;
    }

    list = &TcbWaitList;
    if(ListSearch(list, tcb->list_node.id) != NULL) {
        return list;
    }

    list = &ExecutionQueue;
    if(ListSearch(list, tcb->list_node.id) != NULL) {
        return list;
    }

    return NULL;
}


pTcb_t KTcbFromId(Id_t task_id)
{
    ListNode_t *node = ListSearch(&TcbList, task_id);
    if(node != NULL) {
        return (pTcb_t)ListNodeChildGet(node);
    }
    node = ListSearch(&TcbWaitList, task_id);
    if(node != NULL) {
        return (pTcb_t)ListNodeChildGet(node);
    }
    return NULL;
}

pTcb_t KTaskRunningGet(void)
{
    return TcbRunning;
}

OsResult_t KTcbMove(pTcb_t to_move, LinkedList_t *from_list, LinkedList_t* to_list)
{
    if(!KCoreFlagGet(CORE_FLAG_KERNEL_MODE)) {
        return OS_RES_RESTRICTED;
    }

    if(to_move == NULL) {
        return OS_RES_NULL_POINTER;
    }
    return ListNodeMove(from_list, to_list, &to_move->list_node);
}

void KTcbSwap(pTcb_t x, pTcb_t y, LinkedList_t *list)
{
    ListNodeSwap(list, &x->list_node, &y->list_node);
}


void KTcbDestroy(pTcb_t tcb, LinkedList_t *list)
{
    ListNode_t *node = &tcb->list_node;
    if(list == NULL) {
        ListNodeAddSorted(&TcbList, node);
    } else if(list != &TcbList) {
        ListNodeMove(list, &TcbList, node);
    }
    EventListDestroy(&tcb->event_list);
    ListNodeDeinit(&TcbList, node);
    KMemFreeObject((void**)&tcb, NULL);
}


static OsResult_t ITaskListen(pTcb_t tcb, Id_t object_id, U32_t event, U8_t flags, U32_t timeout_ms, Id_t *event_id)
{
    OsResult_t result = OS_RES_ERROR;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_RES_OK) {
            result = EventListen(&tcb->event_list, object_id, event, flags, ConvertMsToUs(timeout_ms), event_id);
            ListNodeUnlock(&tcb->list_node);
        }
    }
    return result;
}

/* Adds t_us to the task's runtime.  */
void KTaskRunTimeUpdate(void)
{
    static U32_t last_micros = 0;
    U32_t t_accu_us = 0;
    U32_t curr_micros = OsRunTimeMicrosGet();

    if(curr_micros == 0) {
        return;
    }

    if(curr_micros >= last_micros) {
        t_accu_us = curr_micros - last_micros;
    } else {
        t_accu_us = last_micros - curr_micros;
    }
    last_micros = curr_micros;

    if(t_accu_us != 0) {
        TcbRunning->run_time_us += t_accu_us;
    }
}

void KTaskRunTimeReset(pTcb_t tcb)
{
    tcb->run_time_us = 0;
}
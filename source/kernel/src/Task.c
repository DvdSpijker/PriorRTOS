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

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

LOG_FILE_NAME("Task.c");

/* Free task ID buffer */
#define TASK_ID_BUFFER_SIZE 4
Id_t TaskIdBuffer[TASK_ID_BUFFER_SIZE];

#define TASK_EVENT_SLEEP (EVENT_TYPE_STATE_CHANGE | 0x00002000)
static OsResult_t UtilTaskSubscribe(pTcb_t tcb, Id_t object_id, U32_t event, U8_t flags, U32_t timeout_ms, Id_t *out_event_id);

extern void CoreTaskAddDescendingPriority(LinkedList_t *from_list, LinkedList_t *to_list, pTcb_t task);
extern void TaskGenericNameSet(Id_t task_id, const char* gen_name);


OsResult_t TaskInit(void)
{
    ListInit(&TcbList, (Id_t)ID_TYPE_TASK, TaskIdBuffer, TASK_ID_BUFFER_SIZE);
    ListInit(&TcbWaitList, (Id_t)ID_TYPE_TASK, NULL, 0);

    //LOG_INFO_NEWLINE("TcbList: %p", &TcbList);
    //LOG_INFO_NEWLINE("TcbWaitList: %p", &TcbWaitList);

    TcbRunning = TcbIdle = NULL;

    return OS_OK;
}


Id_t TaskCreate(Task_t handler, TaskCat_t category, Prio_t priority, U8_t param,
                U32_t stack_size, const void *p_arg, U32_t v_arg)
{

#ifndef PRTOS_CONFIG_USE_SCHEDULER_PREEM
    OS_ARG_UNUSED(stack_size);
#endif

    if (priority < 1 || priority > 5) {
        return INVALID_ID;
    }
    if (category > TASK_CAT_OS) {
        return INVALID_ID;
    }
    if (category == TASK_CAT_OS && CoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return INVALID_ID;
    }

    volatile pTcb_t new_TCB;

    /* Use data section of ObjectAlloc for Task stack in the future. */
    new_TCB = (pTcb_t)CoreObjectAlloc(sizeof(Tcb_t), 0, NULL);
    if (new_TCB == NULL) {
        LOG_ERROR_NEWLINE("Failed to allocate memory for a task.");
        return INVALID_ID;
    }

    OsResult_t result;
    ListNodeInit(&new_TCB->list_node, (void*)new_TCB);
    result = ListNodeAddSorted(&TcbList, &new_TCB->list_node);

    if(result != OS_OK) {
        LOG_ERROR_NEWLINE("A task could not be added to the list.");
        ListNodeDeinit(&TcbList, &new_TCB->list_node);
        CoreObjectFree((void **)&new_TCB, NULL);
        return INVALID_ID;
    }
    ListInit(&new_TCB->event_list, 0, NULL, 0);
    new_TCB->handler = handler;
    new_TCB->priority = UtilCalculatePriority(category, priority);
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
            UtilTaskFlagSet(new_TCB, TASK_FLAG_ESSENTIAL);
        }
        if(param & TASK_PARAM_NO_PREEM) {
            UtilTaskFlagSet(new_TCB, TASK_FLAG_NO_PREEM);
        }
        if(param & TASK_PARAM_INSTANT_WAKE) {
            TaskWake(new_TCB->list_node.id, v_arg);
        }
    }
#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
    EventPublish(new_TCB->list_node.id, TASK_EVENT_CREATE, EVENT_FLAG_NONE);
#endif
    LOG_INFO_NEWLINE("Task created with ID %04x", new_TCB->list_node.id);
    return new_TCB->list_node.id;
}

OsResult_t TaskDelete(Id_t *task_id)
{

    pTcb_t tcb = NULL;
    OsResult_t result = OS_ERROR;

    if(task_id == NULL) {
        tcb = TcbRunning;
    } else {
        if (*task_id == TcbRunning->list_node.id) {
            tcb = TcbRunning;
        } else {
            tcb = UtilTcbFromId(*task_id);
        }
        *task_id = INVALID_ID;
    }

    if(tcb != NULL) {
        UtilTaskFlagSet(tcb, TASK_FLAG_DELETE);
#ifdef PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
        EventPublish(task_id, TASK_EVENT_DELETE, EVENT_FLAG_NONE);
#endif
        result = OS_OK;
        LOG_INFO_NEWLINE("Deleted task %04x", tcb->list_node.id);
    }

    return result;
}

#if PRTOS_CONFIG_ENABLE_TASKNAMES>0
void TaskGenericNameSet(Id_t task_id, const char* gen_name)
{
    pTcb_t tcb = NULL;
    OsResult_t result = OS_ERROR;

    if (task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }

    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            strcpy(tcb->generic_name,gen_name);
            result = OS_OK;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;
}

#endif


OsResult_t TaskRealTimeDeadlineSet(Id_t rt_task_id, U32_t t_ms)
{
    OsResult_t result = OS_ERROR;
    if(rt_task_id == INVALID_ID) {
        return OS_INVALID_ID;
    }
    pTcb_t tcb = UtilTcbFromId(rt_task_id);
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            result = OS_INVALID_ID;
            if(tcb->category == TASK_CAT_REALTIME) {
                tcb->deadline_time_us = t_ms;
                result = OS_OK;
            }
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;
}

OsResult_t TaskWake(Id_t task_id, U32_t v_arg)
{
    if(task_id == INVALID_ID) {
        return OS_INVALID_ID;
    }
    OsResult_t result = OS_ERROR;
    pTcb_t tcb = UtilTcbFromId(task_id);
    if(tcb != NULL) {
        tcb->v_arg = v_arg;
        result = EventPublish(task_id, TASK_EVENT_ACTIVATE, (EVENT_FLAG_ADDRESSED | EVENT_FLAG_NO_HANDLER));
    }    
    return result;
}


#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM


OsResult_t TaskSuspend(Id_t task_id)
{
    return OS_OK;
}

void TaskResume(Id_t task_id)
{

}

#endif



OsResult_t TaskPoll(Id_t object_id, U32_t event, U8_t flags, U32_t timeout_ms, Id_t *out_event_id)
{
    /* Acquire correct tcb. */
    OsResult_t result = OS_ERROR;
    result = UtilTaskSubscribe(TcbRunning, object_id, event, flags, timeout_ms, out_event_id);
    return result;
}


OsResult_t TaskSleep(U32_t t_ms)
{
    OsResult_t result = OS_OK;
    if(t_ms > 0) {
        Id_t sleep_event;
        result = TaskPoll(INVALID_ID, TASK_EVENT_SLEEP, EVENT_FLAG_NO_HANDLER, t_ms, &sleep_event);
    } else {
        result = OS_OUT_OF_BOUNDS;
    }

    return result;
}


OsResult_t TaskPrioritySet(Id_t task_id, Prio_t new_priority)
{
    if (new_priority < 1 || new_priority > 5) {
        return OS_OUT_OF_BOUNDS;
    }

    pTcb_t tcb = NULL;
    OsResult_t result = OS_ERROR;

    if (task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }

    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            tcb->priority = UtilCalculatePriority(tcb->category, new_priority);
            result = OS_OK;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;
}

Prio_t TaskPriorityGet(Id_t task_id)
{
    pTcb_t tcb = NULL;
    if (task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }
    Prio_t priority = 0;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_READ) == OS_OK) {
            priority = tcb->priority;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_READ);
        }
    }

    return priority;
}

TaskState_t TaskStateGet(Id_t task_id)
{
    pTcb_t tcb = NULL;
    if (task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }

    TaskState_t state = TASK_STATE_UNDEFINED;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_READ) == OS_OK) {
            state = tcb->state;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_READ);
        }
    }
    return state;
}

OsResult_t TaskCategorySet(Id_t task_id, TaskCat_t new_cat)
{
    if(new_cat == TASK_CAT_OS) {
        if(CoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
            return OS_RESTRICTED;
        }
    }
    if(new_cat > TASK_CAT_LOW) {
        return OS_OUT_OF_BOUNDS;
    }

    OsResult_t result = OS_ERROR;
    pTcb_t tcb = NULL;
    if(task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }

    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            tcb->priority = UtilCalculateInvPriority(tcb->priority, tcb->category);
            tcb->category = new_cat;
            tcb->priority = UtilCalculatePriority(new_cat, tcb->priority);
            result = OS_OK;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;
}

Id_t TaskIdGet(void)
{
    Id_t id = INVALID_ID;
    if(ListNodeLock(&TcbRunning->list_node, LIST_LOCK_MODE_READ) == OS_OK) {
        id = ListNodeIdGet(&TcbRunning->list_node);
        ListNodeUnlock(&TcbRunning->list_node, LIST_LOCK_MODE_READ);
    }
    return id;
}



U32_t TaskRuntimeGet(Id_t task_id)
{
    U32_t runtime_us = 0;
    pTcb_t tcb = NULL;

    if(task_id == INVALID_ID) {
        tcb = TcbRunning;
    } else {
        tcb = UtilTcbFromId(task_id);
    }
    if(tcb != NULL) {
        if(ListNodeLock(&TcbRunning->list_node, LIST_LOCK_MODE_READ) == OS_OK) {
            runtime_us = tcb->run_time_us;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_READ);
        }
    }
    return runtime_us;
}


S8_t TaskEventStateGet(Id_t event_id)
{
    S8_t result = 0;
    pTcb_t tcb = TcbRunning;
    static pEvent_t occ_event =  NULL;
    static LinkedList_t *list = NULL;
    if(tcb != NULL) {
        list = &tcb->event_list;
    }

    if(list->size == 0) {
        result = -2;
    }

    if(result == 0) {
        occ_event = EventFromId(list, event_id);
        if(occ_event != NULL) {
            if(ListNodeLock(&occ_event->list_node, LIST_LOCK_MODE_READ) == OS_OK) {
                if(EventHasOccurred(occ_event)) {
                    result = 0;
                } else if (EventHasTimedOut(occ_event)) {
                    result = 1;
                } else {
                    result = -1;
                }
                ListNodeUnlock(&occ_event->list_node, LIST_LOCK_MODE_READ);
            }
        } else {
            result = -3;
        }
    }

    return result;
}


S8_t TaskEventHandle(Id_t event_id)
{
    S8_t result = -1;
    pTcb_t tcb = TcbRunning;
    LinkedList_t *list = NULL;
    static pEvent_t occ_event =  NULL;
    if(tcb != NULL) {
        list = &tcb->event_list;
    }

    occ_event = EventFromId(list, event_id);
    if(occ_event != NULL) {
        if(ListNodeLock(&occ_event->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            if(EVENT_FLAG_GET(occ_event->event_code, EVENT_FLAG_PERMANENT)) {
                EVENT_FLAG_CLEAR(occ_event->event_code, EVENT_FLAG_OCCURRED);
                EVENT_FLAG_CLEAR(occ_event->event_code, EVENT_FLAG_TIMED_OUT);
                EventLifeTimeReset(occ_event);
                EventOccurrenceCountReset(occ_event);
            } else {
                EventHandleFromId(list, event_id);
                EventDestroy(list, occ_event);
            }
            result = 0;
            ListNodeUnlock(&occ_event->list_node, LIST_LOCK_MODE_WRITE);
        }
    }

    return result;

}


/********************************/


void UtilTaskStateSet(pTcb_t tcb_pointer, TaskState_t new_state)
{
    tcb_pointer->state = new_state;
}


void UtilTaskFlagSet(pTcb_t tcb, TaskFlags_t flag)
{
    tcb->flags |= (U8_t)flag;
}

void UtilTaskFlagClear(pTcb_t tcb ,TaskFlags_t flag)
{
    tcb->flags &= ~((U8_t)flag);
}

U8_t UtilTaskFlagGet(pTcb_t tcb, TaskFlags_t flag)
{
    return (tcb->flags & ((U8_t)flag));
}


Prio_t UtilCalculateInvPriority(Prio_t P, TaskCat_t Mj)
{
    if(Mj > 0) {
        return P/(Mj*5) + 1;
    } else {
        return P+1;
    }
}

Prio_t UtilCalculatePriority(TaskCat_t Mj,Prio_t Mi)
{
    return (Mj * 5 + Mi - 1);
}


LinkedList_t* UtilTcbLocationGet(pTcb_t tcb)
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


pTcb_t UtilTcbFromId(Id_t task_id)
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

OsResult_t UtilTcbMove(pTcb_t to_move, LinkedList_t *from_list, LinkedList_t* to_list)
{
    if(!CoreFlagGet(CORE_FLAG_KERNEL_MODE)) {
        return OS_RESTRICTED;
    }

    if(to_move == NULL) {
        return OS_NULL_POINTER;
    }
    return ListNodeMove(from_list, to_list, &to_move->list_node);
}

void UtilTcbSwap(pTcb_t x, pTcb_t y, LinkedList_t *list)
{
    ListNodeSwap(list, &x->list_node, &y->list_node);
}


void UtilTcbDestroy(pTcb_t tcb, LinkedList_t *list)
{
    ListNode_t *node = &tcb->list_node;
    if(list == NULL) {
        ListNodeAddSorted(&TcbList, node);
    } else if(list != &TcbList) {
        ListNodeMove(list, &TcbList, node);
    }
    EventListDestroy(&tcb->event_list);
    ListNodeDeinit(&TcbList, node);
    CoreObjectFree((void**)&tcb, NULL);
}


static OsResult_t UtilTaskSubscribe(pTcb_t tcb, Id_t object_id, U32_t event, U8_t flags, U32_t timeout_ms, Id_t *out_event_id)
{
    OsResult_t result = OS_ERROR;
    if(tcb != NULL) {
        if(ListNodeLock(&tcb->list_node, LIST_LOCK_MODE_WRITE) == OS_OK) {
            result = EventSubscribe(&tcb->event_list, object_id, event, flags, ConvertMsToUs(timeout_ms), out_event_id);
            if((flags & EVENT_FLAG_PERMANENT)) {
                UtilTaskFlagSet(tcb, TASK_FLAG_WAIT_PERMANENT);
            } else {
                UtilTaskFlagSet(tcb, TASK_FLAG_WAIT_ONCE);
            }
            result = OS_OK;
            ListNodeUnlock(&tcb->list_node, LIST_LOCK_MODE_WRITE);
        }
    }
    return result;
}

/* Adds t_us to the task's runtime.  */
void UtilTaskRuntimeAdd(pTcb_t tcb, U32_t t_us)
{
    tcb->run_time_us += t_us;
}

void UtilTaskRuntimeReset(pTcb_t tcb)
{
    tcb->run_time_us = 0;
}
/*
 * TaskDef.h
 *
 * Created: 27-9-2017 23:05:26
 *  Author: Dorus
 */


#ifndef TASK_DEF_H_
#define TASK_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "include/OsTypes.h"
#include "include/Task.h"
#include "kernel/inc/Event.h"

#define IDLE_TASK_PRIO   0x0A //Idle Task priority

#define TASK_EVENT_ACTIVATE       (EVENT_TYPE_STATE_CHANGE | 0x00004000)

typedef enum {
    TASK_FLAG_DELETE         = 0x80,
    TASK_FLAG_INIT           = 0x40,
    TASK_FLAG_ESSENTIAL      = 0x20,
    TASK_FLAG_NO_PREEM       = 0x10,
} TaskFlags_t;

union DynamicPrio {
	/* Incremented every cycle that the task is not running.
	 * The task will age and be boosted in priority to avoid starvation. */
    U32_t  age_us;

    /* Real-Time deadline in microseconds. The closer the deadline,
     * the higher the priority of the task. */
    U32_t rt_deadline_us;
};

/* Scheduling parameters.
 * In separate structure because this
 * struct is only used when the task is
 * in a scheduling queue i.e. redundant
 * for a considerable amount of time. */
struct SchedAttributes {
    LinkedList_t *queue;	/* Scheduling queue the task is in. */
    Prio_t   prio_sum;		/* Sum of priorities. */
    Prio_t inherited_prio;	/* Possible inherited priority from another task. */
    union DynamicPrio dyn_prio_param; /* Dynamic priority parameter. */
};


/* Task Control Block definition */

typedef struct Tcb_t {

    ListNode_t       list_node;

#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM
    /* Task stack and stack pointer.
     * The stack is dynamically allocated upon creation. */
    U32_t               stack_size;
    MemBase_t           *stack;
    MemBase_t           *stack_pointer;
#endif


    Task_t              handler; /* Task handler entry point. */
    const void          *p_arg;
    U32_t               v_arg;

#if PRTOS_CONFIG_ENABLE_TASKNAMES==1
    char                name[PRTOS_CONFIG_TASK_NAME_LENGTH_CHARS];
#endif

    TaskFlags_t         flags;
    Prio_t              priority;
    TaskCat_t           category;
    TaskState_t         state;

    //struct SchedAttributes *scheduling_attributes;

    LinkedList_t        event_list;

    volatile U32_t      run_time_us;
    volatile U32_t      active_time_us;
    U32_t               deadline_time_us;

} Tcb_t;

typedef struct Tcb_t * pTcb_t;

/* Kernel Task API */

OsResult_t   KTaskInit(void);
LinkedList_t *KTcbWaitListRefGet(void);
LinkedList_t *KTcbListRefGet(void);
pTcb_t KTcbIdleGet(void);
OsResult_t KTcbMove(pTcb_t to_move, LinkedList_t *from_list, LinkedList_t* to_list);
void KTcbSwap(pTcb_t x, pTcb_t y, LinkedList_t* list);
OsResult_t KTcbDestroy(pTcb_t TCB, LinkedList_t* list);
LinkedList_t* KTcbLocationGet(pTcb_t tcb);
void KTaskStateSet(pTcb_t tcb_pointer, TaskState_t new_state);
pTcb_t KTcbFromId(Id_t task_id);
void KTcbRunningRefSet(pTcb_t tcb);

void KTaskFlagSet(pTcb_t tcb, TaskFlags_t flag);
void KTaskFlagClear(pTcb_t tcb ,TaskFlags_t flag);
U8_t KTaskFlagGet(pTcb_t tcb, TaskFlags_t flag);

Prio_t KTaskCalculateInvPriority(Prio_t P, TaskCat_t Mj);
Prio_t KTaskCalculatePriority(TaskCat_t Mj,Prio_t Mi);

void KTaskRunTimeUpdate(void);
void KTaskRunTimeReset(pTcb_t tcb);

/* Request to activate a task. Returns OS_RES_FAIL if the request was denied. */
OsResult_t KTaskActivateRequest(pTcb_t tcb);

#ifdef __cplusplus
}
#endif
#endif /* TASK_DEF_H_ */

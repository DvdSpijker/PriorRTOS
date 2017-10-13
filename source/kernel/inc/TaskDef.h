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

#include <Types.h>
#include <Event.h>
#include <Task.h>

#define IDLETASK_PRIO   0x0A //Idle Task priority

#define TASK_EVENT_ACTIVATE       (EVENT_TYPE_STATE_CHANGE | 0x00004000)

typedef enum {
    TASK_FLAG_DELETE         = 0x80,
    TASK_FLAG_WAIT_ONCE      = 0x40,
    TASK_FLAG_WAIT_PERMANENT = 0x20,
    TASK_FLAG_WAIT_BLOCKING  = 0x10,
    TASK_FLAG_INIT           = 0x08,
    TASK_FLAG_ESSENTIAL      = 0x04,
    TASK_FLAG_NO_PREEM       = 0x02,
} TaskFlags_t;

union DynamicPrio {
    U32_t  age_us;  /* Incremented every cycle that the task is not running.
                         * The task will age and be boosted in priority to avoid starvation. */
    U32_t deadline_us;
};

/* Scheduling parameters.
 * In separate structure because this
 * struct is only used when the task is
 * in a scheduling queue i.e. redundant
 * for a considerable amount of time. */
struct SchedParams {
    LinkedList_t *queue;    /* Scheduling queue the task is in. */
    U8_t   prio_sum;        /* All priorities added. */
    Prio_t boosted_prio;    /* Priority boosted on top of base priority. */
    Prio_t inherited_prio;  /* Possible inherited priority from other task. */
    union DynamicPrio dyn_prio;
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
    TaskFlags_t         flags;
    const void          *p_arg;
    U32_t               v_arg;

#if PRTOS_CONFIG_ENABLE_TASKNAMES>0
    char                generic_name[PRTOS_CONFIG_TASK_NAME_LENGTH];
#endif

    Prio_t              priority;
    TaskCat_t           category;
    TaskState_t         state;

    //struct SchedParams *scheduling_parameters;

    LinkedList_t        event_list;

    volatile U32_t      run_time_us;
    volatile U32_t      active_time_us;
    U32_t               deadline_time_us;

} Tcb_t;

typedef struct Tcb_t * pTcb_t;

LinkedList_t TcbList; /* Holds all tasks with states
                      other than dormant, scheduled or running. */

LinkedList_t TcbWaitList; /* Holds all dormant tasks i.e. waiting for an event. */

LinkedList_t ExecutionQueue; /* Holds all the scheduled and currently running tasks. */

pTcb_t TcbRunning;/* Holds a pointer to the TCB
                     of the currently running task. */

pTcb_t TcbIdle;/* Holds a pointer to the TCB
                  of the OS Idle task (see Kernel_task.c). */

/* Kernel Task API */

OsResult_t   TaskInit(void);
OsResult_t UtilTcbMove(pTcb_t to_move, LinkedList_t *from_list, LinkedList_t* to_list);
void UtilTcbSwap(pTcb_t x, pTcb_t y, LinkedList_t* list);
void UtilTcbDestroy(pTcb_t TCB, LinkedList_t* list);
LinkedList_t* UtilTcbLocationGet(pTcb_t tcb);
void UtilTaskStateSet(pTcb_t tcb_pointer, TaskState_t new_state);
pTcb_t UtilTcbFromId(Id_t task_id);
void UtilTaskFlagSet(pTcb_t tcb, TaskFlags_t flag);
void UtilTaskFlagClear(pTcb_t tcb ,TaskFlags_t flag);
U8_t UtilTaskFlagGet(pTcb_t tcb, TaskFlags_t flag);
Prio_t UtilCalculateInvPriority(Prio_t P, TaskCat_t Mj);
Prio_t UtilCalculatePriority(TaskCat_t Mj,Prio_t Mi);
void UtilTaskRuntimeAdd(pTcb_t tcb, U32_t t_us);
void UtilTaskRuntimeReset(pTcb_t tcb);


#ifdef __cplusplus
}
#endif
#endif /* TASK_DEF_H_ */
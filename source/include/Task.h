/******************************************************************************************************************************************
 *  File: Task.h
 *  Description: Task API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright© 2017    D. van de Spijker
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights to use,
 *  copy, modify, merge, publish, distribute and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed or sold, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**********************************************************************************************************************************************/


#ifndef TASK_H_
#define TASK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <OsTypes.h>
#include <EventDef.h>


/* Task_t definition see Types.h. */

/* Task States */
typedef enum {
    /*  The task was disabled by the kernel.
     *  This can be due to an access violation or stack overflow. */
    TASK_STATE_DISABLED     = 0xFF,

    /* The task has been created but is not activated or actively waiting. */
    TASK_STATE_IDLE         = 0x05,

    /* The task is actively waiting for an event to unblock. */
    TASK_STATE_WAITING      = 0x04,

    /* The task is active and will be executed. */
    TASK_STATE_ACTIVE       = 0x03,

    /* The task has been selected to execute. */
    TASK_STATE_RUNNING      = 0x02,

    /* A Real-Time task is about to miss its deadline and will have
     * absolute maximum priority to execute. */
    TASK_STATE_CRITICAL     = 0x01,

    /* Invalid task state. Should not be occupied by any task. */
    TASK_STATE_UNDEFINED    = 0x00,
} TaskState_t;

/* Standard task stack size. */
#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM
#define TASK_STD_STACK_SIZE (U32_t)PRTOS_CONFIG_STANDARD_STACK_SIZE_BYTES
#endif

/* Task Categories */
typedef enum {
    TASK_CAT_OS         = 0x04, /* Reserved for the kernel. */
    TASK_CAT_REALTIME   = 0x03, /* Scheduled according to a Shortest Deadline First policy. */
    TASK_CAT_HIGH       = 0x02, /* Scheduled according to Highest Priority First and Aging policy. */
    TASK_CAT_MEDIUM     = 0x01, /* Scheduled according to a Shortest Job First policy. */
    TASK_CAT_LOW        = 0x00  /* Scheduled according to a Shortest Job First policy. */
} TaskCat_t;

#define TASK_PARAMETER_NONE         0x00 /* This task has no parameters. */
#define TASK_PARAMETER_START        0x01 /* This task is promoted to the active state after creation. */
#define TASK_PARAMETER_ESSENTIAL    0x02 /* This task is essential, when it fails the system fails. System failure exception is raised. */
#define TASK_PARAMETER_TIMESLICED   0x04 /* This task has a configurable time slice that can be set using TaskTimeSliceSet. */
#define TASK_PARAMETER_NO_PREEM     0x08 /* Locks the scheduler by default when this task is executing. */


/* Task Event macros */
#ifdef PRTOS_CONFIG_USE_TASK_EVENT_CREATE_DELETE
#define TASK_EVENT_CREATE         EVENT_TYPE_CREATE
#define TASK_EVENT_DELETE         EVENT_TYPE_DELETE
#endif

#ifdef PRTOS_CONFIG_USE_TASK_EVENT_SUSPEND
#define TASK_EVENT_SUSPEND  (EVENT_TYPE_STATE_CHANGE | 0x00004000)
#endif

#ifdef PRTOS_CONFIG_USE_TASK_EVENT_EXECUTE_EXIT
#define TASK_EVENT_EXECUTE        (EVENT_TYPE_STATE_CHANGE | 0x00001000)
#define TASK_EVENT_EXIT           (EVENT_TYPE_STATE_CHANGE | 0x00002000)
#define TASK_EVENT_DISABLE        (EVENT_TYPE_STATE_CHANGE | 0x00003000)
#endif

/******************************************************************************
 * @func: Id_t TaskCreate(Task_t handler, TaskCat_t category,
 * Prio_t priority, const void *p_arg, U32_t v_arg)
 *
 * @desc: Creates a Task for the given handler. The
 * task is now able to be scheduled based on its category and priority.
 * The arguments are passed to task upon execution.
 *
 * @argin: (Task_t) handler; Task handler function.
 * @argin: (TaskCat_t) category; Task category: TASK_CAT_LOW, TASK_CAT_MEDIUM,
 * TASK_CAT_HIGH, TASK_CAT_REALTIME.
 * @argin: (Prio_t) priority; Task priority: 1-5 (5 is highest).
 * @argin: (U8_t) param; Task creation parameter, use TASK_PARAMETER_* macros.
 * E.g. (TASK_PARAMETER_ESSENTIAL | TASK_PARAMETER_NO_PREEM). Use TASK_PARAMETER_NONE
 * to pass no parameters.
 * @argin: (U32_t) stack_size; Task stack size in bytes. Pass TASK_STD_STACK_SIZE to
 * get the configured standard stack size.
 * @argin: (const void*) p_arg; Pointer task argument. Passed to the task
 * when executed.
 * @argin: (U32_t) v_arg; Value task argument. Passed to the task when executed.
 *
 * @rettype:  (Id_t); Task ID
 * @retval:   ID_INVALID; if an error occurred during task creation.
 * @retval:   Other; if successful.
 ******************************************************************************/
Id_t TaskCreate(Task_t handler, TaskCat_t category, Prio_t priority, U8_t param,
                U32_t stack_size, const void *p_arg, U32_t v_arg);


/******************************************************************************
 * @func: OsResult_t TaskDelete(Id_t *task_id)
 *
 * @desc: Deletes the Task matching the task_id.
 * The task cannot be scheduled anymore after calling this function.
 * Note: task_id will be set to ID_INVALID to avoid illegal use of the deleted task.
 *
 * @argin: (Id_t *) task_id; Task ID. NULL for current task.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if operation was successful.
 * @retval:   OS_RES_ERROR; if the task handler was not found in any of the lists.
 ******************************************************************************/
OsResult_t TaskDelete(Id_t *task_id);

/******************************************************************************
 * @func: Id_t TaskIdGet(void)
 *
 * @desc: Returns the ID of the calling task.
 *
 * @rettype:  (Id_t); Task ID
 * @retval:   OS_ID_INVALID; Error occurred.
 * @retval:   Other; valid task ID.
 ******************************************************************************/
Id_t TaskIdGet(void);

/******************************************************************************
 * @func: OsResult_t TaskRealTimeDeadlineSet(Id_t rt_task_id, U32_t t_ms)
 *
 * @desc: Sets the scheduling deadline of the Real-Time task.
 * This overrides the default deadline defined by
 * CONFIG_REAL_TIME_TASK_DEADLINE_MS_DEFAULT
 *
 * @argin: (Id_t) rt_task_id; Real-Time Task ID.
 * @argin: (U32_t) t_ms; Deadline in ms.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if operation was successful.
 * @retval:   OS_RES_ID_INVALID; if the given task was not a Real-Time task or if
 *            the task ID was an invalid ID (ID_INVALID).
 * @retval:   OS_RES_ERROR; if the task handler was not found in any of the lists.
 ******************************************************************************/
OsResult_t TaskRealTimeDeadlineSet(Id_t rt_task_id, U32_t t_ms);


/******************************************************************************
 * @func: OsResult_t TaskPrioritySet(Id_t task_id, Prio_t new_priority)
 *
 * @desc: Assigns a new priority level to the task.
 *
 * @argin: (Id_t) task_id; Task ID. ID_INVALID = Running task ID.
 * @argin: (Prio_t) priority; New task priority: 1-5 (5 is highest).
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if operation was successful.
 * @retval:   OS_RES_OUT_OF_BOUNDS; if the new priority was not within bounds (1-5).
 * @retval:   OS_RES_ERROR; if the task handler was not found in any of the lists.
 ******************************************************************************/
OsResult_t TaskPrioritySet(Id_t task_id, Prio_t new_priority);


/******************************************************************************
 * @func: Prio_t TaskPriorityGet(Id_t task_id)
 *
 * @desc: Returns the priority level of the task.
 *
 * @argin: (Id_t) task_id; Task ID. ID_INVALID = Running task ID.
 *
 * @rettype:  (Prio_t); task priority
 * @retval:   0; task could not be found.
 * @retval:   1-5; valid task priority.
 ******************************************************************************/
Prio_t TaskPriorityGet(Id_t task_id);


/******************************************************************************
 * @func: TaskState_t TaskStateGet(Id_t task_id)
 *
 * @desc: Returns the task's current state.
 *
 * @argin: (Id_t) task_id; Task ID. ID_INVALID = Running task ID.
 *
 * @rettype:  (TaskState_t); task state
 * @retval: Any; valid state.
 ******************************************************************************/
TaskState_t TaskStateGet(Id_t task_id);



/******************************************************************************
 * @func: U32_t TaskRunTimeGet(void)
 *
 * @desc: Returns the current task's runtime (since last task switch) in microseconds. 
 *
 * @rettype:  (U32_t); task runtime in us.
 * @retval:   0; if the task could not be found.
 * @retval:   Other; if valid.
 ******************************************************************************/
U32_t TaskRunTimeGet(void);


/******************************************************************************
 * @func: OsResult_t TaskResumeWithVarg(Id_t task_id, U32_t v_arg)
 *
 * @desc: Resumes the task and passes a value argument to it.
 * The value argument will be passed to the task upon execution.
 * NOTE 1:  This does NOT work when the task is disabled (TASK_STATE_DISABLED).
 * NOTE 2:  If task v_arg is unused, pass 0.
 * NOTE 3:  A task CANNOT resume itself.
 *
 * @argin: (Id_t) task_id; Task ID.
 * @argin: (U32_t) v_arg; Task Value argument.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the task was resumed.
 * @retval:   OS_RES_ERROR; if the task could not be found.
 ******************************************************************************/
OsResult_t TaskResumeWithVarg(Id_t task_id, U32_t v_arg);

/******************************************************************************
 * @func: OsResult_t TaskResume(Id_t task_id)
 *
 * @desc: Resumes the task.
 * NOTE 1:  This does NOT work when the task is disabled (TASK_STATE_DISABLED).
 * NOTE 2:  A task CANNOT resume itself
 *
 * @argin: (Id_t) task_id; Task ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the task was resumed.
 * @retval:   OS_RES_ERROR; if the task could not be found.
 ******************************************************************************/
OsResult_t TaskResume(Id_t task_id);


/* Pre-emptive only. Not ready to use. */
#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM

OsResult_t TaskSuspend(Id_t task_id);

void TaskSuspendSelf(void);

void TaskSuspendAll(void);

void TaskResumeAll(void);

#else

/******************************************************************************
 * @func: TaskSuspendSelf()
 *
 * @desc: Calling task will suspend execution.
 ******************************************************************************/
#define TaskSuspendSelf()   \
return;                     \

#endif

/**********************/


/******************************************************************************
 * @func: OsResult_t TaskSleep(U32_t t_ms)
 *
 * @desc: Calling task will sleep for the specified amount of
 * time after exiting. After the sleep-timer expires, the task is
 * automatically woken and executed.
 *
 * @argin: (U32_t) t_ms; Sleep time in milliseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the sleep timer was created.
 * @retval:   OS_RES_ERROR; if the task could not be found.
 ******************************************************************************/
OsResult_t TaskSleep(U32_t t_ms);

/******************************************************************************
 * @func: OsResult_t TaskPollAdd(Id_t object_id, U32_t event, U32_t timeout_ms)
 *
 * @desc: Add an event to the calling task's polling list. The event poll is auto-
 * matically re-added when the event occurs. To remove the event poll, call
 * TaskPollRemove.
 *
 * @argin: (Id_t) object_id; ID of the event emitting object.
 * @argin: (U32_t) event; Event to poll.
 * @argin: (U32_t) timeout_ms; Event timeout in milliseconds. If OS_TIMEOUT_INFINITE
 * is passed, the task will wait indefinitely.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the event is added to the polling list.
 * @retval:   OS_RES_ERROR; if an error occurred.
 ******************************************************************************/
OsResult_t TaskPollAdd(Id_t object_id, U32_t event, U32_t timeout_ms);

/******************************************************************************
 * @func: OsResult_t TaskPollRemove(Id_t object_id, U32_t event)
 *
 * @desc: Remove an event from the calling task's polling list.
 *
 * @argin: (Id_t) object_id; ID of the event emitting object.
 * @argin: (U32_t) event; Event to remove.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the event is removed from the polling list.
 * @retval:   OS_RES_ERROR; if an error occurred.
 ******************************************************************************/
OsResult_t TaskPollRemove(Id_t object_id, U32_t event);

/******************************************************************************
 * @func: OsResult_t TaskPoll(Id_t object_id, U32_t event, U32_t timeout_ms,
 * bool add_poll)
 *
 * @desc: The task will poll for the event emitted by the
 * specified object in a NON-BLOCKING fashion.
 * When this event occurs the task will be activated.
 * If the event does not occur within the specified time, the event
 * times out and the task will be activated to handle the timeout.
 *
 * @argin: (Id_t) object_id; ID of the event generating object. 
 * @argin: (U32_t) event; Event to listen to.
 * @argin: (U32_t) timeout_ms; Event timeout in milliseconds. If OS_TIMEOUT_INFINITE
 * is passed, the task will wait indefinitely.
 * @argin: (bool) add_poll; Add a new event poll is the event has occurred or is not yet
 * being polled.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_POLL; if the event is being polled.
 * @retval:   OS_RES_EVENT; if the polled event has occurred.
 * @retval:   OS_RES_TIMEOUT; if the timeout expired.
 * @retval:   OS_RES_FAIL; if the task is not polling the event (and add_poll=false).
 * @retval:   OS_RES_ERROR; if an error occurred.
 ******************************************************************************/
OsResult_t TaskPoll(Id_t object_id, U32_t event, U32_t timeout_ms, bool add_poll);

#ifdef PRTOS_CONFIG_USE_SCHEDULER_PREEM
/******************************************************************************
 * @func: OsResult_t TaskWait(Id_t object_id, U32_t event, U32_t timeout_ms)
 *
 * @desc: The task will wait for the specified event to be emitted by the
 * specified object in a BLOCKING fashion.
 * This function will return when the event has occurred, the timeout expires
 * or when an error occurred.
 *
 * @argin: (Id_t) object_id; ID of the event generating object. If ID_INVALID
 * the task will be listened to all.
 * @argin: (U32_t) event; Event to listen to.
 * @argin: (U32_t) timeout_ms; Event timeout in milliseconds. If OS_TIMEOUT_INFNITE
 * is passed, the task will wait indefinitely.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_EVENT; if the event has occurred.
 * @retval:   OS_RES_TIMEOUT; if the timeout expired.
 * @retval:   OS_RES_ERROR; if an error occurred.
 ******************************************************************************/
OsResult_t TaskWait(Id_t object_id, U32_t event, U32_t timeout_ms);
#endif

#ifdef PRTOS_CONFIG_USE_TASK_EVENT_EXECUTE_EXIT
/******************************************************************************
 * @func: OsResult_t TaskJoin(Id_t task_id, U32_t timeout)
 *
 * @desc: The calling task will wait/poll for the specified task to be deleted.
 * Calling task will only wait/poll until the timeout.
 *
 * @argin: (Id_t) task_id; Task ID to join with.
 * @argin: (U32_t) timeout; Amount of time to wait/poll for the join event.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_EVENT; if the event has occurred.
 * @retval:   OS_RES_TIMEOUT; if the timeout expired.
 * @retval:   OS_RES_ERROR; if the task could not be found.
 ******************************************************************************/
OsResult_t TaskJoin(Id_t task_id, U32_t timeout);
#endif


/******************************************************************************
 * @func: TASK_INIT_BEGIN()
 *
 * @desc: Initializes the variables required by the event handlers. All
 * user code between _BEGIN and _END will only be executed once.
 ******************************************************************************/
#define TASK_INIT_BEGIN()               \
static U8_t init_done = 0;              \
if(init_done == 0) {                    \



/******************************************************************************
 * @func: TASK_INIT_END()
 *
 * @desc: Indicates the end of the TASK_INIT block. Must be used in
 * combination with TASK_INIT_BEGIN().
 ******************************************************************************/
#define TASK_INIT_END()                \
    init_done = 1;                     \
}                                      \


#if PRTOS_CONFIG_ENABLE_TASKNAMES>0
void TaskGenericNameSet(Id_t task_id, const char* gen_name);
#endif


#ifdef __cplusplus
}
#endif
#endif /* TASK_H_ */

#ifndef OS_H_
#define OS_H_

#include "PriorRTOSConfig.h"
#include "OsTypes.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PRTOS_CONFIG_USE_EXTERNAL_OS_HEAP
/******************************************************************************
 * @func: void OsHeapSet(U8_t *os_heap)
 *
 * @desc: Sets the memory used for the OS Heap. Must be called before OsInit.
 * The memory must of size PRTOS_CONFIG_OS_HEAP_SIZE_BYTES.
 ******************************************************************************/
void OsHeapSet(U8_t *os_heap);
#endif

/******************************************************************************
 * @func: OsResult_t OsInit(OsResult_t *result_optional)
 *
 * @desc: Initializes the Prior RTOS kernel. This function must be called
 * before any other Prior API function, with the exception of OsHeapSet.
 *
 * @argout: (OsResult_t *) result_optional; Result of optional module initialization.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; all essential modules were initialized successfully.
 * @retval:   OS_RES_ERROR; if one of the essential modules was not initiated successfully.
 * It is recommended to call OsReset or hard-reset the target.
 ******************************************************************************/
OsResult_t OsInit(OsResult_t *result_optional);

/******************************************************************************
 * @func: void OsStart (Id_t start_task_id)
 *
 * @desc: Starts the OS tick and scheduler. The first task to be executed
 * can be specified by start_task_id. When an error occurs while loading the
 * specified start task, the Idle task is loaded instead.
 * Note: This function only returns when OsStop is called or when a critical
 * error occurred.
 *
 * @argin: (Id_t) start_task_id; ID of the first task to be executed.
 ******************************************************************************/
void OsStart(Id_t start_task_id);


/******************************************************************************
 * @func: void OsStop(void)
 *
 * @desc: Stops the OS tick and scheduler. The currently executing task
 * will either finish execution (in cooperative mode) or be suspended
 * (in pre-emptive mode). All tasks and other objects will be deleted. 
 ******************************************************************************/
void OsStop(void);

/******************************************************************************
 * @func: U16_t OsFrequencyGet(void)
 *
 * @desc: Returns the current OS frequency in Hz.
 *
 * @rettype:  (U16_t); Current OS frequency
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid frequencies.
 ******************************************************************************/
U16_t OsFrequencyGet(void);


/******************************************************************************
 * @func: OsVer_t OsVersionGet(void)
 *
 * @desc: Returns the OS version e.g. 0x0101 = V1.01. OsVer_t may be
 * converted to a string using ConvertOsVersionToString.
 *
 * @rettype:  (OsVer_t); Current OS version
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid versions.
 ******************************************************************************/
OsVer_t OsVersionGet(void);


/******************************************************************************
 * @func: OsResult_t OsRunTimeGet(OsRunTime_t runtime)
 *
 * @desc: Copies the current OS runtime to the runtime array.
 * 
 * @argin: (OsRunTime_t) runtime; Array initialized with OS_RUN_TIME_INIT.
 * @argout: (OsRunTime_t) runtime; runtime[0] = hours, runtime[1] = microseconds.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the operation was successful.
 * @retval:   OS_RES_ERROR; if the array did NOT comply with the requirements stated
 * in the description.
 ******************************************************************************/
OsResult_t OsRunTimeGet(OsRunTime_t runtime);

/******************************************************************************
 * @func: U32_t OsRunTimeMicrosDelta(U32_t us)
 *
 * @desc: Returns the difference between us and the current micros.
 *
 * @argin:   (U32_t) us; Earlier moment in time in microseconds.
 *
 * @rettype:  (U32_t); Delta micros
 * @retval:   0; if the calculation could not be performed.
 * @retval:   Other; valid delta value.
 ******************************************************************************/
U32_t OsRunTimeMicrosDelta(U32_t us);

/******************************************************************************
 * @func: U32_t OsRunTimeMicrosGet(void)
 *
 * @desc: Returns the 'microseconds' component of the OS runtime.
 *
 * @rettype:  (U32_t) OS runtime microseconds.
 ******************************************************************************/
U32_t OsRunTimeMicrosGet(void);

/******************************************************************************
 * @func: U32_t OsRunTimeHoursGet(void)
 *
 * @desc: Returns the 'hours' component of the OS runtime.
 *
 * @rettype:  (U32_t); OS runtime hours.
 * @retval:   Any; valid hours.
 ******************************************************************************/
U32_t OsRuntimeHoursGet(void);


/******************************************************************************
 * @func: U32_t OsTickPeriodGet(void)
 *
 * @desc: Returns the OS tick period in microseconds.
 * Microseconds can be converted to milliseconds using ConvertUsToMs.
 *
 * @rettype:  (U32_t); OS tick period in us
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid tick periods.
 ******************************************************************************/
U32_t OsTickPeriodGet(void);


/******************************************************************************
 * @func: U32_t OsTasksTotalGet(void)
 *
 * @desc: Returns the total number of tasks currently present in the
 * system.
 *
 * @rettype:  (U32_t); Total number of tasks.
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid number of tasks.
 ******************************************************************************/
U32_t OsTasksTotalGet(void);

/******************************************************************************
 * @func: U32_t OsTasksActiveGet(void)
 *
 * @desc: Returns the number of active tasks currently present in the
 * system. A task is considered active if it occupies one of the following
 * states: TASK_STATE_RUNNING, TASK_STATE_ACTIVE or TASK_STATE_CRITICAL.
 *
 * @rettype:  (U32_t); Number of active tasks.
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid number of tasks.
 ******************************************************************************/
U32_t OsTasksActiveGet(void);


/******************************************************************************
 * @func: U32_t OsEventsTotalGet(void)
 *
 * @desc: Returns the number of emitted events in the system at that
 * moment.
 *
 * @rettype:  (U32_t); Total number of events.
 * @retval:   0; if an error occurred.
 * @retval:   Other; for valid number of events.
 ******************************************************************************/
U32_t OsEventsTotalGet(void);

/******************************************************************************
 * @func: bool OsTaskExists(Id_t task_id)
 *
 * @desc: Validates if the passed ID belongs to an existing task.
 *
 * @rettype:  (bool); Validation result.
 * @retval:   false; if the ID does not belong to an existing task.
 * @retval:   true; if the ID does belong to an existing task.
 ******************************************************************************/
bool OsTaskExists(Id_t task_id);


/******************************************************************************
 * @func: Id_t OsCurrentTaskGet(void)
 *
 * @desc: Returns the ID of the current running task.
 *
 * @rettype:  (Id_t); Task ID
 * @retval:   OS_ID_INVALID; error occurred.
 * @retval:   Other; valid current task ID.
 ******************************************************************************/
Id_t OsCurrentTaskGet(void);


/******************************************************************************
 * @func: void OsCritSectBegin(void)
 *
 * @desc: Locks the scheduler and disables interrupts. This function
 * should ONLY be used in critical sections that require precise timing.
 * This function may be called recursively throughout other functions, as long
 * as every OsCritSectBegin call is paired with a OsCritSectEnd call within
 * the same function scope.
 ******************************************************************************/
void OsCritSectBegin(void);



/******************************************************************************
 * @func: void OsCritSectEnd(void)
 *
 * @desc: Unlocks the scheduler and enables interrupts. This function should
 * be called at the end of the critical code section.
 * This function may be called recursively throughout other functions, as long
 * as every OsCritSectBegin call is paired with a OsCritSectEnd call within
 * the same function scope.
 ******************************************************************************/
void OsCritSectEnd(void);



/******************************************************************************
 * @func: void OsIsrBegin(void)
 *
 * @desc: Informs the kernel that a user ISR is currently executing.
 * This prevents the kernel from switching tasks during the executing of an
 * interrupt. Note that the OS tick interrupt will keep occurring (if its
 * priority is higher).
 * The kernel will only switch tasks when all interrupts have finished execution.
 * Note that a OsIsrBegin call HAS to be paired with a OsIsrEnd call within
 * the same ISR scope.
 ******************************************************************************/
void OsIsrBegin(void);



/******************************************************************************
 * @func: void OsIsrEnd(void)
 *
 * @desc: Informs the kernel that a user ISR has finished executing and is
 * allowed to switch tasks.
 * The kernel will only switch tasks when all interrupts have finished execution.
 * Note that a OsIsrBegin call HAS to be paired with a OsIsrEnd call within
 * the same ISR scope.
 ******************************************************************************/
void OsIsrEnd(void);


/******************************************************************************
 * @func: S8_t OsIsrNestCountGet(void)
 *
 * @desc: Returns the number of Interrupt Service Routines that
 * are nested and have called OsIsrBegin.
 *
 * @rettype:  (S8_t); Nest count
 * @retval:   -1; Cannot access the nest counter at this moment.
 * @retval:   0; No nesting.
 * @retval:   Other; Nesting level.
 ******************************************************************************/
S8_t OsIsrNestCountGet(void);



/******************************************************************************
 * @func: void OsSchedulerLock(void)
 *
 * @desc: Locks the scheduler preventing the kernel from scheduling NEW
 * tasks for execution. Already scheduled tasks will still execute. When the
 * execution queue is empty the Idle task will be executed.
 * This function may be called recursively throughout other functions, as long
 * as every OsSchedulerLock call is paired with a OsSchedulerUnlock call within
 * the same function scope.
 ******************************************************************************/
void OsSchedulerLock(void);


/******************************************************************************
 * @func: void OsSchedulerUnlock(void)
 *
 * @desc: Unlocks the scheduler allowing it to scheduler new tasks if
 * the schedule lock counter is equal to zero i.e. all nested locks have executed
 * their respective unlocks.
 * This function may be called recursively throughout other functions, as long
 * as every OsSchedulerLock call is paired with a OsSchedulerUnlock call within
 * the same function scope.
 ******************************************************************************/
void OsSchedulerUnlock(void);


/******************************************************************************
 * @func: bool OsSchedulerIsLocked(void)
 *
 * @desc: Returns the scheduler lock state.
 *
 * @rettype:  (bool); Scheduler lock state
 * @retval:   false; Not locked.
 * @retval:   true; Locked.
 ******************************************************************************/
bool OsSchedulerIsLocked(void);

#ifdef __cplusplus
}
#endif


#endif
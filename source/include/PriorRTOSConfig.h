#ifndef PRIOR_RTOS_CONFIG_H_
#define PRIOR_RTOS_CONFIG_H_

/*******************************************************************************************************************
 *  File: PriorRTOSConfig.h
 *  Description: Prior Configuration header

 *  OS Version: V0.4.1
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
 *  3. The Software shall not be distributed , under a different name or otherwise.
 *
 *  4. This Software is free to use and distribute for non-commercial purposes.
 *
 *  5.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
***********************************************************************************************************************/

#include "StdTypes.h"

/*******************************************************
 * Information on Prior RTOS configuration macros.
 *
 * - All configuration macros have the PRTOS_CONFIG_ prefix in their name.
 * 
 * - Settings requiring a specific value (e.g. frequency or heap size) are
 * suffixed with their respective unit (e.g. PRTOS_CONFIG_F_CPU_HZ or
 * PRTOS_CONFIG_OS_HEAP_SIZE_BYTES).
 * 
 * - PRTOS_CONFIG_ENABLE_ settings expect a 1 to enable, and a 0 to disable.
 * 
 * - PRTOS_CONFIG_USE_ settings must be defined if used, and undefined/commented
 * out if not used. 
 * 
 *******************************************************/

/************ General Settings ************/

/*******************************************************
 * @macro: PRTOS_CONFIG_F_CPU_HZ
 *
 * @desc: Target CPU frequency in Hertz, e.g. 48000000 =
 * 48MHz.
 *
 * @dtype:    U32_t
 * @unit:     Hertz.
 *******************************************************/
#define PRTOS_CONFIG_F_CPU_HZ					72000000


/*******************************************************
 * @macro: PRTOS_CONFIG_F_OS_TIMER_HZ
 *
 * @desc: Non-prescaled Clock frequency of the hardware
 * timer used to generate the OS tick interrupt.
 *
 * @dtype:    U32_t
 * @unit:     Hertz.
 *******************************************************/
#define PRTOS_CONFIG_F_OS_TIMER_HZ              72000000


/*******************************************************
 * @macro: PRTOS_CONFIG_F_OS_HZ
 *
 * @desc: The frequency of the OS tick interrupt in Hertz.
 * A higher frequency results in a more responsive system,
 * but also increases CPU load.
 *
 * @dtype:    U16_t
 * @unit:     Hertz.
 *******************************************************/
#define PRTOS_CONFIG_F_OS_HZ						1000


/*******************************************************
 * @macro: PRTOS_CONFIG_IRQ_PRIORITY_TYPE
 *
 * @desc: Defines the Interrupt Request priority type.
 *******************************************************/
#define PRTOS_CONFIG_IRQ_PRIORITY_TYPE				U32_t

/*******************************************************
 * @macro: PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY
 *
 * @desc: OS tick interrupt priority.
 *
 * @dtype:    IrqPriority_t
 *******************************************************/
#define PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY 				0


/*******************************************************
 * @macro: PRTOS_CONFIG_USE_SCHEDULER_COOP
 *
 * @desc: The kernel will use the cooperative scheduler.
 * Tasks must yield voluntarily (by calling TaskSuspendSelf)
 * in order for the scheduler to switch tasks.
 *******************************************************/
#define PRTOS_CONFIG_USE_SCHEDULER_COOP

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_SCHEDULER_PREEM
 *
 * @desc: The kernel will use the pre-emptive scheduler.
 * A task can be switched out by the scheduler when
 * needed e.g. a higher priority task is ready. A task
 * can also call TaskSuspendSelf to suspend execution
 * or call TaskSuspend to suspend another task.
 *******************************************************/
/* !!!NOT YET AVAILABLE IN V 0.4.X!!! */
/* #define PRTOS_CONFIG_USE_SCHEDULER_PREEM */

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK
 *
 * @desc: All system calls will be implicitly non-blocking
 * even when using the pre-emptive scheduler, meaning
 * that they will poll for events instead of waiting.
 *******************************************************/
#define PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK


/*******************************************************
 * @macro: PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS 
 * 
 * @desc: Defines the amount of ticks every emitted event
 * lasts. A higher number of ticks enables tasks to respond
 * to events that occurred further in the past.
 *
 * @dtype: U8_t
 * @unit: ticks
 *******************************************************/
#define PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS				3


/***** List settings. *****/

/*******************************************************
 * @macro: PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS 
 * 
 * @desc: Enable the verification of all Lists existing
 * within the kernel. After every access the list's
 * navigation is checked for any corruption.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION	1

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_SORTED_LISTS 
 * 
 * @desc: The List API will use sorted lists where
 * possible. Resulting in more efficient add/search
 * operations, however code size is also increased.
 *******************************************************/
#define PRTOS_CONFIG_USE_SORTED_LISTS



/************ Memory Settings ************/

/*******************************************************
 * @macro: PRTOS_CONFIG_MEM_WIDTH_8_BITS
 *
 * @desc: Target memory is 8 bits wide.
 *
 * @unit: bits
 *******************************************************/
/*#define PRTOS_CONFIG_MEM_WIDTH_8_BITS */

/*******************************************************
 * @macro: PRTOS_CONFIG_MEM_WIDTH_16_BITS
 *
 * @desc: Target memory is 16 bits wide.
 *
 * @unit: bits
 *******************************************************/
/*#define PRTOS_CONFIG_MEM_WIDTH_16_BITS */

/*******************************************************
 * @macro: PRTOS_CONFIG_MEM_WIDTH_32_BITS
 *
 * @desc: Target memory is 32 bits wide.
 *
 * @unit: bits
 *******************************************************/
#define PRTOS_CONFIG_MEM_WIDTH_32_BITS

/*******************************************************
 * @macro: PRTOS_CONFIG_OS_HEAP_SIZE_BYTES
 *
 * @desc: Size (in bytes) of the statically allocated
 * OS Heap. The OS Heap is split into a part used by the
 * kernel, and a part that can be used by the user. The size
 * of the user heap is defined by
 * @ref(PRTOS_CONFIG_USER_HEAP_SIZE_BYTES).
 * Kernel heap size = OS heap size - user heap size.
 *
 * @dtype: U32_t
 * @unit: bytes
 *******************************************************/
#define PRTOS_CONFIG_OS_HEAP_SIZE_BYTES				0x4400 /* 17kB */

/*******************************************************
 * @macro:  PRTOS_CONFIG_USER_HEAP_SIZE_BYTES
 *
 * @desc: Size (in bytes) of the User part
 * of the OS Heap.
 *
 * @dtype: U32_t
 * @unit: bytes
 *******************************************************/
#define PRTOS_CONFIG_USER_HEAP_SIZE_BYTES			0x800 /* 2kB */

/*******************************************************
 * @macro: PRTOS_CONFIG_N_USER_POOLS
 *
 * @desc: Number of pools available to the user to allocate
 * blocks of memory. Pools are statically allocated.
 *******************************************************/
#define PRTOS_CONFIG_N_USER_POOLS						1


/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION
 *
 * @desc: Enables protection of each pool by means of
 * padding and a checksum.
 *******************************************************/
 /* !!!NOT YET AVAILABLE IN V 0.4.X!!! */
#define PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION			0


/************ Module Settings ************/
/* On PRTOS_CONFIG_USE_<MODULE>_EVENT_* directives:
 * Defined events will be generated by
 * their respective objects and are
 * available for subscription by tasks.
 * The available events per modules can be
 * found in their respective header files
 * in the include folder. */


/* Task Module settings */

/*******************************************************
 * @macro: PRTOS_CONFIG_STANDARD_STACK_SIZE_BYTES
 *
 * @desc: Defines the standard stack size in bytes.
 *
 * @dtype: U32_t
 * @unit: Bytes
 *******************************************************/
#define PRTOS_CONFIG_STANDARD_STACK_SIZE_BYTES			0

/*******************************************************
 * @macro: PPRTOS_CONFIG_REAL_TIME_TASK_DEADLINE_DEFAULT_MS
 *
 * @desc: The Real-Time Task Deadline
 * indicates the amount of time a real-time task is allowed
 * to be delayed before starting execution.
 * This value is used a default, the deadline can be changed
 * using TaskDeadlineSet.
 *
 * @dtype: U32_t
 * @unit: Milliseconds
 *******************************************************/
#define PRTOS_CONFIG_REAL_TIME_TASK_DEADLINE_DEFAULT_MS 15

//#define PRTOS_CONFIG_USE_TASK_EVENT_EXECUTE_EXIT
//#define PRTOS_CONFIG_USE_TASK_EVENT_CREATE_DELETE
#define PRTOS_CONFIG_USE_TASK_EVENT_SUSPEND


/* Timer Module Settings. */

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS
 *
 * @desc: Enable the software Timer module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS				1

/*******************************************************
 * @macro: PRTOS_CONFIG_TIMER_INTERVAL_RESOLUTION_MS
 *
 * @desc: Defines the number of OS ticks that occur before
 * all software timers are updated. A higher prescaler results
 * in software timers with a lower resolution e.g.
* Prescaler 5 => resolution: 5 * OS tick period. On the
* other hand it also reduces CPU load.
*
* @dtype: U32_t
* @unit: Milliseconds
 *******************************************************/
#define PRTOS_CONFIG_TIMER_INTERVAL_RESOLUTION_MS		50

#define PRTOS_CONFIG_USE_TIMER_EVENT_OVERFLOW
#define PRTOS_CONFIG_USE_TIMER_EVENT_START_STOP_RESET


/* Mailbox Module Settings. */
/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_MAILBOXES
 *
 * @desc: Enable the Mailbox module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_MAILBOXES					1

#define PRTOS_CONFIG_USE_MAILBOX_EVENT_POST_PEND


/* Semaphore Module Settings. */
/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_SEMAPHORES
 *
 * @desc: Enable the Semaphore module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_SEMAPHORES					0

#define PRTOS_CONFIG_USE_SEM_EVENT_ACQUIRE_RELEASE


/* Eventgroup Module Settings. */
/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_EVENTGROUPS
 *
 * @desc: Enable the Eventgroup module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_EVENTGROUPS                 1

#define PRTOS_CONFIG_USE_EVENTGROUP_EVENT_FLAG_SET
#define PRTOS_CONFIG_USE_EVENTGROUP_EVENT_FLAG_CLEAR


/* Ring-buffer Module Settings. */
/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_RINGBUFFERS
 *
 * @desc: Enable the Ringbuffer module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_RINGBUFFERS					1

#define PRTOS_CONFIG_USE_RINGBUFFER_EVENT_DATA_IN_OUT
#define PRTOS_CONFIG_USE_RINGBUFFER_EVENT_EMPTY_FULL
#define PRTOS_CONFIG_USE_RINGBUFFER_EVENT_PURGE




/************ Utility Settings ************/

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_CPULOAD_CALC
 *
 * @desc: Enable CPU load calculation for each task.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_CPULOAD_CALC				0

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_TASKNAMES
 *
 * @desc: Use task names. A task can be assigned a
 * human-readable name with TaskGenericNameSet.
 *******************************************************/
/* #define PRTOS_CONFIG_USE_TASKNAMES */

/*******************************************************
 * @macro: PRTOS_CONFIG_TASK_NAME_LENGTH_CHARS
 *
 * @desc: Defines the maximum length of a task name in chars.
 *
 * @dtype: U8_t
 * @unit: characters
 *******************************************************/
#define PRTOS_CONFIG_TASK_NAME_LENGTH_CHARS				20

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_CONVERT_LIB_IN_APP
 *
 * @desc: Use the Convert library in the application.
 *******************************************************/
#define PRTOS_CONFIG_USE_CONVERT_LIB_IN_APP

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_WATCHDOG
 *
 * @desc: Enable the Watchdog timer.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_WATCHDOG					0


/************ Logging and Debugging Settings ************/

/*******************************************************
 * @macro: PRTOS_CONFIG_DEBUG_SERIAL_BAUD_RATE_BPS
 *
 * @desc: Defines the baud rate of the Debug serial port in bits per second.
 *
 * @dtype: U32_t
 * @unit: Bits per second (bps)
 *******************************************************/
#define PRTOS_CONFIG_DEBUG_SERIAL_BAUD_RATE_BPS		115200

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_LOGGING
 *
 * @desc: Enable the Logger module.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LOGGING						1

/*******************************************************
 * @macro: PRTOS_CONFIG_USE_NEWLIB
 *
 * @desc: Use C newlib for standard IO.
 *******************************************************/
#define PRTOS_CONFIG_USE_NEWLIB

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_LOG_EVENT
 *
 * @desc: Enable Event logging.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LOG_EVENT					0

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_LOG_INFO
 *
 * @desc: Enable Info logging.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LOG_INFO					1

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_LOG_ERROR
 *
 * @desc: Enable Error logging.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LOG_ERROR					1

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_LOG_DEBUG
 *
 * @desc: Enable Debug logging.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_LOG_DEBUG					1

/* !!!NOT YET AVAILABLE IN V 0.4.1!!! */
/* #define PRTOS_CONFIG_USE_LOGGER_MODE_FILE */

#define PRTOS_CONFIG_USE_LOGGER_MODE_SERIAL

/* !!!NOT YET AVAILABLE IN V 0.4.1!!! */
//#define PRTOS_CONFIG_USE_LOGGER_MODE_RINGBUFFER

/*******************************************************
 * @macro: PRTOS_CONFIG_ENABLE_SHELL
 *
 * @desc: Enable the Shell module. The shell provides
 * a Command Line Interface.
 *******************************************************/
#define PRTOS_CONFIG_ENABLE_SHELL						0

#endif

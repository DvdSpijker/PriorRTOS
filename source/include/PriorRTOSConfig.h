#ifndef PRIOR_RTOS_CONFIG_H_
#define PRIOR_RTOS_CONFIG_H_

/******************************************************************************************************************************************
 *  File: PriorRTOSConfig.h
 *  Description: Prior Configuration header

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
**********************************************************************************************************************************************/


/************ General Settings ************/

/******************************************************************************
 * @macro: PRTOS_CONFIG_F_CPU_HZ
 *
 * @desc: Target CPU frequency in Hertz, e.g. 48000000 = 48MHz.
 *
 * @dtype:    (U32_t)
 * @unit:     Hertz.
 ******************************************************************************/
#define PRTOS_CONFIG_F_CPU_HZ                                     (U32_t)48000000


/******************************************************************************
 * @macro: PRTOS_CONFIG_F_OS_TIMER_HZ
 *
 * @desc: Non-prescaled Clock frequency of the hardware timer used to generate the OS tick
 * interrupt.
 *
 * @dtype:    (U32_t)
 * @unit:     Hertz.
 ******************************************************************************/
#define PRTOS_CONFIG_F_OS_TIMER_HZ                               PRTOS_CONFIG_F_CPU_HZ


/******************************************************************************
 * @macro: PRTOS_CONFIG_F_OS_TIMER_HZ
 *
 * @desc: The frequency of the OS tick interrupt in Hertz.
 * A higher frequency results in a more responsive system,
 * but also increases CPU load.
 *
 * @dtype:    (U16_t)
 * @unit:     Hertz.
 ******************************************************************************/
#define PRTOS_CONFIG_F_OS_HZ                                      (U16_t)2000


/******************************************************************************
 * @macro: PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY
 *
 * @desc: OS tick interrupt priority.
 *
 * @dtype:    (IrqPriority_t)
 ******************************************************************************/
#define PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY                         (IrqPriority_t)7


/******************************************************************************
 * @macro: PRTOS_CONFIG_USE_SCHEDULER_COOP
 *
 * @desc: The OS will use a cooperative scheduler. Tasks have to yield volentarily
 * (by calling TASK_YIELD()) in order for the scheduler to switch tasks.
 *
 ******************************************************************************/
#define PRTOS_CONFIG_USE_SCHEDULER_COOP

/******************************************************************************
 * @macro: PRTOS_CONFIG_USE_SCHEDULER_PREEM
 *
 * @desc: The OS will use a cooperative scheduler. Tasks have to yield volentarily
 * (by calling TASK_YIELD()) in order for the scheduler to switch tasks.
 *
 ******************************************************************************/
/* #define PRTOS_CONFIG_USE_SCHEDULER_PREEM */ /* !!!NOT YET AVAILABLE IN V 0.4.X!!! */

/******************************************************************************
 * @macro: PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK
 *
 * @desc: All system calls will be non-blocking even when using the pre-emptive
 * scheduler, meaning that they will poll for events instead of waiting.
 *
 ******************************************************************************/
#define PRTOS_CONFIG_USE_SYS_CALL_NO_BLOCK


/******************************
 * Setting: PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS
 * @desc: Defines the amount of ticks
 * every emitted event lasts.
 * A higher number of ticks enables tasks to respond
 * to events that occurred in the past.
 *
 * @data type: (U8_t) Unsigned 8 bit integer
 ******************************/
#define PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS                        (U8_t)2


/***** List settings. *****/

/* Enable the verification of all Lists existing within the
 * kernel. */
#define PRTOS_CONFIG_ENABLE_LIST_INTEGRITY_VERIFICATION           1

/* The List API will use sorted lists where possible.
 * Resulting in more efficient add/search operations,
 * however code size is also increased. */
#define PRTOS_CONFIG_USE_SORTED_LISTS

/*************************/

/******************************************/


/************ Memory Settings ************/

/* Memory width. */
/*#define PRTOS_CONFIG_MEM_WIDTH_8_BITS */
/*#define PRTOS_CONFIG_MEM_WIDTH_16_BITS */
#define PRTOS_CONFIG_MEM_WIDTH_32_BITS

/* Size (in bytes) of the statically allocated OS Heap. The OS Heap is split
 * into a part used by the kernel, and a part that can be used by the user.
 * The size of the user heap can be configured; PRTOS_CONFIG_USER_HEAP_SIZE. */
#define PRTOS_CONFIG_OS_HEAP_SIZE_BYTES                           2304

/* Size (in bytes) of the User accessible part of the OS Heap. */
#define PRTOS_CONFIG_USER_HEAP_SIZE_BYTES                         0

/* Number of pools available to the user to allocate blocks of memory.
 * Pools are statically allocated. */
#define PRTOS_CONFIG_N_USER_POOLS                                 0

/* Enables protection of each pool by means of padding and a checksum. */
#define PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION                     0



/******************************************/


/************ Module Settings ************/
/* On PRTOS_CONFIG_USE_EVENT directives:
 * Defined events will be generated by
 * their respective objects and are
 * available for subscription by tasks.
 * The available events per modules can be
 * found in their respective header files
 * in the include folder. */


/* Task Module settings */

/* Size of each task's stack in bytes. Only required when using the Pre-Emptive scheduler. */
#define PRTOS_CONFIG_STANDARD_STACK_SIZE_BYTES                   0

/******************************
 * Setting: PRTOS_CONFIG_REAL_TIME_TASK_DEADLINE_DEFAULT_MS
 * @desc: The Real-Time Task Deadline
 * indicates the amount of time a real-time task is allowed
 * to be delayed before starting execution.
 * This value is used a default, the deadline can be changed
 * using TaskDeadlineSet.
 *
 * Unit: ms
 ******************************/
#define PRTOS_CONFIG_REAL_TIME_TASK_DEADLINE_DEFAULT_MS           (U8_t)15

//#define PRTOS_CONFIG_USE_EVENT_TASK_EXECUTE_EXIT
//#define PRTOS_CONFIG_USE_EVENT_TASK_CREATE_DELETE
#define PRTOS_CONFIG_USE_EVENT_TASK_YIELD_SUSPEND
/*************************/

/* Timer Module Settings. */
#define PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS                       1

/* Defines the number of OS ticks that occur before all software timers are updated.
* A higher prescaler results in software timers with a lower resolution e.g.
* Prescaler 5 => resolution: 5 * OS tick period. On the other hand it also reduces
* CPU load. Prescaler 2 results in max resolution and CPU load. */
#define PRTOS_CONFIG_TIMER_INTERVAL_RESOLUTION_MS                 (U8_t)10

#define PRTOS_CONFIG_USE_EVENT_TIMER_OVERFLOW
#define PRTOS_CONFIG_USE_EVENT_TIMER_START_STOP_RESET
/*************************/

/* Mailbox Module Settings. */
#define PRTOS_CONFIG_ENABLE_MAILBOXES                             0
#define PRTOS_CONFIG_USE_EVENT_MAILBOX_POST_PEND
/*************************/

/* Semaphore Module Settings. */
#define PRTOS_CONFIG_ENABLE_SEMAPHORES                            0
#define PRTOS_CONFIG_USE_EVENT_SEM_ACQUIRE_RELEASE
/*************************/

/* Eventgroup Module Settings. */
#define PRTOS_CONFIG_ENABLE_EVENTGROUPS                           0
#define PRTOS_CONFIG_USE_EVENT_EVENTGROUP_FLAG_SET
#define PRTOS_CONFIG_USE_EVENT_EVENTGROUP_FLAG_CLEAR
/*************************/

/* Ring-buffer Module Settings. */
#define PRTOS_CONFIG_ENABLE_RINGBUFFERS                           1
#define PRTOS_CONFIG_USE_EVENT_RINGBUF_DATA_IN_OUT
#define PRTOS_CONFIG_USE_EVENT_RINGBUF_EMPTY_FULL
#define PRTOS_CONFIG_USE_EVENT_RINGBUF_FLUSH
/*************************/


/* Pipe Module Settings. */
/* !!!NOT AVAILABLE IN V 0.4.1!!! */
#define PRTOS_CONFIG_ENABLE_PIPES                                 0
/*************************/

/* Signal Module Settings. */
/* !!!NOT AVAILABLE IN V 0.4.1!!! */
#define PRTOS_CONFIG_ENABLE_SIGNALS                               0
/*************************/

/******************************************/
#define PRTOS_CONFIG_ENABLE_WATCHDOG							  0

/************ Utility Settings ************/

#define PRTOS_CONFIG_ENABLE_CPULOAD_CALC                          0
#define PRTOS_CONFIG_ENABLE_TASKNAMES                             0
#define PRTOS_CONFIG_TASK_NAME_LENGTH                             20

#define PRTOS_CONFIG_USE_CONVERT_LIB_IN_APP

/******************************************/

/************ Logging and Debugging Settings ************/
#define PRTOS_CONFIG_ENABLE_LOGGING								  1
#define PRTOS_CONFIG_USE_NEWLIB                                   1
#define PRTOS_CONFIG_ENABLE_LOG_EVENT                             1
#define PRTOS_CONFIG_ENABLE_LOG_INFO                              0
#define PRTOS_CONFIG_ENABLE_LOG_ERROR                             0
#define PRTOS_CONFIG_ENABLE_LOG_DEBUG                             0

#define PRTOS_CONFIG_LOGGER_UART_BAUD_RATE                        (U32_t)115200

/* #define PRTOS_CONFIG_USE_LOGGER_MODE_FILE */ /* !!!NOT YET AVAILABLE IN V 0.4.1!!! */
#define PRTOS_CONFIG_USE_LOGGER_MODE_UART

/* Enable the Shell module. The shell provides a Command Line Interface.
/* TODO: Shell UART linkage. */
#define PRTOS_CONFIG_ENABLE_SHELL                                 0

/* !!!NOT YET AVAILABLE IN V 0.4.1!!! */
#define PRTOS_CONFIG_ENABLE_TASKTRACE                             0

/******************************************/





#endif
/**********************************************************************************************************************************************
*  File: Core.c
*
*  Description: Prior Core contains OS tick and scheduler
*  functions.
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
*  Copyright� 2017    D. van de Spijker
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

/* Prior RTOS includes. */
#include "include/Os.h"
#include "kernel/inc/CoreDef.h"
#include "port/PortCore.h"

#include "include/PriorRTOS.h"

#include "kernel/inc/Scheduler.h"
#include "include/Logger.h"
#include "kernel/inc/LoggerDef.h"
#include "include/Convert.h"
#include "kernel/inc/KernelTask.h"
#include "kernel/inc/List.h"
#include "kernel/inc/Event.h"
#include "kernel/inc/IdTypeDef.h"
#include "include/IdType.h"
#include "kernel/inc/TaskDef.h"
#include "kernel/inc/MemoryDef.h"

/* Kernel Tasks. */
#include "kernel/inc/ktask/KernelTaskIdle.h"


/*Include standard libraries*/
#include <stdlib.h>
#include <math.h>

LOG_FILE_NAME("Core.c");

#define NUM_TCB_LISTS 2

#if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
#include "kernel/inc/TimerDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_PIPES==1
#include "kernel/inc/PipeDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1
#include "kernel/inc/EventgroupDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_SIGNALS==1
#include "kernel/inc/SignalDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_MAILBOXES==1
#include "kernel/inc/MailboxDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_SEMAPHORES==1
#include "kernel/inc/SemaphoreDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
#include "kernel/inc/RingbufferDef.h"
#endif

#if PRTOS_CONFIG_ENABLE_SHELL==1
#include "include/Shell.h"
OsResult_t KShellInit(void);
#endif


/****************************/

#if  defined(PRTOS_CONFIG_USE_SCHEDULER_COOP) && defined(PRTOS_CONFIG_USE_SCHEDULER_PREEM)
#error "PriorRTOS: Multiple schedulers in use; Both PRTOS_CONFIG_USE_SCHEDULER_COOP and PRTOS_CONFIG_USE_SCHEDULER_PREEM are defined."
#endif

#ifdef PRTOS_CONFIG_USE_SCHEDULER_COOP
static void ICoreTickCoop(void);
#else
static void ICoreTickPreem(void);
#endif
static void ICoreTickInvoke(U32_t tc);

static void ICoreSwitchTask(void);
static U16_t  ICoreCalculatePrescaler(U16_t f_os);

static void ICoreRunTimeUpdate(void);
static void ICoreRunTimeAccumulate(U32_t t_us);

static void ICoreOsIntDisable(void);
static void ICoreOsIntEnable(void);

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
static void ICoreWdtInit(void);
static void ICoreWdtKick();
static void ICoreWdtEnable(U8_t wdt_expire_opt);
static void ICoreWdtDisable();
#endif

/* OS Heap and memory. */
static Pmb_t OsPoolTable[MEM_NUM_POOLS];
#ifndef PRTOS_CONFIG_USE_EXTERNAL_OS_HEAP
static U8_t OsHeap[PRTOS_CONFIG_OS_HEAP_SIZE_BYTES];
#else 
static U8_t *OsHeap = NULL;
#endif

/* Kernel Register */

typedef struct Reg_t {
    volatile U8_t       lock; /* Kernel register lock. */

    U64_t               f_cpu;
    U16_t               prescaler;
    float               f_os_timer;
    U16_t               f_os;
    U16_t               ovf;
    U32_t               tick_period_us;
    
	/* Holds a pointer to the TCB of the currently running task. */
    pTcb_t				tcb_running;
    pTcb_t				tcb_idle;
    LinkedList_t		*tcb_wait_list;
    LinkedList_t		*tcb_list;
    LinkedList_t		*event_list;

    volatile U8_t       isr_nest_counter;
    volatile U8_t       crit_sect_nest_counter;
    volatile U8_t       kernel_mode_nest_counter;
    volatile U8_t       scheduler_lock;
    volatile U16_t      sreg;
    U8_t                os_running;

    volatile U32_t      micros;
    volatile U32_t      hours;
    volatile U32_t      t_accu;  /* Accumulated time since last RunTime update. */

#if PRTOS_CONFIG_ENABLE_CPULOAD_CALC>0
    volatile U8_t           cpu_load;
#endif

} Reg_t;

static Reg_t KernelReg;

static LinkedList_t *TcbLists[NUM_TCB_LISTS] = {NULL};

#define KERNEL_REG_LOCK()       \
if(KernelReg.lock < 0xFF) {     \
    KernelReg.lock++;           \

#define KERNEL_REG_UNLOCK()     \
  KernelReg.lock--;             \
}                               \
else { while (1); }             \

#ifdef PRTOS_CONFIG_USE_EXTERNAL_OS_HEAP
void OsHeapSet(U8_t *os_heap)
{
	if(OsHeap == NULL && os_heap != NULL) {
		OsHeap = os_heap;
	}
}
#endif

OsResult_t OsInit(OsResult_t *status_optional)
 {
	if(status_optional == NULL) {
		return OS_RES_INVALID_ARGUMENT;
	}

    OsResult_t status_kernel   = OS_RES_OK; //status tracker for essential kernel modules
    *status_optional = OS_RES_OK; //status tracker for optional modules
    KernelReg.lock = 0;
    KERNEL_REG_LOCK() {
        KernelReg.os_running = 1;
        KernelReg.kernel_mode_nest_counter = 0;
        KernelReg.isr_nest_counter = 0;
        KernelReg.crit_sect_nest_counter = 0;
        KernelReg.micros = 0;
        KernelReg.hours = 0;
        KernelReg.t_accu = 0;
        KernelReg.scheduler_lock = 0;
    }
    KERNEL_REG_UNLOCK();
    KCoreFlagClear(CORE_FLAG_ALL);//clear all flags

    OsCritSectBegin();
    KCoreKernelModeEnter(); //Enable kernel mode

#if ( PRTOS_CONFIG_ENABLE_LOG_DEBUG == 1 || PRTOS_CONFIG_ENABLE_LOG_ERROR == 1 || PRTOS_CONFIG_ENABLE_LOG_INFO == 1 || PRTOS_CONFIG_ENABLE_LOG_EVENT == 1 )
    KLogInit();
#endif

    char os_version_buf[CONVERT_BUFFER_SIZE_OSVERSION_TO_STRING];
    ConvertOsVersionToString(OS_VERSION, os_version_buf);
    LOG_INFO_NEWLINE("Prior RTOS %s",os_version_buf);

    /*Initiate essential modules*/
    LOG_INFO_NEWLINE("Initializing kernel...");

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    ICoreWdtInit();
    ICoreWdtEnable(PORT_WDT_EXPIRE_8_S);
#endif

    LOG_INFO_NEWLINE("Initializing OS Timer and Tick interrupt...");
    U16_t os_timer_ovf = ICoreCalculatePrescaler(PRTOS_CONFIG_F_OS_HZ);
    PortOsTimerInit(KernelReg.prescaler, os_timer_ovf);
    PortOsIntInit((IrqPriority_t)PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY);
    PortOsIntDisable();
    LOG_INFO_APPEND("ok");
    
    LOG_INFO_NEWLINE("Initializing IDs...");
    KIdInit();
    LOG_INFO_APPEND("ok");
    
    /* Initialize memory management */
    LOG_INFO_NEWLINE("Initializing module:Memory Management...");
    status_kernel = KMemInit(OsHeap, PRTOS_CONFIG_OS_HEAP_SIZE_BYTES, PRTOS_CONFIG_USER_HEAP_SIZE_BYTES,
    		OsPoolTable); 
    if(status_kernel == OS_RES_CRIT_ERROR) {
        LOG_ERROR_NEWLINE("critical error");
        return status_kernel;
    }
    LOG_INFO_APPEND("ok");

    /* Initialize task management */
    LOG_INFO_NEWLINE("Initializing module:Task...");
    status_kernel = KTaskInit();
    if(status_kernel == OS_RES_CRIT_ERROR) {
        LOG_ERROR_NEWLINE("critical error");
        return status_kernel;
    }
    KernelReg.tcb_list = KTcbListRefGet();
    KernelReg.tcb_wait_list = KTcbWaitListRefGet();
    KernelReg.tcb_idle = KTcbIdleGet();
    KernelReg.tcb_running = NULL;
    TcbLists[0] = KernelReg.tcb_wait_list;
    TcbLists[1] = KernelReg.tcb_list;

    LOG_INFO_APPEND("ok");

    LOG_INFO_NEWLINE("Initializing module:Event...");
    status_kernel = EventInit();
    if(status_kernel != OS_RES_OK) {
        LOG_ERROR_NEWLINE("Failed to initialize module: Event!");
        return status_kernel;
    }
    KernelReg.event_list = EventListRefGet();
    LOG_INFO_APPEND("ok");


    /*Core initiations*/
    LOG_INFO_NEWLINE("Initializing Scheduler...");
    KSchedulerInit(KernelReg.event_list); /* Initialize Scheduler */
    LOG_INFO_APPEND("ok");



#if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
    LOG_INFO_NEWLINE("Initializing module:Timer...");
    status_kernel = KTimerInit();
    if(status_kernel == OS_RES_CRIT_ERROR) {
        LOG_ERROR_NEWLINE("No timer ID buffer defined");
        return status_kernel;
    }
    LOG_INFO_APPEND("ok");
#endif


#if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1
    LOG_INFO_NEWLINE("Initializing module:Eventgroup...");
    KEventgroupInit();
    LOG_INFO_APPEND("ok");
#endif

#if PRTOS_CONFIG_ENABLE_MAILBOXES==1
    LOG_INFO_NEWLINE("Initializing module:Mailbox...");
    KMailboxInit();
    LOG_INFO_APPEND("ok");
#endif

#if PRTOS_CONFIG_ENABLE_SIGNALS==1
    LOG_INFO_NEWLINE("Initializing module:Signal...");
    KSigInit();
    LOG_INFO_APPEND("ok");
#endif

#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
    LOG_INFO_NEWLINE("Initializing module:Ringbuffer...");
    KRingbufInit();
    LOG_INFO_APPEND("ok");

#if PRTOS_CONFIG_ENABLE_SHELL==1
    LOG_INFO_NEWLINE("Initializing module:Shell...");
    KShellInit();
    LOG_INFO_APPEND("ok");
#endif

#endif

#if PRTOS_CONFIG_ENABLE_PIPES==1
    LOG_INFO_NEWLINE("Initializing module:Pipe...");
    KPipeInit();
    LOG_INFO_APPEND("ok");
#endif

#if PRTOS_CONFIG_ENABLE_SEMAPHORES==1
    LOG_INFO_NEWLINE("Initializing module:Semaphore...");
    KSemaphoreInit();
    LOG_INFO_APPEND("ok");
#endif

    KCoreKernelModeExit(); //Clear kernel mode flag
    OsCritSectEnd();

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    ICoreWdtDisable();
#endif

    LOG_INFO_NEWLINE("Kernel successfully initialized");

    return status_kernel;
}

void OsStart(Id_t start_task_id)
{
	U8_t *running = &KernelReg.os_running;;

	OsCritSectBegin();

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    ICoreWdtEnable(PORT_WDT_EXPIRE_120MS);
#endif

    KCoreKernelModeEnter(); //Disable kernel mode

    LOG_INFO_NEWLINE("Preparing first task for execution...");
    if (start_task_id != ID_INVALID) { //if Prior should not start with Idle task
        KernelReg.tcb_running = KTcbFromId(start_task_id); //Search for TCB
        if (KernelReg.tcb_running == NULL) {
            LOG_ERROR_NEWLINE("Specified task not found! Starting with Idle instead.");
            KernelReg.tcb_running = KernelReg.tcb_idle; //No valid TCB found
        }

    } else {
        KernelReg.tcb_running = KernelReg.tcb_idle; //Prior should start with Idle task
    }

    ListNodeRemove(KernelReg.tcb_list, &KernelReg.tcb_running->list_node);
	KTaskStateSet(KernelReg.tcb_running, TASK_STATE_RUNNING);
	KTcbRunningRefSet(KernelReg.tcb_running);

    KCoreKernelModeExit(); //Disable kernel mode

    LOG_INFO_APPEND("ok");


    LOG_INFO_NEWLINE("Starting Prior RTOS tick at %u Hz...", KernelReg.f_os);
    OsSchedulerUnlock();
    ICoreOsIntEnable();
    PortOsTimerTicksReset();


    OsCritSectEnd();
    PortOsTimerEnable();
    LOG_INFO_APPEND("running");

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    ICoreWdtDisable();
    ICoreWdtEnable(PORT_WDT_EXPIRE_8_S);
#endif

    /* Embedded infinite run-loop. */
    while (*running) {

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
        ICoreWdtKick();
#endif
        if(KernelReg.tcb_running->handler != NULL) {

#ifdef PRTOS_CONFIG_USE_EVENT_TASK_EXECUTE_EXIT
            EventEmit(KernelReg.tcb_running->list_node.id, TASK_EVENT_EXECUTE, EVENT_FLAG_NONE);
#endif

            (KernelReg.tcb_running->handler)(KernelReg.tcb_running->p_arg, KernelReg.tcb_running->v_arg);

#ifdef PRTOS_CONFIG_USE_EVENT_TASK_EXECUTE_EXIT
            EventEmit(KernelReg.tcb_running->list_node.id, TASK_EVENT_EXIT, EVENT_FLAG_NONE);
#endif

        } else {
            LOG_ERROR_NEWLINE("Current task handler is null!");
            while(1);
        }
        ICoreTickInvoke(PortOsTimerTicksGet());
    }

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    ICoreWdtDisable();
#endif
}

void OsStop(void)
{
    KernelReg.os_running = 0;
    /* TODO: De-init all modules. Delete everything! */
}

void OsReset(void)
{
    /* TODO: Implementation. */
}

U16_t OsFrequencyGet(void)
{
    return KernelReg.f_os;
}

OsVer_t OsVersionGet(void)
{
    return ((OsVer_t)OS_VERSION);
}

U32_t OsRunTimeMicrosDelta(U32_t us)
{
    U32_t delta_us = 0;
    U32_t curr_us = OsRunTimeMicrosGet();
    if(curr_us != 0) {
        if(curr_us >= us) {
            delta_us = curr_us - us;
        } else {
            delta_us = us - curr_us;
        }
        us = curr_us;
    }
    return delta_us;
}

OsResult_t OsRunTimeGet(OsRunTime_t runtime)
{
    //Input check
    if(runtime[0] != 0 || runtime[1] != 0) {
        return OS_RES_FAIL;
    }
    OsResult_t result = OS_RES_LOCKED;
    KERNEL_REG_LOCK() {
        runtime[0] = KernelReg.hours;
        runtime[1] = KernelReg.micros;
        result = OS_RES_OK;
    }
    KERNEL_REG_UNLOCK();

    return result;
}

U32_t OsRunTimeMicrosGet(void)
{
    U32_t micros = 0;
    KERNEL_REG_LOCK() {
        micros = KernelReg.micros;
    }
    KERNEL_REG_UNLOCK();

    return micros;
}

U32_t OsRuntimeHoursGet(void)
{
    U32_t hours = 0;
    KERNEL_REG_LOCK() {
        hours = KernelReg.hours;
    }
    KERNEL_REG_UNLOCK();

    return hours;
}

U32_t OsTasksTotalGet(void)
{
    return (ListSizeGet(KernelReg.tcb_list) + ListSizeGet(KernelReg.tcb_wait_list) + KSchedulerQueueSizeGet() + 1);
}


U32_t OsTasksActiveGet(void)
{
    return (ListSizeGet(KernelReg.tcb_wait_list) + KSchedulerQueueSizeGet() + 1);
}

U32_t OsEventsTotalGet(void)
{
    return (EventListSizeGet(KernelReg.event_list));
}


bool OsTaskExists(Id_t task_id)
{
    if(IdIsInGroup(task_id, ID_GROUP_TASK) == 0) {
        return false;
    }
    pTcb_t tcb = KTcbFromId(task_id);
    if(tcb == NULL) {
        return false;
    } else {
        return true;
    }
}

Id_t OsCurrentTaskGet(void)
{
    Id_t id = ID_INVALID;
    if(ListNodeLock(&KernelReg.tcb_running->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
        id = ListNodeIdGet(&KernelReg.tcb_running->list_node);
        ListNodeUnlock(&KernelReg.tcb_running->list_node);
    }
    return id;
}

void OsCritSectBegin(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.crit_sect_nest_counter < 0xFF) {
            KernelReg.crit_sect_nest_counter++;
        } else {
            LOG_ERROR_NEWLINE("Critical section nest-count overflow.");
        }

        if(KernelReg.crit_sect_nest_counter == 1) {
            PortGlobalIntDisable();
        }
    }
    KERNEL_REG_UNLOCK();


}


void OsCritSectEnd()
{
    KERNEL_REG_LOCK() {
        if(KernelReg.crit_sect_nest_counter > 0) {
            KernelReg.crit_sect_nest_counter--;
        }  else {
            LOG_ERROR_NEWLINE("Critical section nest-count underflow.");
        }

        if(KernelReg.crit_sect_nest_counter == 0) {
            PortGlobalIntEnable();
        }
    }
    KERNEL_REG_UNLOCK();
}

void OsIsrBegin(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.isr_nest_counter < 127) {
            KernelReg.isr_nest_counter++;
        }
    }
    KERNEL_REG_UNLOCK();
}

void OsIsrEnd(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.isr_nest_counter > 0) {
            KernelReg.isr_nest_counter--;
        }
    }
    KERNEL_REG_UNLOCK();
}


S8_t OsIsrNestCountGet(void)
{
    S8_t nest_count = -1;
    KERNEL_REG_LOCK() {
        nest_count = KernelReg.isr_nest_counter;
    }
    KERNEL_REG_UNLOCK();

    return nest_count;
}

void OsSchedulerLock(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.scheduler_lock < 255) {
            KernelReg.scheduler_lock++;
        }
    }
    KERNEL_REG_UNLOCK();
}

void OsSchedulerUnlock(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.scheduler_lock > 0) {
            KernelReg.scheduler_lock--;
        }
    }
    KERNEL_REG_UNLOCK();
}

bool OsSchedulerIsLocked(void)
{
    bool scheduler_lock = false;
    KERNEL_REG_LOCK() {
        scheduler_lock = (KernelReg.scheduler_lock ? true : false);
    }
    KERNEL_REG_UNLOCK();
    return scheduler_lock;
}


U32_t OsTickPeriodGet(void)
{
    return KernelReg.tick_period_us;
}

/*******************************/



/*********OS Runtime****************/

static void ICoreRunTimeAccumulate(U32_t t_us)
{

    KERNEL_REG_LOCK() {
        KernelReg.t_accu += t_us;
    }
    KERNEL_REG_UNLOCK();
}

static void ICoreRunTimeUpdate(void)
{
    /* Accumulated time in us. Only cleared when
    * the Kernel Reg lock is successful. */
    KERNEL_REG_LOCK() {
        KernelReg.micros += KernelReg.t_accu;
        KernelReg.t_accu = 0;
        if(KernelReg.micros >= USEC_PER_HOUR) {
            KernelReg.micros = 0;
            KernelReg.hours++;
        }
    }
    KERNEL_REG_UNLOCK();
}

/********************************/


/************OS Tick***********/

void OsTick(void)
{
	if(KernelReg.tcb_running != NULL) {
#ifdef PRTOS_CONFIG_USE_SCHEDULER_COOP
		ICoreTickCoop();
#else
		ICoreTickPreem();
#endif
	}
}

#ifdef PRTOS_CONFIG_USE_SCHEDULER_COOP

/* Cooperative Tick. */
static void ICoreTickCoop(void)
{
    OsIsrBegin();
    OsCritSectBegin();
    KCoreKernelModeEnter();

    if(!KCoreFlagGet(CORE_FLAG_TICK)) {
        KCoreFlagSet(CORE_FLAG_TICK);

        if(!KCoreFlagGet(CORE_FLAG_DISPATCH)) {
            ICoreRunTimeAccumulate(KernelReg.tick_period_us);
        }
        /* Update OS run-time */
        ICoreRunTimeUpdate();

        /* Update run-time of current task. */
        KTaskRunTimeUpdate();

        /* Only switch tasks if dispatch flag is set and if the ISR nesting level is singular.
        * This way all other interrupts will finish avoiding problems arising when a task
        * switch would occur during a user interrupt.
        * The task executing at the beginning of any user interrupt is then identical to
        * the one executing at the end. */
        if(KCoreFlagGet(CORE_FLAG_DISPATCH) && (KernelReg.isr_nest_counter == 1)) {
            KTaskRunTimeReset(KernelReg.tcb_running); /* Reset the tasks run time. */
            ICoreSwitchTask();
            KCoreFlagClear(CORE_FLAG_DISPATCH);
        }

        /* Scheduler will only run if all required lists and the scheduler itself are unlocked. */
        if(!KernelReg.scheduler_lock && !ListIsLocked(KernelReg.tcb_wait_list) && !ListIsLocked(KernelReg.tcb_list) && !ListIsLocked(KernelReg.event_list)) {
        	KSchedulerCycle(TcbLists, NUM_TCB_LISTS);
        }

        if(KSchedulerQueueSizeGet()) {
            KCoreFlagClear(CORE_FLAG_IDLE);
        }
        KCoreFlagClear(CORE_FLAG_TICK);
    }

    KCoreKernelModeExit();
    OsCritSectEnd();
    OsIsrEnd();

    PortOsIntFlagClear();
}

#else

static void ICoreTickPreem(void)
{

}

#endif

static void ICoreTickInvoke(U32_t tc)
{
    /* Calculate time passed since last Tick interrupt and update t_accu. */
    ICoreRunTimeAccumulate(CORE_OS_TIMER_TICKS_TO_US(tc));

    KCoreFlagSet(CORE_FLAG_DISPATCH); /* Set dispatch flag so Tick interrupt will switch tasks. */
    PortOsTimerTicksSet(KernelReg.ovf); /* Invoke Tick interrupt by setting the timer-counter to its compare value. */
#if PRTOS_CONFIG_ENABLE_WATCHDOG==1
    PortWdtKick();
#endif
    while(KCoreFlagGet(CORE_FLAG_DISPATCH)); /* Wait until Tick interrupt handles the task switch and clears the dispatch flag. */
}

/********************************/




static void ICoreSwitchTask(void)
{
    if(KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return;
    }

#if PRTOS_CONFIG_ENABLE_TASKTRACE==1
    TtUpdate();
#endif

    if (ListSizeGet(&KernelReg.tcb_running->event_list) != 0) { /* If task is waiting for an event. */
        /* Set task state to dormant and insert into tcb_wait_list. */
        KTaskStateSet(KernelReg.tcb_running,TASK_STATE_WAITING);
        ListNodeAddSorted(KernelReg.tcb_wait_list, &KernelReg.tcb_running->list_node);
    } else if(!KTaskFlagGet(KernelReg.tcb_running, TASK_FLAG_DELETE)) {  /* If task is not waiting for an event and will not be deleted. */
        /* Set task state to idle and insert into tcb_list. */
        KTaskStateSet(KernelReg.tcb_running,TASK_STATE_IDLE);
        ListNodeAddSorted(KernelReg.tcb_list, &KernelReg.tcb_running->list_node);
    } else {  /* If task has to be deleted. */
        if(KernelReg.tcb_running->category != TASK_CAT_OS || KernelReg.tcb_running != KernelReg.tcb_idle) {
            if(KTcbDestroy(KernelReg.tcb_running, NULL) != OS_RES_OK) {
                /* TODO: Throw exception. */
            }
        } else {
            /* Idle or other OS task cannot be deleted.
            * Move back to tcb_list. */
            /* TODO: Throw exception for violation. */
            KTaskStateSet(KernelReg.tcb_running,TASK_STATE_IDLE);
            KTaskFlagClear(KernelReg.tcb_running, TASK_FLAG_DELETE);
            ListNodeAddSorted(KernelReg.tcb_list, &KernelReg.tcb_running->list_node);
        }
    }

    KernelReg.tcb_running = KSchedulerQueueTaskPop();
    if(KernelReg.tcb_running == NULL) {
    	ListNodeRemove(KernelReg.tcb_list, &KernelReg.tcb_idle->list_node);
    	KernelReg.tcb_running =  KernelReg.tcb_idle;
    }
	KTaskStateSet(KernelReg.tcb_running, TASK_STATE_RUNNING);
	KTcbRunningRefSet(KernelReg.tcb_running);
}

/******************************************/



/************* Core flag API *************/

void KCoreFlagSet(CoreFlags_t flag)
{
    KERNEL_REG_LOCK() {
//    	if(KernelReg.sreg & flag) {
//    		LOG_ERROR_NEWLINE("Flag=1");
//    	}
        (KernelReg.sreg) |= ((U16_t)flag);
    }
    KERNEL_REG_UNLOCK();
}

void KCoreFlagClear(CoreFlags_t flag)
{
    KERNEL_REG_LOCK() {
//    	if(flag != CORE_FLAG_IDLE && !(KernelReg.sreg & flag)) {
//    		LOG_ERROR_NEWLINE("Flag=0");
//    	}
        (KernelReg.sreg) &= ~((U16_t)flag);
    }
    KERNEL_REG_UNLOCK();
}

U16_t KCoreFlagGet(CoreFlags_t flag)
{
    /* Not locking the Kernel Register to guarantee that
    * this functions always returns a set of flags instead of
    * 0 which would be the case if a KERNEL_REG_LOCK is
    * unsuccessful. */
    return ((KernelReg.sreg & (U16_t)flag) ? 1 : 0);
}

/*********************************************/




/*************** Kernel Mode API ***************/

U8_t KCoreKernelModeEnter(void)
{
    if(KernelReg.kernel_mode_nest_counter == 0) {
        PortSuperVisorModeEnable(); /* Enable SV mode if available. */
        KCoreFlagSet(CORE_FLAG_KERNEL_MODE);
    }
    if(KernelReg.kernel_mode_nest_counter < 255) {
        KernelReg.kernel_mode_nest_counter++;
        return KernelReg.kernel_mode_nest_counter;
    } else {
    	LOG_ERROR_NEWLINE("Kernel mode overflow.");
    }

    return 0;
}


U8_t KCoreKernelModeExit(void)
{
    if(KernelReg.kernel_mode_nest_counter == 1) {
        PortSuperVisorModeDisable(); /* Disable SV mode if available. */
        KCoreFlagClear(CORE_FLAG_KERNEL_MODE);
    }
    if(KernelReg.kernel_mode_nest_counter> 0) {
        KernelReg.kernel_mode_nest_counter--;
    } else {
    	LOG_ERROR_NEWLINE("Kernel mode underflow.");
    }

    return KernelReg.kernel_mode_nest_counter;
}

Id_t KCoreTaskRunningGet(void)
{
	Id_t id = ID_INVALID;

	KERNEL_REG_LOCK() {
	    if(ListNodeLock(&KernelReg.tcb_running->list_node, LIST_LOCK_MODE_READ) == OS_RES_OK) {
	        id = ListNodeIdGet(&KernelReg.tcb_running->list_node);
	        ListNodeUnlock(&KernelReg.tcb_running->list_node);
	    }
	} KERNEL_REG_UNLOCK();

	return id;
}

/*********************************************/


/************* OS Interrupt API *************/

static void ICoreOsIntEnable(void)
{
    PortOsIntEnable();
    KCoreFlagSet(CORE_FLAG_OS_IRQ_EN);
}


static void ICoreOsIntDisable(void)
{
    PortOsIntDisable();
    KCoreFlagClear(CORE_FLAG_OS_IRQ_EN);
}

/*********************************************/


/************************************************/

static U16_t ICoreCalculatePrescaler(U16_t f_os)
{

    volatile U8_t presc_cnt = 1;
    volatile U16_t ovf;
    volatile float tmp_ovf = 0xFFEE;
    KernelReg.f_cpu = PRTOS_CONFIG_F_CPU_HZ;
    float tick_t = (1/(float)f_os); //in s
    KernelReg.f_os = f_os;

    while (presc_cnt < 5 || tmp_ovf > 0xFFFF) {

        if (presc_cnt == 1) {
            KernelReg.prescaler = 1;
        } else if (presc_cnt == 2) {
            KernelReg.prescaler =8;
        } else if (presc_cnt == 3) {
            KernelReg.prescaler = 65;
        } else if (presc_cnt == 4) {
            KernelReg.prescaler = 256;
        } else if (presc_cnt == 5) {
            KernelReg.prescaler = 1024;
        }

        KernelReg.f_os_timer = PRTOS_CONFIG_F_OS_TIMER_HZ/ KernelReg.prescaler; //in Hz
        tmp_ovf = (tick_t * KernelReg.f_os_timer);

        if(tmp_ovf > 0xFFFF) {
            presc_cnt++;
        } else {
            break;
        }
    }

    ovf = (U16_t)(floor(tmp_ovf) - 1);
    KernelReg.tick_period_us = (U32_t)(tick_t * (U32_t)1000000);
    KernelReg.ovf = ovf;
    return ovf;
}

/************OS Watchdog Timer************/

#if PRTOS_CONFIG_ENABLE_WATCHDOG==1

static void ICoreWdtInit(void)
{
    PortWdtInit(0, 5);
}

static void ICoreWdtEnable(U8_t wdt_expire_opt)
{
    PortWdtEnable(wdt_expire_opt);
}

static void ICoreWdtDisable(void)
{
    PortWdtDisable();
}

static void ICoreWdtKick(void)
{
    PortWdtKick();
}

void OsWdtIsr(void)
{
    OsIsrBegin();
    OsCritSectBegin();
    ICoreWdtDisable();
    LOG_ERROR_NEWLINE("WatchdogTimer overflow.");
    while(1);
    //CoreFlagSet(dispatch_flag);
    ICoreWdtKick();

    OsCritSectEnd();
    OsIsrEnd();
}


#endif

/*************************************/












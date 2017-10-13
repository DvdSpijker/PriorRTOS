/**********************************************************************************************************************************************
*  File: Prior_core.c
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

/* Prior RTOS includes. */
#include <PriorRTOS.h>
#include <Types.h>
#include <Logger.h>
#include <LoggerDef.h>
#include <Convert.h>
#include <KernelTask.h>
#include <List.h>
#include <Event.h>

#include <CoreDef.h>
#include <TaskDef.h>
#include <MmDef.h>

/* Kernel Tasks. */
#include <KernelTaskIdle.h>


/*Include standard libraries*/
#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>


//#define KERNEL_TIMERS   2 /* Number of Kernel timers. */

#define KERNEL_HEAP_SIZE 20
#define OBJECT_HEAP_SIZE (PRTOS_CONFIG_OS_HEAP_SIZE_BYTES - (PRTOS_CONFIG_USER_HEAP_SIZE_BYTES + KERNEL_HEAP_SIZE))

LOG_FILE_NAME("Core.c");


#if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
#include <TimerDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_PIPES==1
#include <PipeDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1
#include <EventgroupDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_SIGNALS==1
#include <SignalDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_MAILBOXES==1
#include <MailboxDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_SEMAPHORES==1
#include <SemaphoreDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
#include <RingbufferDef.h>
#endif

#if PRTOS_CONFIG_ENABLE_PERPHERALS==1
#include <Peripheral.h>
#endif

#if PRTOS_CONFIG_ENABLE_SHELL==1
#include <Shell.h>
OsResult_t ShellInit(void);
#endif


/****************************/

#define KERNEL_HEAP_MAGICWORD (MemBase_t)0xAB

void CoreTick(void);
static void CoreTickInvoke(U32_t tc);

static void CoreSchedulerInit(void);
static void CoreSchedulerCycle(void);
static void CoreEventBrokerCycle(LinkedList_t *activated_task_list);
static void CoreTaskListCompare(LinkedList_t *activated_task_list, LinkedList_t *task_list, pEvent_t compare_event, bool last_event);
static void CoreTaskEventsCompare(LinkedList_t *activated_task_list, LinkedList_t *task_list, pTcb_t task,
LinkedList_t *task_event_list,  pEvent_t compare_event, bool last_event);

void CoreTaskAddDescendingPriority(LinkedList_t *from_list, LinkedList_t *to_list, pTcb_t task);

static void CoreSwitchTask(void);
static OsResult_t CoreLoadNewTask(pTcb_t tcb);
static U16_t  CoreCalculatePrescaler(U16_t f_os);
static void CoreRunTimeUpdate(U32_t t_us);

static void CoreOsIntDisable(void);
static void CoreOsIntEnable(void);

#if PRTOS_CONFIG_ENABLE_WDT==1
static void CoreWdtInit(void);
static void CoreWdtKick();
static void CoreWdtEnable(U8_t wdt_expire_opt);
static void CoreWdtDisable();
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

    U8_t                ostick_cnt;
    volatile U8_t       isr_nest_counter;
    volatile U8_t       crit_sect_nest_counter;
    volatile U8_t       kernel_mode_nest_counter;
    volatile U8_t       scheduler_lock;
    volatile U16_t      sreg;
    U8_t                os_running;

    //Id_t                OS_TimerTable[KERNEL_TIMERS];

    volatile U32_t      micros;
    volatile U32_t      hours;
    volatile U32_t      t_accu;  /* Accumulated time since last TimerUpdateAll. */
    volatile U8_t       n_ticks; /* Number of ticks occurred since last TimerUpdateAll. */

    Id_t                kernel_heap;
    Id_t                object_heap; /* Holds all objects created by kernel modules. */
    U16_t               object_count;
    MemBase_t*          heap_mgw0;


    #if PRTOS_CONFIG_ENABLE_CPULOAD_CALC>0
    volatile U8_t           cpu_load;
    #endif

} Reg_t;

static Reg_t KernelReg;

#define KERNEL_REG_LOCK()       \
if(KernelReg.lock < 255) {      \
    if(KernelReg.lock == 0) {   \
        PortGlobalIntDisable(); \
    }                           \
    KernelReg.lock++;           \

    #define KERNEL_REG_UNLOCK()     \
}                               \
if(KernelReg.lock > 0) {        \
    if(KernelReg.lock == 1) {   \
        PortGlobalIntEnable();  \
    }                           \
    KernelReg.lock--;            \
}                               \

OsResult_t OsInit(OsResult_t *status_optional)
{

    OsResult_t status_kernel   = OS_OK; //status tracker for essential kernel modules
    *status_optional = OS_OK; //status tracker for optional modules
    KernelReg.lock = 0;
    KERNEL_REG_LOCK() {
        KernelReg.os_running = 1;
        KernelReg.kernel_mode_nest_counter = 0;
        KernelReg.isr_nest_counter = 0;
        KernelReg.crit_sect_nest_counter = 0;
        KernelReg.micros = 0;
        KernelReg.hours = 0;
        KernelReg.t_accu = 0;
        KernelReg.n_ticks = 0;
        KernelReg.ostick_cnt = 0;
        KernelReg.scheduler_lock = 0;
        KernelReg.kernel_heap = INVALID_ID;
        KernelReg.object_heap = INVALID_ID;
        KernelReg.object_count = 0;
    }
    KERNEL_REG_UNLOCK();

    CoreFlagClear(CORE_FLAG_ALL);//clear all flags
    CoreKernelModeEnter(); //Enable kernel mode

    #if( PRTOS_CONFIG_ENABLE_LOG_DEBUG == 1 || PRTOS_CONFIG_ENABLE_LOG_ERROR == 1 || PRTOS_CONFIG_ENABLE_LOG_INFO == 1 )
    LogInit();
    #endif

    char os_version_buf[CONVERT_BUFFER_SIZE_OSVERSION_TO_STRING];
    ConvertOsVersionToString(OS_VERSION, os_version_buf);
    LOG_INFO_NEWLINE("Prior RTOS %s",os_version_buf);

    /*Initiate essential modules*/
    LOG_INFO_NEWLINE("Initializing kernel...");

    #if PRTOS_CONFIG_ENABLE_WDT==1
    CoreWdtInit();
    CoreWdtEnable(PORT_WDT_EXPIRE_8_S);
    #endif

    LOG_INFO_NEWLINE("Initializing OS Timer and Tick interrupt...");
    U16_t os_timer_ovf = CoreCalculatePrescaler(PRTOS_CONFIG_F_OS_HZ);
    PortGlobalIntDisable();
    PortOsTimerInit(KernelReg.prescaler, os_timer_ovf);
    PortOsTickInit(PRTOS_CONFIG_OS_TICK_IRQ_PRIORITY);
    LOG_INFO_APPEND("ok");

    LOG_INFO_NEWLINE("Initializing module:Memory Management...");
    status_kernel = MmInit(); //Initiate memory management
    if(status_kernel == OS_OUT_OF_BOUNDS) {
        LOG_ERROR_NEWLINE("No pools defined by user...");
    }
    LOG_INFO_APPEND("ok");

    LOG_INFO_NEWLINE("Creating Kernel heap...");

    KernelReg.kernel_heap = MmPoolCreate(KERNEL_HEAP_SIZE); //Create pool for kernel heap
    if(KernelReg.kernel_heap == INVALID_ID) {
        status_kernel = OS_CRIT_ERROR;
        LOG_ERROR_NEWLINE("Invalid pool ID returned");
        return status_kernel;
        } else {
        MmKernelHeapSet(KernelReg.kernel_heap);
        KernelReg.heap_mgw0 = (MemBase_t*)MmAlloc(KernelReg.kernel_heap,sizeof(MemBase_t));
        if(KernelReg.heap_mgw0 == NULL) {
            status_kernel = OS_CRIT_ERROR;
            LOG_ERROR_NEWLINE("Allocation of magic-words in the Kernel heap returned null");
            } else {
            *(KernelReg.heap_mgw0) = (MemBase_t)KERNEL_HEAP_MAGICWORD;
            LOG_INFO_APPEND("ok");
        }

    }

    LOG_INFO_NEWLINE("Creating Object heap...");
    KernelReg.object_heap = MmPoolCreate((OBJECT_HEAP_SIZE - 10)); //Create pool for object heap. /* TODO: Fix PoolCreate. */
    if(KernelReg.object_heap == INVALID_ID) {
        status_kernel = OS_CRIT_ERROR;
        LOG_ERROR_NEWLINE("Invalid pool ID returned");
        return status_kernel;
        } else {
        LOG_INFO_APPEND("ok");
    }

    LOG_INFO_NEWLINE("Initializing module:Task...");
    status_kernel = TaskInit(); //Initiate task management
    if(status_kernel == OS_CRIT_ERROR) {
        LOG_ERROR_NEWLINE("No task ID buffer defined");
        return status_kernel;
    }
    LOG_INFO_APPEND("ok");

    LOG_INFO_NEWLINE("Initializing module:Event...");
    status_kernel = EventInit();
    if(status_kernel != OS_OK) {
        LOG_ERROR_NEWLINE("Failed to initialize module: Event!");
        return status_kernel;
    }
    LOG_INFO_APPEND("ok");


    /*Core initiations*/
    LOG_INFO_NEWLINE("Initializing Scheduler...");
    CoreSchedulerInit(); //Initiate Prior Scheduler
    LOG_INFO_APPEND("ok");



    #if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
    LOG_INFO_NEWLINE("Initializing module:Timer...");
    status_kernel = TimerInit();
    if(status_kernel == OS_CRIT_ERROR) {
        LOG_ERROR_NEWLINE("No timer ID buffer defined");
        return status_kernel;
    }
    LOG_INFO_APPEND("ok");
    #endif


    #if PRTOS_CONFIG_ENABLE_EVENTGROUPS==1
    LOG_INFO_NEWLINE("Initializing module:Eventgroup...");
    EventgroupInit();
    LOG_INFO_APPEND("ok");
    #endif

    #if PRTOS_CONFIG_ENABLE_MAILBOXES==1
    LOG_INFO_NEWLINE("Initializing module:Mailbox...");
    MailboxInit();
    LOG_INFO_APPEND("ok");
    #endif

    #if PRTOS_CONFIG_ENABLE_SIGNALS==1
    LOG_INFO_NEWLINE("Initializing module:Signal...");
    sig_Init();
    LOG_INFO_APPEND("ok");
    #endif

    #if PRTOS_CONFIG_ENABLE_RINGBUFFERS==1
    LOG_INFO_NEWLINE("Initializing module:Ringbuffer...");
    RingbufInit();
    LOG_INFO_APPEND("ok");

    #if PRTOS_CONFIG_ENABLE_SHELL==1
    LOG_INFO_NEWLINE("Initializing module:Shell...");
    ShellInit();
    LOG_INFO_APPEND("ok");
    #endif

    #endif

    #if PRTOS_CONFIG_ENABLE_PIPES==1
    LOG_INFO_NEWLINE("Initializing module:Pipe...");
    PipeInit();
    LOG_INFO_APPEND("ok");
    #endif

    #if PRTOS_CONFIG_ENABLE_SEMAPHORES==1
    LOG_INFO_NEWLINE("Initializing module:Semaphore...");
    SemaphoreInit();
    LOG_INFO_APPEND("ok");
    #endif


    #if PRTOS_CONFIG_ENABLE_PERPHERALS==1
    LOG_INFO_NEWLINE("Initializing module:Peripheral...");
    //HalInit(); //Initiate Peripherals
    LOG_INFO_APPEND("ok");
    #endif


    LOG_INFO_NEWLINE("Creating Kernel tasks and services...");

    /*Create system tasks*/

    /* Idle task is not created using KernelTask create since it is not an OS category task, it should be on the
    * lowest possible priority. However, the Idle task is essential and cannot be deleted. */
    KernelTaskIdIdle = TaskCreate(KernelTaskIdle, TASK_CAT_LOW, 1, TASK_PARAM_ESSENTIAL, 0, NULL, 0);
    if(KernelTaskIdIdle == INVALID_ID) { //Create Idle task, check if successful
        status_kernel = OS_CRIT_ERROR;
        LOG_ERROR_NEWLINE("Invalid ID returned while creating KernelTaskIdle");
        return status_kernel;
        } else {
        TcbIdle = UtilTcbFromId(KernelTaskIdIdle); //Assign pointer to Idle TCB to TCB_idle
        if(TcbIdle == NULL) {
            status_kernel = OS_CRIT_ERROR;
            LOG_ERROR_NEWLINE("KernelTaskIdle could not be found in the task list.");
            return status_kernel;
        }
        LOG_INFO_NEWLINE("KernelTaskIdle created");
    }

    //Set generic task names if enables
    #if PRTOS_CONFIG_ENABLE_TASKNAMES==1
    TaskGenericNameSet(KernelTaskIdle,"KernelTaskIdle");
    #endif

    CoreKernelModeExit(); //Clear kernel mode flag

    #if PRTOS_CONFIG_ENABLE_WDT==1
    CoreWdtDisable();
    #endif

    LOG_INFO_NEWLINE("Kernel successfully initialized");

    return status_kernel;
}

void OsStart(Id_t start_task_id)
{

    #if PRTOS_CONFIG_ENABLE_WDT==1
    CoreWdtEnable(PORT_WDT_EXPIRE_120MS);
    #endif

    CoreKernelModeEnter(); //Disable kernel mode

    LOG_INFO_NEWLINE("Preparing first task for execution...");
    if (start_task_id != INVALID_ID) { //if Prior should not start with Idle task
        TcbRunning = UtilTcbFromId(start_task_id); //Search for TCB
        if (TcbRunning == NULL) {
            LOG_ERROR_NEWLINE("Specified task not found! Starting with Idle instead.");
            TcbRunning = TcbIdle; //No valid TCB found
        }

        } else {
        TcbRunning = TcbIdle; //Prior should start with Idle task
    }

    ListNodeRemove(&TcbList, &TcbRunning->list_node);
    CoreLoadNewTask(TcbRunning);

    CoreKernelModeExit(); //Disable kernel mode

    LOG_INFO_APPEND("ok");


    LOG_INFO_NEWLINE("Starting Prior RTOS tick at %u Hz...", KernelReg.f_os);
    OsSchedulerUnlock();
    CoreOsIntEnable();
    PortGlobalIntEnable();
    PortOsTimerTicksReset();
    PortOsTimerStart();
    LOG_INFO_APPEND("running");

    #if PRTOS_CONFIG_ENABLE_WDT==1
    CoreWdtDisable();
    CoreWdtEnable(PORT_WDT_EXPIRE_8_S);
    #endif

    /* Embedded infinite run-loop. */
    U8_t *running = &KernelReg.os_running;
    while (*running) {

        #if PRTOS_CONFIG_ENABLE_WDT==1
        CoreWdtKick();
        #endif
        if(TcbRunning->handler != NULL) {

            #ifdef PRTOS_CONFIG_USE_EVENT_TASK_EXECUTE_EXIT
            EventPublish(TcbRunning->list_node.id, TASK_EVENT_EXECUTE, EVENT_FLAG_NONE);
            #endif
            (TcbRunning->handler)(TcbRunning->p_arg, TcbRunning->v_arg);

            #ifdef PRTOS_CONFIG_USE_EVENT_TASK_EXECUTE_EXIT
            EventPublish(TcbRunning->list_node.id, TASK_EVENT_EXIT, EVENT_FLAG_NONE);
            #endif
            } else {
            LOG_ERROR_NEWLINE("Current task handler is null!");
        }
        CoreTickInvoke(PortOsTimerTicksGet());
    }

    #if PRTOS_CONFIG_ENABLE_WDT==1
    CoreWdtDisable();
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

OsResult_t OsFrequencySet(U16_t OS_frequency)
{
    if(OS_frequency > 5250 || OS_frequency == 0) {
        return OS_FAIL;    //Check if frequency is within bounds
    }

    OsCritSectBegin();//Disable interrupts

    PortOsTimerStop(); //Stop OS timer
    U16_t sysTimerovf = CoreCalculatePrescaler(OS_frequency); //Calculate new prescaler with new frequency
    PortGlobalIntDisable();
    PortOsTimerInit(KernelReg.prescaler, sysTimerovf);  //Initialize timer with new ovf
    PortOsTimerStart();
    OsCritSectEnd(); //Enable interrupts
    return OS_OK; //Return status
}

U16_t OsFrequencyGet(void)
{
    return KernelReg.f_os;
}

OsVer_t OsVersionGet(void)
{
    return ((OsVer_t)OS_VERSION);
}

OsResult_t OsRuntimeGet(U32_t* target)
{
    //Input check
    if(target[0] != 0x00000000 || target[1] != 0x00000000) {
        return OS_FAIL;
    }
    OsResult_t result = OS_LOCKED;
    KERNEL_REG_LOCK() {
        target[0] = KernelReg.hours;
        target[1] = KernelReg.micros;
        result = OS_OK;
    }
    KERNEL_REG_UNLOCK();

    return result;
}

U32_t OsRuntimeMicrosGet(void)
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
    return (ListSizeGet(&TcbList) + ListSizeGet(&TcbWaitList) + ListSizeGet(&ExecutionQueue) + 1);
}


U32_t OsTasksActiveGet(void)
{
    return (ListSizeGet(&TcbWaitList) + ListSizeGet(&ExecutionQueue) + 1);
}

U32_t OsEventsTotalGet(void)
{
    return (EventListSizeGet(&EventList));
}


bool OsTaskExists(Id_t task_id)
{
    if((task_id & ID_MASK_TYPE) != (U16_t)ID_TYPE_TASK) {
        return false;
    }
    pTcb_t tcb = UtilTcbFromId(task_id);
    if(tcb == NULL) {
        return false;
        } else {
        return true;
    }
}

void OsCritSectBegin(void)
{
    KERNEL_REG_LOCK() {
        if(KernelReg.crit_sect_nest_counter < 255) {
            KernelReg.crit_sect_nest_counter++;
        }
        if(KernelReg.crit_sect_nest_counter == 1) {
            PortGlobalIntDisable();
            CoreFlagClear(CORE_FLAG_OS_IRQ_EN);
        }
    }
    KERNEL_REG_UNLOCK();
}


void OsCritSectEnd()
{
    KERNEL_REG_LOCK() {
        if(KernelReg.crit_sect_nest_counter > 0) {
            KernelReg.crit_sect_nest_counter--;
        }
        if(KernelReg.crit_sect_nest_counter == 0) {
            PortGlobalIntEnable();
            CoreFlagSet(CORE_FLAG_OS_IRQ_EN);
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

static void CoreRunTimeUpdate(U32_t t_us)
{
    /* Accumulated time in us. Only cleared when
    * the Kernel Reg lock is successful. */
    static U32_t t_accu_us = 0;
    t_accu_us += t_us;

    KERNEL_REG_LOCK() {
        KernelReg.micros += t_accu_us;
        t_accu_us = 0;
        if(KernelReg.micros >= 0xD693A400) { //3600.000.000 us i.e. 1 hour.
            KernelReg.micros = 0;
            KernelReg.hours++;
        }
    }
    KERNEL_REG_UNLOCK();
}

/********************************/


/************OS Tick***********/

/* Cooperative Tick. */
void CoreTick(void)
{
    OsIsrBegin();
    CoreOsIntDisable();//Disable OS timer interrupt to make sure OS ticks don't nest
    CoreKernelModeEnter();

    if(!CoreFlagGet(CORE_FLAG_TICK)) {

        CoreFlagSet(CORE_FLAG_TICK);

        CoreRunTimeUpdate(KernelReg.tick_period_us); //Update run time

        /* Update runtime of currently running task. */
        UtilTaskRuntimeAdd(TcbRunning, KernelReg.tick_period_us);

        /* Only switch tasks if dispatch flag is set and if the ISR nesting level is singular.
        * This way all other interrupts will finish avoiding problems arising when a task
        * switch would occur during a user interrupt.
        * The task executing at the beginning of any user interrupt is then identical to
        * the one executing at the end. */
        if(CoreFlagGet(CORE_FLAG_DISPATCH) && (KernelReg.isr_nest_counter == 1)) {
            CoreSwitchTask();
            CoreFlagClear(CORE_FLAG_DISPATCH);
        }




        #if PRTOS_CONFIG_ENABLE_SOFTWARE_TIMERS==1
        /* Check if the TimerList is not locked and the number of ticks has been reached, then
        * update all timers and reset accumulated time and ticks. */
        //if(!ListIsLocked(&TimerList) && KernelReg.n_ticks >= CONFIG_TIMER_UPDATE_PRESCALER_TICKS) {
        //TimerUpdateAll(KernelReg.t_accu);//Update all timers if list is not locked
        //KernelReg.t_accu = 0;
        //KernelReg.n_ticks = 0;
        //} else {
        //KernelReg.t_accu += OsTickPeriodGet();
        //KernelReg.n_ticks++;
        //}
        #endif

        /* Scheduler will only run if all needed lists and the scheduler itself are unlocked. */
        if(!KernelReg.scheduler_lock && !ListIsLocked(&TcbWaitList) && !ListIsLocked(&TcbList) && !ListIsLocked(&EventList)) {
            CoreSchedulerCycle();
        }

        if(ListSizeGet(&ExecutionQueue) > 0) {
            CoreFlagClear(CORE_FLAG_IDLE);
        }
        CoreFlagClear(CORE_FLAG_TICK);
    }

    CoreKernelModeExit();
    PortOsIntEnable();
    OsIsrEnd();

    PortOsIntFlagClear();
}

static void CoreTickInvoke(U32_t tc)
{
    /* Calculate time passed since last Tick interrupt and update t_accu. */
    U32_t t_us = CORE_OS_TIMER_TICKS_TO_US(tc);
    KernelReg.t_accu += t_us;

    CoreFlagSet(CORE_FLAG_DISPATCH); /* Set dispatch flag so Tick interrupt will switch tasks. */
    PortOsTimerTicksSet(KernelReg.ovf); /* Invoke Tick interrupt by setting the timer-counter to its compare value. */
    #if PRTOS_CONFIG_ENABLE_WDT==1
    PortWdtKick();
    #endif
    while(CoreFlagGet(CORE_FLAG_DISPATCH)); /* Wait until Tick interrupt handles the task switch and clears the dispatch flag. */
}

/********************************/


/*********  Scheduler functions *********/

static void CoreSchedulerInit(void)
{
    ListInit(&ExecutionQueue, (Id_t)ID_TYPE_TASK, NULL, 0);
    //LOG_INFO_NEWLINE("ExecutionQueue: %p", &ExecutionQueue);
    /* Add a mock event to the EventList.
    * This ensures that this list will never
    * be empty. An empty list has to be avoided
    * to keep the EventHandle cycle running, since
    * it also needs to update subscribed event
    * lifetimes. */
    if(EventPublish(INVALID_ID, MOCK_EVENT, EVENT_FLAG_NONE) != OS_OK) {
        LOG_ERROR_NEWLINE("Failed to publish the mock event!");
        while(1); /* Trap. Wdt will overflow if enabled. */
    }
}

static void CoreSchedulerCycle(void)
{

    /* TODO: Check scheduler cycle for correct behavior. */

    CoreFlagSet(CORE_FLAG_SCHEDULER);

    ListSize_t list_size;
    CoreEventBrokerCycle(&ExecutionQueue);
    list_size = ListSizeGet(&ExecutionQueue);
    if(list_size == 0) {
        goto clear_scheduler_flag;
    }

    clear_scheduler_flag:
    CoreFlagClear(CORE_FLAG_SCHEDULER);
}

/* Compares all tasks with all occurred events. All lifetimes are updated, expired events
* will be deleted.
* All activated tasks will be placed in the activated task list. */
static void CoreEventBrokerCycle(LinkedList_t *activated_task_list)
{
    if(EventListSizeGet(&EventList) == 0) {
        LOG_ERROR_NEWLINE("EventList is empty. A Mock event should always be present.");
        while(1); /* Trap. Wdt will overflow if enabled. */
    }

    struct ListIterator it;
    pEvent_t occurred_event = NULL;
    /* Loop through occurred event list and compare all activated tasks and
    * all waiting tasks with every occurred event. */
    bool last_event = false;
    
    LIST_ITERATOR_BEGIN(&it, &EventList, LIST_ITERATOR_DIRECTION_FORWARD);
    {
        if(it.current_node != NULL) {

            occurred_event = (pEvent_t)ListNodeChildGet(it.current_node);

            // LOG_DEBUG_APPEND("\nComparing event %04x with:", it.current_node->id);
            if(it.next_node == NULL) {
                last_event = true;
            }

            /* Handle addressed events (not necessarily subscribed). */
            if(EventFlagGet(occurred_event, EVENT_FLAG_ADDRESSED)) {
                /* For addressed events, the source_id field describes the DESTINATION ID instead of the SOURCE ID. */
                pTcb_t tcb = UtilTcbFromId(occurred_event->source_id);
                if(tcb != NULL) {
                    LinkedList_t *list = UtilTcbLocationGet(tcb);
                    if(list != NULL && list != &ExecutionQueue) {
                        UtilTaskStateSet(tcb, TASK_STATE_ACTIVE);
                        CoreTaskAddDescendingPriority(list, activated_task_list, tcb);
                    }
                }
                if(!last_event) {
                    goto event_cleanup;
                }
            }

            CoreTaskListCompare(activated_task_list, &TcbWaitList, occurred_event, last_event);
            CoreTaskListCompare(activated_task_list, activated_task_list, occurred_event, last_event);
            
            event_cleanup:
            /* Increment the occurred event's lifetime and if it has time left. If this is
            * not the case the event is destroyed. EventDestroy automatically removes the
            * event from its list before deleting it. */
            if(!EventIsMock(occurred_event)) {
                if( (EventLifeTimeIncrement(occurred_event, KernelReg.tick_period_us) == -1) ) {
                    //LOG_DEBUG_NEWLINE("Destroying occurred event %p.", occurred_event);
                    EventDestroy(NULL, occurred_event);
                }
            }
            } else {
            LOG_ERROR_NEWLINE("EventList size (%u) is not consistent with the number of reachable nodes", EventListSizeGet(&EventList));
            break;
        }
    }
    LIST_ITERATOR_END(&it);

    return;
}

/* Compares all tasks and their subscribed events in the Task List with the compare event. All activated tasks
* will be moved to the activated task list. */
static void CoreTaskListCompare(LinkedList_t *activated_task_list, LinkedList_t *task_list, pEvent_t compare_event, bool last_event)
{
    LinkedList_t *task_event_list = NULL;
    pTcb_t task = NULL;
    struct ListIterator it;

    /* Loop through the Task List and compare the occurred event with all subscribed events for
    * every task. */
    LIST_ITERATOR_BEGIN(&it, task_list, LIST_ITERATOR_DIRECTION_FORWARD);
    {
        if(it.current_node != NULL) {
            task = (pTcb_t)ListNodeChildGet(it.current_node);
            //LOG_DEBUG_APPEND("\n\tEvent list of task %04x", it.current_node->id);
            task_event_list = &task->event_list;
            CoreTaskEventsCompare(activated_task_list, task_list, task, task_event_list, compare_event, last_event);
            } else {
            break;
        }
    }
    LIST_ITERATOR_END(&it);

}

/* Compares the Task Event List with the compare event, if the compared event appears in the list the
* task is moved to the activated task list. */
static void CoreTaskEventsCompare(LinkedList_t *activated_task_list, LinkedList_t *task_list, pTcb_t task,
LinkedList_t *task_event_list, pEvent_t compare_event, bool last_event)
{
    pEvent_t task_event = NULL;
    struct ListIterator it;

    /* Loop through Task Event List and compare. */
    LIST_ITERATOR_BEGIN(&it, task_event_list, LIST_ITERATOR_DIRECTION_FORWARD);
    {
        if(it.current_node != NULL) {
            task_event = (pEvent_t)ListNodeChildGet(it.current_node);
            //LOG_DEBUG_APPEND("\n\t\tComparing task event %04x.", it.current_node->id);
            /* If the occurred event matches the subscribed event,
            * set occurred flag and make task active. Then increment
            * its occurrence counter and reset its lifetime. */
            if (EventIsEqual(compare_event, task_event)) { /* Handle subscribed events. */
                EventFlagSet(task_event, EVENT_FLAG_OCCURRED);
                EventFlagClear(task_event, EVENT_FLAG_TIMED_OUT); /* Clear timed-out flag in case it was set during a previous cycle. */
                EventLifeTimeReset(task_event);
                EventOccurrenceCountIncrement(task_event);
                } else if(last_event) { /* If the subscribed event does not match the occurred event. */
                /* Check if the event has a specified time-out, if this is true
                * increment its lifetime. If the event is timed-out, set timed-out flag. */
                if((!EventFlagGet(task_event, EVENT_FLAG_NO_TIMEOUT)) && (!EventFlagGet(task_event, EVENT_FLAG_TIMED_OUT))) {
                    /// LOG_DEBUG_NEWLINE("Updating lifetime of event %04x of task %04x", task_event->list_node.id, task->list_node.id);
                    if(EventLifeTimeIncrement(task_event, KernelReg.tick_period_us) == -1) {
                        EventFlagSet(task_event, EVENT_FLAG_TIMED_OUT);
                        //LOG_DEBUG_NEWLINE("Event (%04x) of task %04x timed out", task_event->list_node.id, task->list_node.id);
                    }
                }
            }

            if( EventFlagGet(task_event, EVENT_FLAG_OCCURRED) || EventFlagGet(task_event, EVENT_FLAG_TIMED_OUT) ) {
                UtilTaskStateSet(task, TASK_STATE_ACTIVE);
                if(EventFlagGet(task_event, EVENT_FLAG_NO_HANDLER)) {
                    //LOG_DEBUG_NEWLINE("Subscribed event %p has no handler.", task_event);
                    if(!EventFlagGet(task_event, EVENT_FLAG_PERMANENT)) {
                        EventDestroy(task_event_list, task_event);
                        } else {
                        EVENT_FLAG_CLEAR(task_event->event_code, EVENT_FLAG_OCCURRED);
                        EventLifeTimeReset(task_event);
                        EventOccurrenceCountReset(task_event);
                    }
                }
            }
            } else {
            break;
        }
    }
    LIST_ITERATOR_END(&it);

    if((task_list != activated_task_list) && (task->state == TASK_STATE_ACTIVE)) {
        CoreTaskAddDescendingPriority(task_list, activated_task_list, task);
    }

}


void CoreTaskAddDescendingPriority(LinkedList_t *from_list, LinkedList_t *to_list, pTcb_t task)
{
    /* TODO: Insert at ExecutionQueueSeparator tail if task category is real-time. */

    pTcb_t compare_task = NULL;
    struct ListIterator it;

    if(ListNodeRemove(from_list, &task->list_node) == NULL) {
        LOG_ERROR_NEWLINE("Removing task %04x from list %p failed.", task, from_list);
        while(1);
    }

    /* Add task to list head if the list is empty.
    * Else loop through list and compare priorities. */
    if(ListSizeGet(to_list) == 0) {
        //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x at head.", task->list_node.id);
        if(ListNodeAddAtPosition(to_list, &task->list_node, LIST_POSITION_HEAD) != OS_OK) {
            LOG_ERROR_NEWLINE("Adding task %04x to list %p failed.", task, from_list);
        }
        } else {
        LIST_ITERATOR_BEGIN(&it, to_list, LIST_ITERATOR_DIRECTION_FORWARD);
        {
            /* If the current node exists, compare priorities,
            * else add task to the tail. */
            if(it.current_node != NULL) {
                compare_task = (pTcb_t)ListNodeChildGet(it.current_node);
                //LOG_DEBUG_APPEND("\n\t\t\tComparing priorities of tasks %04x and %04x.", task->list_node.id, it.current_node->id);
                /* Compare priorities, only add and return if task priority is higher.
                * If the priority is not higher, compare again on next iteration at next node. */
                if(task->priority > compare_task->priority) {
                    //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x before %04x.", task->list_node.id, it.current_node->id);
                    if(ListNodeAddAtNode(to_list, &task->list_node, it.current_node, LIST_ADD_BEFORE) != OS_OK) {
                        LOG_ERROR_NEWLINE("Adding task %04x to list %p failed.", task, from_list);
                        while(1);
                    }
                    return;
                }
                } else {
                //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x at tail.", task->list_node.id);
                if(ListNodeAddAtPosition(to_list, &task->list_node, LIST_POSITION_TAIL) != OS_OK) {
                    LOG_ERROR_NEWLINE("Adding task %04x to list %p failed.", task, from_list);
                    while(1);
                }
                return;
            }
        }
        LIST_ITERATOR_END(&it);

        /* This will only be reached if the task had no higher priority than any of
        * the other tasks in the list. Therefore it is safe to add this task to the tail. */
        //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x at tail.", task->list_node.id);
        ListNodeAddAtPosition(to_list, &task->list_node, LIST_POSITION_TAIL);
    }

}

static void CoreSwitchTask(void)
{
    if(CoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return;
    }
    CoreFlagSet(CORE_FLAG_DISPATCH);

    #if PRTOS_CONFIG_ENABLE_TASKTRACE>0
    TtUpdate();
    #endif

    //EventListPrint(&TcbRunning->event_list);

    if (UtilTaskFlagGet(TcbRunning, TASK_FLAG_WAIT_ONCE) || UtilTaskFlagGet(TcbRunning, TASK_FLAG_WAIT_PERMANENT)) { /* If task is waiting for an event. */
        /* Set task state to dormant and insert into TcbWaitList. */
        UtilTaskStateSet(TcbRunning,TASK_STATE_WAITING);
        UtilTaskFlagClear(TcbRunning, TASK_FLAG_WAIT_ONCE);
        TcbRunning->run_time_us = TcbRunning->run_time_us / 2;
        ListNodeAddSorted(&TcbWaitList, &TcbRunning->list_node);
        } else if(!UtilTaskFlagGet(TcbRunning, TASK_FLAG_DELETE)) {  /* If task is not waiting for an event and will not be deleted. */
        /* Set task state to idle and insert into TcbList. */
        UtilTaskStateSet(TcbRunning,TASK_STATE_IDLE);
        TcbRunning->run_time_us = TcbRunning->run_time_us / 2;
        ListNodeAddSorted(&TcbList, &TcbRunning->list_node);
        } else if (UtilTaskFlagGet(TcbRunning, TASK_FLAG_DELETE)) {  /* If task has to be deleted. */
        if(TcbRunning->category != TASK_CAT_OS || TcbRunning != TcbIdle) {
            UtilTcbDestroy(TcbRunning, NULL);
            } else {
            /* Idle or other OS task cannot be deleted.
            * Move back to TcbList. */
            /* TODO: Throw exception for violation. */
            UtilTaskStateSet(TcbRunning,TASK_STATE_IDLE);
            UtilTaskFlagClear(TcbRunning, TASK_FLAG_DELETE);
            ListNodeAddSorted(&TcbList, &TcbRunning->list_node);
        }
    }
    TcbRunning = NULL;
    CoreLoadNewTask(NULL);

    CoreFlagClear(CORE_FLAG_DISPATCH);
    CoreFlagClear(CORE_FLAG_IDLE);
}

/* Loads a TCB as new running task. TCB has to be
* disconnected from any lists.
* If TCB = NULL, the first task in the ExecutionQueue
* will be loaded. If no task is present in the queue,
* the Idle task is loaded instead. */
static OsResult_t CoreLoadNewTask(pTcb_t tcb)
{
    OsResult_t result = OS_OK;

    if(tcb == NULL) {
        if (ListSizeGet(&ExecutionQueue) > 0) {
            TcbRunning = (pTcb_t)ListNodeChildGet(ListNodeRemoveFromHead(&ExecutionQueue));
            } else {
            TcbRunning = TcbIdle;
            ListNodeRemove(&TcbList, &TcbIdle->list_node);
        }
        } else {
        TcbRunning = tcb;
    }

    UtilTaskStateSet(TcbRunning, TASK_STATE_RUNNING);

    return result;
}

/******************************************/



/************* Core flag API *************/

void CoreFlagSet(CoreFlags_t flag)
{
    KERNEL_REG_LOCK() {
        (KernelReg.sreg) |= ((U16_t)flag);
    }
    KERNEL_REG_UNLOCK();
}

void CoreFlagClear(CoreFlags_t flag)
{
    KERNEL_REG_LOCK() {
        (KernelReg.sreg) &= ~((U16_t)flag);
    }
    KERNEL_REG_UNLOCK();
}

U16_t CoreFlagGet(CoreFlags_t flag)
{
    /* Not locking the Kernel Register to guarantee that
    * this functions always returns a set of flags instead of
    * 0 which would be the case if a KERNEL_REG_LOCK is
    * unsuccessful. */
    return ((KernelReg.sreg & (U16_t)flag) ? 1 : 0);
}

/*********************************************/




/*************** Kernel Mode API ***************/

U8_t CoreKernelModeEnter(void)
{
    OsCritSectBegin();
    if(KernelReg.kernel_mode_nest_counter == 0) {
        PortSuperVisorModeEnable(); /* Enable SV mode if available. */
        CoreFlagSet(CORE_FLAG_KERNEL_MODE);
    }
    if(KernelReg.kernel_mode_nest_counter < 255) {
        KernelReg.kernel_mode_nest_counter++;
        return KernelReg.kernel_mode_nest_counter;
    }

    return 0;
}


U8_t CoreKernelModeExit(void)
{
    OsCritSectEnd();
    if(KernelReg.kernel_mode_nest_counter == 1) {
        PortSuperVisorModeDisable(); /* Disable SV mode if available. */
        CoreFlagClear(CORE_FLAG_KERNEL_MODE);
    }
    if(KernelReg.kernel_mode_nest_counter> 0) {
        KernelReg.kernel_mode_nest_counter--;
    }

    return KernelReg.kernel_mode_nest_counter;
}

/*********************************************/


/************* OS Interrupt API *************/

static void CoreOsIntEnable(void)
{
    PortOsIntEnable();
    CoreFlagSet(CORE_FLAG_OS_IRQ_EN);
}


static void CoreOsIntDisable(void)
{
    PortOsIntDisable();
    CoreFlagClear(CORE_FLAG_OS_IRQ_EN);
}

/*********************************************/


/************* Object allocation API *************/

void *CoreObjectAlloc(U32_t obj_size, U32_t obj_data_size, void **obj_data)
{
    CoreKernelModeEnter(); /* Enter Kernel Mode to access the object heap. */
    /* Check if there is memory available and the object count has not reached its max. value. */
    if( (KernelReg.object_count == 0xFFFF) || (MmPoolFreeSpaceGet(KernelReg.object_heap) < (obj_size + obj_data_size)) ) {
        goto error;
    }

    /* Allocate the memory for the object and check its validity. */
    void *obj = MmAlloc(KernelReg.object_heap, obj_size);
    if(obj != NULL) {
        if(obj_data_size != 0) {
            if(obj_data == NULL) {
                MmFree(&obj);
                goto error;
            }
            *obj_data = MmAlloc(KernelReg.object_heap, obj_data_size);
            if(*obj_data != NULL) {
                KernelReg.object_count++;
                goto valid;
                } else { /* Object data allocation failed => Free the object. */
                MmFree(&obj);
                goto error;
            }
            } else { /* No object data has to be allocated. */
            KernelReg.object_count++;
            goto valid;
        }
    }


    error:
    CoreKernelModeExit();
    while(1);
    /* TODO: Throw exception. */
    return NULL;

    valid:
    CoreKernelModeExit();
    return obj;
}

OsResult_t CoreObjectFree(void **obj, void **obj_data)
{
    OsResult_t result = OS_ERROR;
    result = MmFree(obj_data);
    result = MmFree(obj);
    KernelReg.object_count--;
    return result;
}

/************************************************/

static U16_t CoreCalculatePrescaler(U16_t f_os)
{

    volatile U8_t presc_cnt = 1;
    volatile U16_t ovf;
    volatile float tmp_ovf = 0xFFEE;
    KernelReg.f_cpu = PRTOS_CONFIG_F_CPU_HZ;
    float tick_t = (1/(float)f_os); //in s
    KernelReg.f_os = f_os;

    volatile U16_t prescaling_fact;

    while (presc_cnt < 5 || tmp_ovf > 0xFFFF) {

        if (presc_cnt == 1) {
            prescaling_fact = 1;
            KernelReg.prescaler = 1;
            } else if (presc_cnt == 2) {
            prescaling_fact = 8;
            KernelReg.prescaler =8;
            } else if (presc_cnt == 3) {
            prescaling_fact = 64;
            KernelReg.prescaler = 65;
            } else if (presc_cnt == 4) {
            prescaling_fact = 256;
            KernelReg.prescaler = 256;
            } else if (presc_cnt == 5) {
            prescaling_fact = 1024;
            KernelReg.prescaler = 1024;
        }

        KernelReg.f_os_timer = PRTOS_CONFIG_F_OS_TIMER_HZ/prescaling_fact; //in Hz
        tmp_ovf = (tick_t * KernelReg.f_os_timer);

        if(tmp_ovf > 0xFFFF) {
            presc_cnt++;
            } else {
            break;
        }
    }

    ovf = (U16_t)(floor(tmp_ovf));
    KernelReg.tick_period_us = (U32_t)(tick_t * (U32_t)1000000);
    KernelReg.ovf = ovf;
    return ovf;
}

/************OS Watchdog Timer************/

#if PRTOS_CONFIG_ENABLE_WDT==1

static void CoreWdtInit(void)
{
    PortWdtInit(0, 5);
}

static void CoreWdtEnable(U8_t wdt_expire_opt)
{
    PortWdtEnable(wdt_expire_opt);
}

static void CoreWdtDisable(void)
{
    PortWdtDisable();
}

static void CoreWdtKick(void)
{
    PortWdtKick();
}

void CoreWdtIsr(void)
{
    OsIsrBegin();
    OsCritSectBegin();
    CoreWdtDisable();
    LOG_ERROR_NEWLINE("WatchdogTimer overflow.");
    while(1);
    //CoreFlagSet(dispatch_flag);
    CoreWdtKick();

    OsCritSectEnd();
    OsIsrEnd();
}


#endif

/*************************************/












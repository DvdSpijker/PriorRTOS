#include "main.h"
#include "PriorRTOS.h"

LOG_FILE_NAME("main.c")

#define BLINK_LED 0x01
#define BLINK_INTERVAL_MS 1000
#define BLINK_INTERVAL_STEP_MS 100   
#define READ_SWITCH_INTERVAL_MS 30
#define EVG_FLAG_SWITCH_EDGE    EVENTGROUP_FLAG_MASK_0

void Composer(const void *p_arg, U32_t v_arg);
void Blink(const void *p_arg, U32_t v_arg);
void ReadSwitches(const void *p_arg, U32_t v_arg);

int main(int argc, char** argv) {

    OS_ARG_UNUSED(argc);
    OS_ARG_UNUSED(argv);
    
    OsResult_t k_res, m_res;

	k_res = OsInit(&m_res); /* Initialize OS. */

	char k_res_str[CONVERT_BUFFER_SIZE_RESULT_TO_STRING];
	char m_res_str[CONVERT_BUFFER_SIZE_RESULT_TO_STRING];
	ConvertResultToString(k_res, k_res_str);
	ConvertResultToString(m_res, m_res_str);	
    LOG_DEBUG_NEWLINE("Kernel init result: %s | Modules init result: %s", k_res_str, m_res_str);

	/* Create the Composer task. */
    Id_t tid_composer = TaskCreate(Composer, TASK_CAT_HIGH, 5, TASK_PARAMETER_NONE, 0, NULL, 0);
	if(tid_composer != ID_INVALID) {
        OsStart(tid_composer); /* Start OS scheduler. This function does not return. */
	} else {
        LOG_ERROR_NEWLINE("Failed to create Composer task.\n");
    }
	
	/* Code below is only executed when OsStop is called or if an critical error occurred. */
	LOG_DEBUG_NEWLINE("OS Exit status:");

	while(1);

    return (EXIT_SUCCESS);
}

/* The Composer task creates tasks and other resources. 
 * Each created task is injected with required resources.
 * The Composer is deleted after this process. */
void Composer(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(p_arg);
    OS_ARG_UNUSED(v_arg);
    
    Id_t tids[2] = {ID_INVALID};
    static IdList_t list;
    static Id_t evg_switches = ID_INVALID;
    
    /* Initialize GPIO (buttons, leds, switches etc.) */
    GPIO_INIT(0b01110, 0xFF, 0x00, ONN, 0x0100);
    
    /* Initialize the ID list and add the created timer. */
    IdListInit(&list);
    IdListIdAdd(&list, TimerCreate(ConvertMsToUs(READ_SWITCH_INTERVAL_MS), (TIMER_PARAMETER_PERIODIC | TIMER_PARAMETER_AR), NULL, NULL));
    
    evg_switches = EventgroupCreate(); /* Create the switches eventgroup. */
    if(evg_switches != ID_INVALID) {
        /* Add the eventgroup to the ID list, create the Blink and ReadSwitches tasks. */
        IdListIdAdd(&list, evg_switches);
        tids[0] = TaskCreate(Blink, TASK_CAT_LOW, 5, TASK_PARAMETER_NONE, 0, (void *)&evg_switches, BLINK_INTERVAL_MS);
        tids[1] = TaskCreate(ReadSwitches, TASK_CAT_REALTIME, 1, TASK_PARAMETER_NONE, 0, (void *)&list, 0);
        
        /* If the tasks were created successfully, resume both tasks.
         * This is needed because the tasks do not implicity execute if
         * TASK_PARMATER_START is not passed. */
        if(tids[0] != ID_INVALID && tids[1] != ID_INVALID) {
            TaskResume(tids[0]);
            TaskResume(tids[1]);
        } else {
            LOG_ERROR_NEWLINE("Failed to create system tasks.");
        }
    } else {
        LOG_ERROR_NEWLINE("Failed to create shared eventgroup.");
    }
    
    TaskDelete(NULL); /* Delete self. */
}

/* The Blink tasks blinks the BLINK_LED at an interval between BLINK_INTERVAL_MS and
 * 0 milliseconds. The interval is decreased when the flag in the switches eventgroup is
 * set. */
void Blink(const void *p_arg, U32_t v_arg)
{
    static Id_t evg_switches = ID_INVALID;
	static U32_t blink_interval = 0;
    static U32_t base_interval = 0;
    static char led_state = 0x00;
    
	TASK_INIT_BEGIN() {
		base_interval = v_arg;
        blink_interval = base_interval;
        evg_switches = *((Id_t *)p_arg);
	} TASK_INIT_END();

	/* Toggle the LED. */
    led_state ^= (1 << BLINK_LED);
    Update_LEDs(led_state);
    
    /* If the switch 0 edge flag in the shared eventgroup is set,
     * decrease the interval if able, otherwise reset it. */
    if(EventgroupFlagsGet(evg_switches, EVG_FLAG_SWITCH_EDGE)) {
        if(blink_interval >= BLINK_INTERVAL_STEP_MS) {
            blink_interval -= BLINK_INTERVAL_STEP_MS;
        } else {
            blink_interval = base_interval;
        }
        EventgroupFlagsClear(evg_switches, EVG_FLAG_SWITCH_EDGE);
        LOG_DEBUG_NEWLINE("[BLINK] Changed interval to: %u ms", blink_interval);
    }
       
	TaskSleep(blink_interval);
}

/* The ReadSwitches task checks if any rising edges were detected generated by the 
 * switches. The corresponding flag is set in the switches eventgroup.
 * This task is triggered by a timer. */
void ReadSwitches(const void *p_arg, U32_t v_arg)
{
    OS_ARG_UNUSED(v_arg);
    
    static Id_t evg_switches = ID_INVALID;
    static Id_t tmr_read_trigger = ID_INVALID;
    
    TASK_INIT_BEGIN() {
        /* Acquire the ID list and remove its contents. */
        IdList_t * list = (IdList_t *)p_arg;
        evg_switches = IdListIdRemove(list);
        tmr_read_trigger = IdListIdRemove(list);
        
        /* Add a permanent poll for the trigger timer overflow event and
         * start the timer. */
        TaskPollAdd(tmr_read_trigger, TIMER_EVENT_OVERFLOW, OS_TIMEOUT_INFINITE);
        TimerStart(tmr_read_trigger); /* Start the trigger timer. */
	} TASK_INIT_END();
    
    /* If an overflow event was received, check if there were any rising edges detected. */
    if(TaskPoll(tmr_read_trigger, TIMER_EVENT_OVERFLOW, OS_TIMEOUT_INFINITE, false) == OS_RES_EVENT) {
        if(RisingEdge_SW0()) {
            /* Set the appropriate flags in the switches eventgroup. */
            EventgroupFlagsSet(evg_switches, EVG_FLAG_SWITCH_EDGE);
            LOG_DEBUG_NEWLINE("[RDSW] Detected rising edge");
        }
        TimerStart(tmr_read_trigger); /* Restart the trigger timer. */
    }    
    
    /* Nothing to do, suspend self. */
    TaskSuspendSelf();
}
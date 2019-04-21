
#include "kernel/inc/Scheduler.h"

#include "include/Logger.h"
#include "include/Os.h"
#include "kernel/inc/Event.h"
#include "kernel/inc/List.h"

#include <stdlib.h>

struct EventBrokerArgs {
    LinkedList_t *task_list;
    pTcb_t task;
    pEvent_t compare_event;
    bool last_event;
    U32_t delta_us;
};

static OsResult_t ISchedulerEventBrokerCycle(LinkedList_t *tcb_lists[], U8_t num_lists);
static void ISchedulerTaskListCompare(struct EventBrokerArgs *args);
static void ISchedulerTaskEventsCompare(struct EventBrokerArgs *args);
void ISchedulerTaskAddDescendingPriority(LinkedList_t *from_list, LinkedList_t *to_list, pTcb_t task);

static LinkedList_t ExecutionQueue; /* Holds all the scheduled tasks. */

static LinkedList_t *EventList;

/*********  Scheduler functions *********/

OsResult_t KSchedulerInit(LinkedList_t *event_list)
{
	if(event_list == NULL) {
		return OS_RES_INVALID_ARGUMENT;
	}

	OsResult_t result = OS_RES_ERROR;

	result = ListInit(&ExecutionQueue, ID_GROUP_TASK);
    //LOG_INFO_NEWLINE("ExecutionQueue: %p", &ExecutionQueue);
    /* Add a mock event to the EventList.
    * This ensures that this list will never
    * be empty. An empty list has to be avoided
    * to keep the EventHandle cycle running, since
    * it also needs to update registered event
    * lifetimes. */
	if(result == OS_RES_OK) {
		result = EventEmit(ID_INVALID, MOCK_EVENT, EVENT_FLAG_NONE);
		if(result != OS_RES_OK) {
			LOG_ERROR_NEWLINE("Failed to publish the mock event!");
		}
		EventList = event_list;
	}

    return result;
}

OsResult_t KSchedulerCycle(LinkedList_t *tcb_lists[], U8_t num_lists)
{
	if(tcb_lists == NULL || num_lists == 0) {
		return OS_RES_INVALID_ARGUMENT;
	}

    return ISchedulerEventBrokerCycle(tcb_lists, num_lists);
}

/* Loads a task from the ExecutionQueue (head position).
 * If no task is present in the queue,
* the Idle task is loaded instead. */
pTcb_t KSchedulerQueueTaskPop(void)
{
	pTcb_t tcb = NULL;

	if (ListSizeGet(&ExecutionQueue) != 0) {
		tcb = (pTcb_t)ListNodeChildGet(ListNodeRemoveFromHead(&ExecutionQueue));
	}

    return tcb;
}

U32_t KSchedulerQueueSizeGet(void)
{
	return ListSizeGet(&ExecutionQueue);
}

/* Compares all tasks with all occurred events. All lifetimes are updated, expired events
* will be deleted.
* All activated tasks will be placed in the activated task list. */
static OsResult_t ISchedulerEventBrokerCycle(LinkedList_t *tcb_lists[], U8_t num_lists)
{
    if(EventList->size == 0) {
        LOG_ERROR_NEWLINE("EventList is empty. A Mock event must always be present.");
     	return OS_RES_CRIT_ERROR;
    }

    OsResult_t result = OS_RES_OK;
    struct ListIterator it;
    pEvent_t occurred_event = NULL;
    /* Loop through occurred event list and compare all activated tasks and
    * all waiting tasks with every occurred event. */
    bool last_event = false;
    struct EventBrokerArgs args;

    static U32_t micros = 0;
    U32_t delta_us = OsRunTimeMicrosDelta(micros);
    micros = OsRunTimeMicrosGet();
    args.delta_us = delta_us;

    LIST_ITERATOR_BEGIN(&it, EventList, LIST_ITERATOR_DIRECTION_FORWARD);
    {
        if(it.current_node != NULL) {

            occurred_event = (pEvent_t)ListNodeChildGet(it.current_node);

            // LOG_DEBUG_APPEND("\nComparing event %04x with:", it.current_node->id);
            if(it.next_node == NULL) {
                last_event = true;
            }

            /* Handle addressed events (not necessarily listened). */
            if(EventFlagGet(occurred_event, EVENT_FLAG_ADDRESSED)) {
                /* For addressed events, the source_id field describes the DESTINATION ID instead of the SOURCE ID. */
                pTcb_t tcb = KTcbFromId(occurred_event->source_id);
                if(tcb != NULL) {
                    LinkedList_t *list = KTcbLocationGet(tcb);
                    if(list != NULL && list != &ExecutionQueue) {
                        KTaskStateSet(tcb, TASK_STATE_ACTIVE);
                        ISchedulerTaskAddDescendingPriority(list, &ExecutionQueue, tcb);
                    }
                }
                if(!last_event) {
                    goto event_cleanup;
                }
            }

            args.compare_event = occurred_event;
            args.last_event = last_event;

            for(U8_t i = 0; i < num_lists; i++) {
            	  args.task_list = tcb_lists[i];
            	  ISchedulerTaskListCompare(&args);
            }

      	  args.task_list = &ExecutionQueue;
      	  ISchedulerTaskListCompare(&args);

event_cleanup:
            /* Increment the occurred event's lifetime and check if it has time left. If this is
            * not the case the event is destroyed. EventDestroy automatically removes the
            * event from its list before deleting it. */
            if(!EventIsMock(occurred_event)) {
                if( (EventLifeTimeIncrement(occurred_event, delta_us) == -1) ) {
                    //LOG_DEBUG_NEWLINE("Destroying occurred event %p.", occurred_event);
                    EventDestroy(NULL, occurred_event);
                }
            }
        } else {
            LOG_ERROR_NEWLINE("EventList size (%u) is not consistent with the number of reachable nodes", EventListSizeGet(EventList));
            result = OS_RES_CRIT_ERROR;
            break;
        }
    }
    LIST_ITERATOR_END(&it);

    return result;
}



/* Compares all tasks and their listened events in the Task List with the compare event. All activated tasks
* will be moved to the activated task list. */
static void ISchedulerTaskListCompare(struct EventBrokerArgs *args)
{

    pTcb_t task = NULL;
    struct ListIterator it;

    /* Loop through the Task List and compare the occurred event with all listened events for
    * every task. */
    LIST_ITERATOR_BEGIN(&it, args->task_list, LIST_ITERATOR_DIRECTION_FORWARD);
    {
        if(it.current_node != NULL) {
            task = (pTcb_t)ListNodeChildGet(it.current_node);
            //LOG_DEBUG_APPEND("\n\tEvent list of task %04x", it.current_node->id);
            args->task = task;
            ISchedulerTaskEventsCompare(args);
        } else {
            break;
        }
    }
    LIST_ITERATOR_END(&it);

}

/* Compares the Task Event List with the compare event, if the compared event appears in the list the
* task is moved to the activated task list. */
static void ISchedulerTaskEventsCompare(struct EventBrokerArgs *args)
{
    LinkedList_t *task_list = args->task_list;
    pTcb_t task = args->task;
    pEvent_t compare_event = args->compare_event;
    bool last_event = args->last_event;
    LinkedList_t *task_event_list = &task->event_list;

    pEvent_t task_event = NULL;
    struct ListIterator it;

    /* Loop through Task Event List and compare. */
    LIST_ITERATOR_BEGIN(&it, task_event_list, LIST_ITERATOR_DIRECTION_FORWARD) {
        if(it.current_node != NULL) {
            task_event = (pEvent_t)ListNodeChildGet(it.current_node);
            //LOG_DEBUG_APPEND("\n\t\tComparing task event %04x.", it.current_node->id);
            /* If the occurred event matches the listened event,
            * set occurred flag and make task active. Then increment
            * its occurrence counter and reset its lifetime. */
            if (EventIsEqual(compare_event, task_event)) { /* Handle listened events. */
                EventFlagSet(task_event, EVENT_FLAG_OCCURRED);
                EventFlagClear(task_event, EVENT_FLAG_TIMED_OUT); /* Clear timed-out flag in case it was set during a previous cycle. */
                EventLifeTimeReset(task_event);
                EventOccurrenceCountIncrement(task_event);
            } else if(last_event) { /* If the listened event does not match the occurred event. */
                /* Check if the event has a specified time-out, if this is true
                * increment its lifetime. If the event is timed-out, set timed-out flag. */
                if((!EventFlagGet(task_event, EVENT_FLAG_NO_TIMEOUT)) && (!EventFlagGet(task_event, EVENT_FLAG_TIMED_OUT))) {
                    /// LOG_DEBUG_NEWLINE("Updating lifetime of event %04x of task %04x", task_event->list_node.id, task->list_node.id);
                    if(EventLifeTimeIncrement(task_event, args->delta_us) == -1) {
                        EventFlagSet(task_event, EVENT_FLAG_TIMED_OUT);
                        //LOG_DEBUG_NEWLINE("Event (%04x) of task %04x timed out", task_event->list_node.id, task->list_node.id);
                    }
                }
            }

            if( EventFlagGet(task_event, EVENT_FLAG_OCCURRED) || EventFlagGet(task_event, EVENT_FLAG_TIMED_OUT) ) {
                KTaskStateSet(task, TASK_STATE_ACTIVE);
            }
        } else {
            break;
        }
    }
    LIST_ITERATOR_END(&it);

    if((task_list != &ExecutionQueue) && (task->state == TASK_STATE_ACTIVE)) {
        ISchedulerTaskAddDescendingPriority(task_list, &ExecutionQueue, task);
    }
}


void ISchedulerTaskAddDescendingPriority(LinkedList_t *from_list, LinkedList_t *to_list, pTcb_t task)
{
    pTcb_t compare_task = NULL;
    struct ListIterator it;

    if(ListNodeRemove(from_list, &task->list_node) == NULL) {
        LOG_ERROR_NEWLINE("Removing task %08x from list %p failed.", ListNodeIdGet(&task->list_node), from_list);
        while(1);
    }

    /* Add task to list head if the list is empty.
    * Else loop through list and compare priorities. */
    if(ListSizeGet(to_list) == 0) {
        //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x at head.", task->list_node.id);
        if(ListNodeAddAtPosition(to_list, &task->list_node, LIST_POSITION_HEAD) != OS_RES_OK) {
        	LOG_ERROR_NEWLINE("Adding task %08x to list %p failed.", ListNodeIdGet(&task->list_node), to_list);
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
                    if(ListNodeAddAtNode(to_list, &task->list_node, it.current_node, LIST_ADD_BEFORE) != OS_RES_OK) {
                    	LOG_ERROR_NEWLINE("Adding task %08x to list %p failed.", ListNodeIdGet(&task->list_node), to_list);
                        while(1);
                    }
                    return;
                }
            } else {
                //LOG_DEBUG_APPEND("\n\t\t\tAdding task %04x at tail.", task->list_node.id);
                if(ListNodeAddAtPosition(to_list, &task->list_node, LIST_POSITION_TAIL) != OS_RES_OK) {
                	LOG_ERROR_NEWLINE("Adding task %08x to list %p failed.", ListNodeIdGet(&task->list_node), to_list);
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

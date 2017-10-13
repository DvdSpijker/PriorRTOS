/*
 * Prior_event.c
 *
 * Created: 09-Dec-16 19:16:54
 *  Author: User
 */

#include <PriorRTOSConfig.h>
#include <Logger.h>
#include <LoggerDef.h>
#include <Types.h>
#include <Event.h>
#include <Fletcher.h>

#include <stdlib.h>
#include <stdio.h>

LOG_FILE_NAME("Event.c");

U32_t PublishedEventLifetimeUs;

extern U32_t OsTickPeriodGet(void);

/* TODO: Events will have to be stored in their lists in a FIFO manner.
 * This means: Newly published / subscribed events are pushed to the back.
 * Refreshed events will be pushed to the front.
 * Subscribed events that have occurred will be at the front of the list,
 * not-occurred events will remain at the bottom.
 * AddSorted operations will have to be replaced. */

pEvent_t EventCreate(Id_t source_id, U32_t event_code, U32_t life_time_us);
U16_t EventToHash(Id_t source_id, U32_t code);
void EventLifeTimeReset(pEvent_t event);

OsResult_t EventInit(void)
{
    ListInit(&EventList, 0, NULL, 0);
    //LOG_DEBUG_NEWLINE("EventList: %p", &EventList);
    PublishedEventLifetimeUs = OsTickPeriodGet() * PRTOS_CONFIG_EVENT_LIFE_TIME_TICKS;

    return OS_OK;
}

pEvent_t EventCreate(Id_t source_id, U32_t event_code, U32_t life_time_us)
{
    pEvent_t new_event = NULL;

    new_event = (pEvent_t)CoreObjectAlloc(sizeof(Event_t), 0, NULL);//malloc(sizeof(Event_t));
    if(new_event == NULL) {
        return NULL;
    }
    new_event->life_time_us = life_time_us;
    new_event->life_time_us_cnt = 0;
    new_event->occurrence_cnt = 0;
    new_event->source_id = source_id;
    new_event->event_code = event_code;

    ListNodeInit(&new_event->list_node, (void*)new_event);

    return new_event;
}

OsResult_t  EventAddToList(LinkedList_t *event_list, pEvent_t event)
{
    OsResult_t result = OS_OK;
    if(event_list == NULL || event == NULL) {
        result = OS_NULL_POINTER;
    }
    if(result == OS_OK) {
        result = ListNodeAddAtPosition(event_list, &event->list_node, LIST_POSITION_TAIL);
    }
    return result;
}

OsResult_t EventSubscribe(LinkedList_t *task_event_list, Id_t object_id, U32_t event_code, U8_t flags, U32_t life_time_us, Id_t *out_event_id)
{
    U16_t hash = EventToHash(object_id, event_code);
    ListNode_t *node = ListSearch(task_event_list, hash);
    pEvent_t new_event = NULL;
    if(node != NULL) {
        new_event = (pEvent_t)ListNodeChildGet(node);
        if(!(EVENT_FLAG_GET(new_event->event_code, EVENT_FLAG_NO_TIMEOUT))) {
            EventLifeTimeReset(new_event);
        }
        return OS_FAIL;
    }

    U32_t new_event_code = event_code;
    if(life_time_us == 0) {
        flags |= EVENT_FLAG_NO_TIMEOUT;
    }
    EVENT_FLAG_SET(new_event_code, flags);

    new_event = EventCreate(object_id, new_event_code, life_time_us);
    if(new_event != NULL) {
        new_event->list_node.id = hash;
        OsResult_t result = ListNodeAddAtPosition(task_event_list, &new_event->list_node, LIST_POSITION_TAIL);
        if(result == OS_OK) {
            *out_event_id = ListNodeIdGet(&new_event->list_node);
        } else {
            CoreObjectFree((void **)&new_event, NULL);
            *out_event_id = INVALID_ID;
        }
        return result;
    }
    LOG_ERROR_NEWLINE("Subscribing to event from object %04x failed.");
    return OS_ERROR;
}


OsResult_t EventPublish(Id_t source_id, U32_t event_code, U8_t flags)
{
    U16_t hash = EventToHash(source_id, event_code);
    ListNode_t *node = ListSearch(&EventList, hash);
    if(node != NULL) {
        EventLifeTimeReset((pEvent_t)ListNodeChildGet(node));
        return OS_FAIL;
    }

    U32_t new_event_code = event_code;
    EVENT_FLAG_SET(new_event_code, flags);

    pEvent_t new_event = EventCreate(source_id, new_event_code, PublishedEventLifetimeUs);
    if(new_event != NULL) {
        new_event->list_node.id = hash;
        /* Published events are stored in the EventList in a FIFO manner. */
        if(ListNodeAddAtPosition(&EventList, &new_event->list_node, LIST_POSITION_TAIL) == OS_OK) {
            //LOG_EVENT(new_event);
            return OS_OK;
        } else {
            CoreObjectFree((void **)&new_event, NULL);
        }

    }
    LOG_ERROR_NEWLINE("Publishing event for source %04x failed.");
    return OS_ERROR;
}

pEvent_t EventPeekHead(LinkedList_t *list)
{
    if(EventList.size == 0) {
        return NULL;
    }

    ListNode_t *pk_node = NULL;
    pEvent_t pk_event = NULL;

    pk_node = ListNodePeek(list, LIST_POSITION_HEAD);
    if(pk_node == NULL) {
        return NULL;
    }
    pk_event = (pEvent_t)ListNodeChildGet(pk_node);

    return pk_event;
}

pEvent_t EventPeekNext(pEvent_t event)
{
    if(event != NULL) {
        ListNode_t *next_node = ListNodePeekNext(&event->list_node);
        if(next_node != NULL) {
            return ((pEvent_t)ListNodeChildGet(next_node));
        }
    }
    return NULL;
}
pEvent_t EventPeekPrev(pEvent_t event)
{
    if(event != NULL) {
        ListNode_t *prev_node = ListNodePeekPrev(&event->list_node);
        if(prev_node != NULL) {
            return ((pEvent_t)ListNodeChildGet(prev_node));
        }
    }
    return NULL;
}

S8_t EventOccurrenceCountIncrement(pEvent_t event)
{
    S8_t status = 0;

    if(event->occurrence_cnt < 255) {
        event->occurrence_cnt++;
    } else {
        status = -1;
    }

    return status;
}

void EventOccurrenceCountReset(pEvent_t event)
{
    event->occurrence_cnt = 0;
}

S8_t EventLifeTimeIncrement(pEvent_t event, U32_t t_us)
{
    S8_t status = 0;

    if((event->life_time_us_cnt + t_us) < event->life_time_us) {
        event->life_time_us_cnt += t_us;
    } else {
        status = -1;
    }

    return status;
}

void EventLifeTimeReset(pEvent_t event)
{
    event->life_time_us_cnt = 0;
}

pEvent_t EventHandleFromId(LinkedList_t *list, Id_t event_id)
{
    ListNode_t *rm_node = ListSearch(list, event_id);
    pEvent_t rm_event = NULL;

    if(rm_node != NULL) {
        rm_event = (pEvent_t)ListNodeChildGet(rm_node);
        if((EVENT_FLAG_GET(rm_event->event_code, EVENT_FLAG_OCCURRED)) || (EVENT_FLAG_GET(rm_event->event_code, EVENT_FLAG_TIMED_OUT))) {
            ListNodeRemove(list, rm_node);
            return rm_event;
        }
    }
    return NULL;

}

pEvent_t EventHandleFifo(LinkedList_t *list)
{
    if(list == NULL) {
        return NULL;
    }
    ListNode_t *rm_node = NULL;
    ListNode_t *next_rm_node = NULL;
    ListSize_t size = list->size;
    pEvent_t rm_event = NULL;
    rm_node = ListNodePeek(list, LIST_POSITION_HEAD);
    for(ListSize_t i = 0; i < size; i++) {
        if(rm_node != NULL) {
            next_rm_node = ListNodePeekNext(rm_node);
            rm_event = (pEvent_t)ListNodeChildGet(rm_node);
            if((EVENT_FLAG_GET(rm_event->event_code, EVENT_FLAG_OCCURRED)) || (EVENT_FLAG_GET(rm_event->event_code, EVENT_FLAG_TIMED_OUT))) {
                ListNodeRemove(list, rm_node);
                return rm_event;
            }
        } else {
            break;
        }
        rm_node = next_rm_node;
    }

    return NULL;
}

OsResult_t EventListDestroy(LinkedList_t *list)
{
    if(list == NULL) {
        return OS_NULL_POINTER;
    } else if(list == &EventList) {
        return OS_RESTRICTED;
    }

    OsResult_t result = OS_OK;

    if(list->size > 0) {
        ListNode_t *node = list->head;
        do {
            result = EventDestroy(list, (pEvent_t)ListNodeChildGet(node));
            node = node->next_node;
        } while (node != NULL);
    }

    return result;

}

OsResult_t EventDestroy(LinkedList_t *list, pEvent_t event)
{

    if(event == NULL) {
        return OS_NULL_POINTER;
    }
    if(list == NULL) {
        list = &EventList;
    }

    if(ListNodeIsInList(list, &event->list_node)) {
        ListNodeRemove(list, &event->list_node);
    }
    CoreObjectFree((void **)&event, NULL);
    //free(event);
    return OS_OK;
}

ListSize_t EventListSizeGet(LinkedList_t *event_list)
{
    return (event_list == NULL ? 0 : event_list->size);
}

OsResult_t  EventFlagSet(pEvent_t event, U8_t event_flag)
{
    if(event == NULL) {
        return OS_NULL_POINTER;
    }

    EVENT_FLAG_SET(event->event_code, event_flag);

    return OS_OK;
}


OsResult_t  EventFlagClear(pEvent_t event, U8_t event_flag)
{
    if(event == NULL) {
        return OS_NULL_POINTER;
    }

    EVENT_FLAG_CLEAR(event->event_code, event_flag);

    return OS_OK;
}

U8_t EventFlagGet(pEvent_t event, U8_t event_flag)
{

    return ((EVENT_FLAG_GET(event->event_code, event_flag)) ? 1 : 0);
}


U16_t EventToHash(Id_t source_id, U32_t code)
{
    U64_t event_concat = 0 | ((U64_t)source_id << 32) | code;
    U16_t hash = FletcherGenerate((U8_t *)&event_concat, sizeof(U64_t));

    return (hash);
}


bool EventIsEqual(pEvent_t event_x, pEvent_t event_y)
{
    return (event_x->source_id == event_y->source_id &&
            (event_x->event_code & 0x00FFFFFF) == (event_y->event_code & 0x00FFFFFF));
}


bool EventIsMock(pEvent_t event)
{
    return(event->source_id == INVALID_ID && event->event_code == MOCK_EVENT);
}

bool EventHasOccurred(pEvent_t event)
{
    return(EVENT_FLAG_GET(event->event_code, EVENT_FLAG_OCCURRED));
}

bool EventIsType(pEvent_t event, U32_t event_type)
{
    return(EVENT_TYPE_GET(event->event_code) == EVENT_TYPE_GET(event_type));
}

bool EventHasTimedOut(pEvent_t event)
{
    return(EVENT_FLAG_GET(event->event_code, EVENT_FLAG_TIMED_OUT));
}

pEvent_t EventFromId(LinkedList_t *list, Id_t event_id)
{
    ListNode_t *node = ListSearch(list, event_id);
    pEvent_t event = NULL;
    if(node != NULL) {
        event = (pEvent_t)ListNodeChildGet(node);
    }
    return event;
}

void EventListPrint(LinkedList_t *list)
{
    ListSize_t i = 0;
    ListNode_t *node = NULL;
    pEvent_t event = NULL;

    if(ListLock(list, LIST_LOCK_MODE_READ) != OS_OK) {
        return;
    }

    node = ListNodePeek(list, LIST_POSITION_HEAD);
    if(node != NULL) {
        printf("\n--- Event list %p, size %u ---\n [index][source id][event code]\n", list, list->size);
        event = (pEvent_t)ListNodeChildGet(node);
        for (; i < list->size; i++) {

            printf("[%u][0x%04x][0x%08x]\n", i, event->source_id, (unsigned int)event->event_code);
            node = ListNodePeekNext(node);
            if(node == NULL) {
                break;
            }
            event = (pEvent_t)ListNodeChildGet(node);
        }
        printf("----------------\n");

    }

    ListUnlock(list, LIST_LOCK_MODE_READ);


}
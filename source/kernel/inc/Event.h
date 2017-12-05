/******************************************************************************************************************************************
 *  File: Event.h
 *  Description: Event API.

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


#ifndef EVENT_H_
#define EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <List.h>
#include <EventDef.h>

/* Private Event flags. */
#define     EVENT_FLAG_NONE          0x00
#define     EVENT_FLAG_NO_HANDLER    0x80
#define     EVENT_FLAG_PERMANENT     0x40
#define     EVENT_FLAG_NO_TIMEOUT    0x20
#define     EVENT_FLAG_ADDRESSED     0x10
#define     EVENT_FLAG_TIMED_OUT     0x08
#define     EVENT_FLAG_OCCURRED      0x04


/* Extraction macro-expansions. */
#define    EVENT_COMPOSE(type, specifier, flags) (U32_t)( (24 << ((U32_t)flags & 0xFF000000)) | (16 << ((U32_t)type & 0x00FF0000)) | ((U32_t)specifier & 0x0000FFFF)))
#define    EVENT_TYPE_GET(evt)  (U8_t)((evt & 0x00FF0000) >> 16)
#define    EVENT_SPECIFIER_GET(evt) (U16_t)(evt & 0x0000FFFF);
#define    EVENT_FLAG_GET(evt, flag) ((U8_t)((evt & 0xFF000000) >> 24) & flag)
#define    EVENT_FLAG_SET(evt, flag) (evt |= ( ( ((U32_t)flag) << 24 ) & 0xFF000000) )
#define    EVENT_FLAG_CLEAR(evt, flag) (evt &= ~( ( ((U32_t)flag) << 24 ) & 0xFF000000) )

#define    MOCK_EVENT (U32_t)(EVENT_TYPE_CREATE | 0x0000FEEF)

/* Event_t definition */
typedef struct Event_t {
    ListNode_t   list_node;

    U32_t   life_time_us;
    U32_t   life_time_us_cnt;
    U8_t    occurrence_cnt;
    Id_t    source_id;
    U32_t   event_code;
} Event_t;

typedef struct Event_t * pEvent_t;

LinkedList_t EventList;

/* Event API. */
OsResult_t  EventInit(void);
OsResult_t  EventDestroy(LinkedList_t *list, pEvent_t event);
OsResult_t  EventListDestroy(LinkedList_t *list);
ListSize_t  EventListSizeGet(LinkedList_t *event_list);
pEvent_t    EventListContainsEvent(LinkedList_t *event_list, Id_t source_id, U32_t event_code);

OsResult_t  EventAddToList(LinkedList_t *event_list, pEvent_t event);
OsResult_t  EventListen(LinkedList_t *task_event_list, Id_t object_id, U32_t event_code, U8_t flags, U32_t life_time_us, Id_t *out_event_id);
OsResult_t  EventEmit(Id_t source_id, U32_t event_code, U8_t flags);

S8_t        EventLifeTimeIncrement(pEvent_t event, U32_t t_us);
void        EventLifeTimeReset(pEvent_t event);
S8_t        EventOccurrenceCountIncrement(pEvent_t event);
void        EventOccurrenceCountReset(pEvent_t event);

void        EventHandle(pEvent_t event);
pEvent_t    EventHandleFromId(LinkedList_t *list, Id_t event_id);
pEvent_t    EventHandleFifo(LinkedList_t *list);
pEvent_t    EventFromId(LinkedList_t *list, Id_t event_id);

pEvent_t    EventPeekHead(LinkedList_t *list);
pEvent_t    EventPeekNext(pEvent_t event);
pEvent_t    EventPeekPrev(pEvent_t event);

OsResult_t  EventFlagSet(pEvent_t event, U8_t event_flag);
OsResult_t  EventFlagClear(pEvent_t event, U8_t event_flag);
U8_t        EventFlagGet(pEvent_t event, U8_t event_flag);

bool        EventIsEqual(pEvent_t event_x, pEvent_t event_y);
bool        EventIsMock(pEvent_t event);
bool        EventIsType(pEvent_t event, U32_t event_type);
bool        EventHasOccurred(pEvent_t event);
bool        EventHasTimedOut(pEvent_t event);


void EventListPrint(LinkedList_t *list);


#ifdef __cplusplus
}
#endif
#endif /* EVENT_H_ */
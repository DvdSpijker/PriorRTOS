
/******************************************************************************************************************************************
 *  File: Prior_eventgroup.c
 *  Description: Prior Eventgroup module

 *  OS Version: V0.4
 *  Date: 13/03/2015
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


#include <Eventgroup.h>
#include <EventgroupDef.h>
#include <CoreDef.h>
#include <Event.h>
#include <List.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

#define EVENTGROUP_ID_BUFFER_SIZE 3
Id_t EventgroupIdBuffer[EVENTGROUP_ID_BUFFER_SIZE];

OsResult_t EventgroupInit (void)
{
    ListInit(&EventGroupList, ID_TYPE_EVENTGROUP, EventgroupIdBuffer, EVENTGROUP_ID_BUFFER_SIZE);
    return OS_OK;
}

/*Event Group user API*/

Id_t EventgroupCreate(void)
{
    //pEventGrp_t new_eventgrp = (pEventGrp_t)malloc(sizeof(EventGrp_t));
    pEventGrp_t new_eventgrp = (pEventGrp_t)CoreObjectAlloc(sizeof(EventGrp_t), 0, NULL);

    if(new_eventgrp == NULL) {
        return INVALID_ID;
    }

    ListNodeInit(&new_eventgrp->list_node, (void*)new_eventgrp);
    if(ListNodeAddSorted(&EventGroupList, &new_eventgrp->list_node) != OS_OK) {
        CoreObjectFree((void **)&new_eventgrp, NULL);
        return INVALID_ID;
    }

    new_eventgrp->event_reg = 0;

    return new_eventgrp->list_node.id;
}

OsResult_t EventgroupDelete(Id_t *eventgroup_id)
{
    if(*eventgroup_id == INVALID_ID) {
        return OS_INVALID_ID;
    }
    if((*eventgroup_id & ID_TYPE_EVENTGROUP) == 0) {
        return OS_INVALID_ID;
    }

    if(EventGroupList.lock != 0) {
        return OS_LOCKED;
    }


    ListNode_t *node = ListSearch(&EventGroupList, *eventgroup_id);
    void *eventgroup = ListNodeChildGet(node);
    ListNodeDeinit(&EventGroupList, node);
    CoreObjectFree((void **)&eventgroup, NULL);

    *eventgroup_id = INVALID_ID;

    return OS_OK;
}

void EventgroupFlagsSet(Id_t eventgroup_id, U8_t mask)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&EventGroupList, eventgroup_id) {
        pEventGrp_t eventgroup = (pEventGrp_t)ListNodeChildGet(node);
        if(eventgroup != NULL) {
            eventgroup->event_reg |= mask;

#ifdef PRTOS_CONFIG_USE_EVENT_EVENTGROUP_FLAG_SET
            U8_t bitmask = 0x80;
            for (S8_t i = 8; i > 0; i--) {
                if(bitmask & mask) {
                    EventPublish(eventgroup_id, EVENTGROUP_EVENT_FLAG_SET(mask), 0);
                }
                bitmask = bitmask >> 1;
            }
#endif
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
}

OsResult_t EventgroupFlagsClear(Id_t eventgroup_id, U8_t mask)
{
    OsResult_t result = OS_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&EventGroupList, eventgroup_id) {
        pEventGrp_t eventgroup = (pEventGrp_t)ListNodeChildGet(node);
        result = OS_ERROR;

        if(eventgroup != NULL) {
            result = OS_OK;
            eventgroup->event_reg &= ~(mask);

#ifdef PRTOS_CONFIG_USE_EVENT_EVENTGROUP_FLAG_CLEAR
            U8_t bitmask = 0x80;
            for (S8_t i = 8; i > 0; i--) {
                if(bitmask & mask) {
                    result = EventPublish(eventgroup_id, EVENTGROUP_EVENT_FLAG_CLEAR(mask), 0);
                }
                bitmask = bitmask >> 1;
            }
#endif
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}

U8_t EventgroupFlagsGet(Id_t eventgroup_id, U8_t mask)
{
    U8_t event_flag_state = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&EventGroupList, eventgroup_id) {

        pEventGrp_t eventgroup = (pEventGrp_t)ListNodeChildGet(node);
        if(eventgroup != NULL) {
            event_flag_state = ((eventgroup->event_reg & mask) ? 1 : 0);
        }
    }
    LIST_NODE_ACCESS_READ_END()

    return event_flag_state;
}

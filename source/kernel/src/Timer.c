/**********************************************************************************************************************************************
 *  File: Prior_timers.c
 *
 *  Description: Prior Timer module
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
 *  Copyrightę 2017    D. van de Spijker
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

//Timer parameter | bit 0: ON | bit 1: permanent | bit 2: auto-reset | bit 3-7 number of iterations (if bit 1 = 0) |


#include <Types.h>
#include <List.h>
#include <Convert.h>
#include <LoggerDef.h>
#include <Timer.h>
#include <Event.h>
#include <TimerDef.h>
#include <KernelTask.h>
#include <KernelTaskTimerUpdate.h>


#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>

LOG_FILE_NAME("Timer.c")

pTimer_t UtilTimerCreate(void);
pTimer_t UtilTimerFromID(Id_t timer_id);

extern U32_t OsTickPeriodGet();
#define TIMER_ID_BUFFER_SIZE 10
Id_t TimerIdBuffer[TIMER_ID_BUFFER_SIZE];

OsResult_t TimerInit(void)
{
    ListInit(&TimerList, (Id_t)ID_TYPE_TIMER, TimerIdBuffer, TIMER_ID_BUFFER_SIZE);

    KernelTaskIdTimerUpdate = KernelTaskCreate(KernelTaskTimerUpdate, 1, TASK_PARAM_ESSENTIAL, 0, NULL,
                              PRTOS_CONFIG_TIMER_INTERVAL_RESOLUTION_MS);

    //LOG_INFO_NEWLINE("TimerList: %p", &TimerList);
    return OS_OK;
}

void TimerUpdateAll(U32_t t_accu)
{
    U32_t dt;

    dt = t_accu; /* Calculate passed time since last update. */

    Id_t timer_id;
    bool destroy = false;
    U8_t list_size;
    pTimer_t timer = NULL, tmp_timer = NULL;
    ListNode_t *list_pointer = ListNodePeek(&TimerList, LIST_POSITION_HEAD);
    struct ListIterator it;
    if(list_pointer != NULL) {
        LIST_ITERATOR_BEGIN(&it, &TimerList, LIST_ITERATOR_DIRECTION_FORWARD) {
            if(it.current_node != NULL) {
                timer = (pTimer_t)ListNodeChildGet(it.current_node);
                destroy = false;

                if(timer->state == TIMER_STATE_RUNNING) {
                    timer->ticks += dt;

                    if(timer->ticks >= timer->T_us) { /* Timer overflow. */
                        timer_id = ListNodeIdGet(&timer->list_node);

#ifdef PRTOS_CONFIG_USE_EVENT_TIMER_OVERFLOW
                        // LOG_DEBUG_NEWLINE("Timer (%04x) overflow event.", timer_id);
                        EventPublish(timer_id, TIMER_EVENT_OVERFLOW, EVENT_FLAG_PERIODIC);
#endif
                        if(!(timer->parameter & TIMER_PARAMETER_PERIODIC)) { /* If timer is not Permanent. */
                            U8_t iter = TIMER_PARAMETER_ITR_GET(timer->parameter); /* Acquire iterations */
                            iter--;
                            if(iter == 0) {
                                destroy = true;
                            } else { /* Load iterations back into timer parameter if timer will not be destroyed. */
                                timer->parameter |= TIMER_PARAMETER_ITR_SET(iter);
                            }
                        }

                        /* Auto reset check. */
                        if((timer->parameter & TIMER_PARAMETER_AR) && destroy == false) {
                            timer->ticks = 0;
                        } else {
                            timer->state = TIMER_STATE_WAITING; /* Timer waiting for reset. */
                        }

                        /* Check destroy flag. */
                        if(destroy == true) {
                            TimerDelete(&timer_id);
                        }

                    } /* End check overflow. */
                } /* End check running. */

            }

        }
        LIST_ITERATOR_END(&it);
    }
}




Id_t TimerCreate(U32_t interval, U8_t parameter)
{
    if(interval == 0) {
        return INVALID_ID;
    }

    pTimer_t new_timer = UtilTimerCreate();

    if(new_timer == NULL) {
        return INVALID_ID;
    }

    Id_t new_timer_id = ListNodeIdGet(&new_timer->list_node);

    new_timer->T_us = interval;
    new_timer->parameter = parameter;
    if (parameter & TIMER_PARAMETER_ON) { //ON bit
        TimerStart(new_timer_id);
    }

    LOG_INFO_NEWLINE("Timer created with ID %04x", new_timer_id);
    return new_timer_id;
}


OsResult_t TimerDelete(Id_t *timer_id)
{
    if(*timer_id == INVALID_ID) {
        return OS_INVALID_ID;
    }
    if((*timer_id & (Id_t)ID_TYPE_TIMER) == 0) {
        return OS_INVALID_ID;
    }

    pTimer_t rm_timer = UtilTimerFromID(*timer_id);
    if(rm_timer != NULL) {
        ListNodeDeinit(&TimerList, &rm_timer->list_node);
        CoreObjectFree((void **)&rm_timer, NULL);
        LOG_INFO_NEWLINE("Deleted timer %04x", *timer_id);
        *timer_id = INVALID_ID;
        return OS_OK;
    }
    LOG_ERROR_NEWLINE("Failed to delete timer %04x", *timer_id);
    return OS_ERROR;
}



OsResult_t TimerReset(Id_t timer_id)
{
    OsResult_t result = OS_ERROR;
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        if(tmp_timer != NULL) {
            tmp_timer->ticks = 0;
            tmp_timer->state = TIMER_STATE_RUNNING;
            result = OS_OK;
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}

void TimerPause(Id_t timer_id)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        tmp_timer->state = TIMER_STATE_WAITING;
    }
    LIST_NODE_ACCESS_WRITE_END();
}

void TimerStop(Id_t timer_id)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        tmp_timer->state = TIMER_STATE_STOPPED;
        tmp_timer->ticks = 0;
    }
    LIST_NODE_ACCESS_WRITE_END();
}

void TimerStart(Id_t timer_id)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        tmp_timer->state = TIMER_STATE_RUNNING;
    }
    LIST_NODE_ACCESS_WRITE_END();
}

void TimerStartAll(void)
{
    pTimer_t list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, 0));
    U8_t i = 0;
    while(list_pointer != NULL) {
        list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, i));
        if(list_pointer != NULL) {
            list_pointer->state = TIMER_STATE_RUNNING;
        }
    }
}

void TimerStopAll(void)
{
    pTimer_t list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, 0));
    U8_t i = 0;
    while(list_pointer != NULL) {
        list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, i));
        if(list_pointer != NULL) {
            list_pointer->state = TIMER_STATE_STOPPED;
        }
    }
}

void TimerResetAll(void)
{
    pTimer_t list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, 0));
    U8_t i = 0;
    while(list_pointer != NULL) {
        list_pointer = (pTimer_t)ListNodeChildGet(ListNodePeek(&TimerList, i));
        if(list_pointer != NULL) {
            list_pointer->ticks = 0;
        }
    }
}

TmrState_t TimerStateGet(Id_t timer_id)
{
    TmrState_t timer_state = TIMER_STATE_INVALID;
    LIST_NODE_ACCESS_READ_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        timer_state = tmp_timer->state;
    }
    LIST_NODE_ACCESS_READ_END();
    return timer_state;
}

U32_t TimerTicksGet(Id_t timer_id)
{
    U32_t ticks = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        ticks = tmp_timer->ticks;
    }
    LIST_NODE_ACCESS_READ_END();
    return ticks;
}

void TimerIntervalSet(Id_t timer_id, U32_t new_interval_us)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        tmp_timer->T_us = new_interval_us;
    }
    LIST_NODE_ACCESS_WRITE_END();
}

U32_t TimerIntervalGet(Id_t timer_id)
{
    U32_t interval = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        interval = tmp_timer->T_us;
    }
    LIST_NODE_ACCESS_READ_END();
    return interval;
}

U8_t TimerIterationsGet(Id_t timer_id)
{
    U8_t itr = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        itr = TIMER_PARAMETER_ITR_GET(tmp_timer->parameter);
    }
    LIST_NODE_ACCESS_READ_END();
    return itr;
}

OsResult_t TimerIterationsSet(Id_t timer_id, U8_t iterations)
{
    OsResult_t result = OS_OK;
    if(iterations > 31 || iterations == 0) {
        result = OS_OUT_OF_BOUNDS;
    }
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        if(tmp_timer != NULL && result == OS_OK) {
            tmp_timer->parameter &= 0x07;
            tmp_timer->parameter |= TIMER_PARAMETER_ITR_SET(iterations);
        } else {
            result = OS_ERROR;
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}

U8_t TimerParameterGet(Id_t timer_id)
{
    U8_t param = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        param = tmp_timer->parameter;
    }
    LIST_NODE_ACCESS_READ_END();
    return param;
}

void TimerParameterSet(Id_t timer_id, U8_t paramtr)
{
    LIST_NODE_ACCESS_WRITE_BEGIN(&TimerList, timer_id) {
        pTimer_t tmp_timer = (pTimer_t)ListNodeChildGet(node);
        tmp_timer->parameter = paramtr;
    }
    LIST_NODE_ACCESS_WRITE_END();
}



/********************************/


pTimer_t UtilTimerFromID(Id_t timer_id)
{
    ListNode_t *node = ListSearch(&TimerList, timer_id);
    if(node != NULL) {
        return (pTimer_t)ListNodeChildGet(node);
    }
    return NULL;
}


pTimer_t UtilTimerCreate(void)
{

    pTimer_t new_timer;
    new_timer = (pTimer_t)CoreObjectAlloc(sizeof(Timer_t), 0, NULL); //malloc(sizeof(Timer_t));
    if(new_timer == NULL) {
        return NULL;
    }

    ListNodeInit(&new_timer->list_node, (void*)new_timer);

    OsResult_t result = ListNodeAddSorted(&TimerList, &new_timer->list_node);
    if(result != OS_OK) {
        ListNodeDeinit(&TimerList, &new_timer->list_node);
        CoreObjectFree((void **)&new_timer, NULL);
        return NULL;
    }
    new_timer->T_us = 0;
    new_timer->state = TIMER_STATE_STOPPED;
    new_timer->ticks = 0;
    new_timer->parameter = 0;

    return new_timer;
}
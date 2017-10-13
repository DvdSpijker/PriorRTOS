/**********************************************************************************************************************************************
 *  File: Ringbuffer.c
 *
 *  Description: Prior Ringbuffer module
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
 *  copy, modify, merge, publish, distribute, sub license, and/or sell copies of the Software,
 *  and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *     from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *     product in which Prior RTOS is integrated. Prior RTOS shall not be
 *     distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *  LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/


#include <Ringbuffer.h>
#include <Types.h>
#include <Mm.h>
#include <List.h>
#include <Event.h>
#include <RingbufferDef.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

static S8_t UtilRingbufLockWrite(Ringbuf_t *ringbuf);
static S8_t UtilRingbufUnlockWrite(Ringbuf_t *ringbuf);

static S8_t UtilRingbufLockRead(Ringbuf_t *ringbuf);
static S8_t UtilRingbufUnlockRead(Ringbuf_t *ringbuf);

static pRingbuf_t UtilRingbufFromId(Id_t ringbuf_id);
static U32_t UtilRingbufBlockNextGet(Ringbuf_t *ringbuf,  U32_t index);

//Id_t RingbufPool;
Id_t RingbufIdBuffer[2];
LinkedList_t RingbufList;


OsResult_t RingbufInit(void)
{
    //RingbufPool = MmPoolCreate(CONFIG_RINGBUFFER_MEMSIZE);
    ListInit(&RingbufList, ID_TYPE_RINGBUF, RingbufIdBuffer, 2);
    return OS_OK;
}


Id_t RingbufCreate(RingbufBase_t *buffer, U32_t size)
{
    pRingbuf_t new_ringbuf = NULL;
    void *int_buffer = NULL;
    if(buffer == NULL) {
        new_ringbuf = (pRingbuf_t)CoreObjectAlloc(sizeof(Ringbuf_t), (size*sizeof(RingbufBase_t)), &int_buffer);
        if(new_ringbuf != NULL) {
            new_ringbuf->buffer = (RingbufBase_t *)int_buffer;
            new_ringbuf->ext_buffer = false;
        } else {
            return INVALID_ID;
        }
    } else {
        new_ringbuf = (pRingbuf_t)CoreObjectAlloc(sizeof(Ringbuf_t), 0, NULL);
        if(new_ringbuf != NULL) {
            new_ringbuf->buffer = buffer;
            new_ringbuf->ext_buffer = true;
        } else {
            return INVALID_ID;
        }
    }

    ListNodeInit(&new_ringbuf->list_node, (void*)new_ringbuf);
    if(ListNodeAddSorted(&RingbufList, &new_ringbuf->list_node) != OS_OK) {
        CoreObjectFree((void **)&new_ringbuf, &int_buffer);
        return INVALID_ID;
    }

    new_ringbuf->size = size;
    new_ringbuf->head = new_ringbuf->buffer;
    new_ringbuf->tail = &new_ringbuf->buffer[size-1];

    new_ringbuf->read_index = size - 1;
    new_ringbuf->write_index = 0;

    return ListNodeIdGet(&new_ringbuf->list_node);
}

OsResult_t RingbufDelete(Id_t *ringbuf_id)
{
    pRingbuf_t ringbuf = UtilRingbufFromId(*ringbuf_id);
    if(ringbuf == NULL) {
        return OS_ERROR;
    }

    OsResult_t result = OS_OK;
    void **buffer = NULL;
    if(ringbuf->ext_buffer == false) {
        buffer = (void **)&ringbuf->buffer;
    }
    ListNodeDeinit(&RingbufList, &ringbuf->list_node);
    CoreObjectFree((void **)&ringbuf, buffer);
    *ringbuf_id = INVALID_ID;

    return result;
}

OsResult_t RingbufWrite(Id_t ringbuf_id, RingbufBase_t *data, U32_t *length)
{
    OsResult_t result = OS_OK;
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_ERROR;
    }

    if(ringbuf->dcount == ringbuf->size) {
        EventPublish(ringbuf_id, RINGBUF_EVENT_FULL, EVENT_FLAG_NONE);
        result = OS_FAIL;
    }

    U32_t i = 0;

    if(result == OS_OK) {
        if(UtilRingbufLockWrite(ringbuf) == 0) {

            while((i < *length)) { //Check if length is reached and if pWrite if still ahead of pRead
                ringbuf->buffer[ringbuf->write_index] = data[i]; //Copy data
                ringbuf->dcount++;
                i++;
                ringbuf->write_index = UtilRingbufBlockNextGet(ringbuf, ringbuf->write_index);
                if(ringbuf->write_index == ringbuf->read_index && ringbuf->dcount != 0) {
                    break;
                }

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_DATA_IN_OUT
            EventPublish(ringbuf_id, RINGBUF_EVENT_DATA_IN, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_EMPTY_FULL
            if(ringbuf->dcount == ringbuf->size) {
                EventPublish(ringbuf_id, RINGBUF_EVENT_FULL, EVENT_FLAG_NONE);
            }
#endif
            result = OS_OK;
            UtilRingbufUnlockWrite(ringbuf);
        } else {
            result = OS_LOCKED;
        }
    }


    *length = i;
    return result;
}

OsResult_t RingbufRead(Id_t ringbuf_id, RingbufBase_t *target, U32_t *amount)
{
    OsResult_t result = OS_OK;
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_ERROR;
    }

    if(ringbuf->dcount == 0) {
        EventPublish(ringbuf_id, RINGBUF_EVENT_EMPTY, EVENT_FLAG_NONE);
        result = OS_FAIL;
    }

    U32_t i = 0;

    if(result == OS_OK) {
        if(UtilRingbufLockRead(ringbuf) == 0) {
            while((i < *amount) ) { //Check if amount is reached and if read index is still lagging write index
                ringbuf->read_index = UtilRingbufBlockNextGet(ringbuf, ringbuf->read_index);
                if(ringbuf->read_index == ringbuf->write_index && ringbuf->dcount == 0) {
                    break;
                }
                target[i] =  ringbuf->buffer[ringbuf->read_index] ; //Copy new data
                ringbuf->dcount--;
                i++;

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_DATA_IN_OUT
            EventPublish(ringbuf_id, RINGBUF_EVENT_DATA_OUT, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_EMPTY_FULL
            if(ringbuf->dcount == 0) {
                EventPublish(ringbuf_id, RINGBUF_EVENT_EMPTY, EVENT_FLAG_NONE);
            }
#endif

            result = OS_OK;
            UtilRingbufUnlockRead(ringbuf);
        } else {
            result = OS_LOCKED;
        }
    }

    *amount = i;
    return result;
}


U32_t RingbufDump(Id_t ringbuf_id, RingbufBase_t* target)
{
    U32_t count = 0;
    U32_t amount  = 0;
    do {
        RingbufRead(ringbuf_id, &target[count], &amount);
        count++;
    } while (amount);
    return count;
}

OsResult_t RingbufFlush(Id_t ringbuf_id)
{
    OsResult_t result = OS_OK;
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_ERROR;
    }

    if(result == OS_OK) {
        if((UtilRingbufLockRead(ringbuf) == 0) && (UtilRingbufLockWrite(ringbuf) == 0)) {
            ringbuf->dcount = 0;
            ringbuf->write_index = 0;
            ringbuf->read_index = ringbuf->size - 1;
#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_FLUSH
            result = EventPublish(ringbuf_id, RINGBUF_EVENT_FLUSHED, EVENT_FLAG_NONE);
#endif
            UtilRingbufUnlockRead(ringbuf);
            UtilRingbufUnlockWrite(ringbuf);
        }
    } else {
        result = OS_LOCKED;
    }

    return result;
}

U32_t RingbufSearch(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length);

/* TODO: Fix bug in RingbufSearchIndex. Somehow gets stuck in inf. loop. */
U32_t RingbufSearchIndex(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length)
{
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    U32_t data_index = 0;
    U32_t occ_index = 0;
    bool found = false;
    U32_t j;
    if(ringbuf != NULL) {
        data_index = ringbuf->read_index;
        for (U32_t i = 0; i <= ringbuf->dcount; i++) {
            for (j = 0; j < query_length; j++) {
                if(ringbuf->buffer[data_index]== query[j]) {
                    found = true;
                } else {
                    found = false;
                    break;
                }
                data_index = UtilRingbufBlockNextGet(ringbuf, data_index);
            }
            i += j;
            if(found == true) {
                occ_index = data_index;
            }
            data_index = UtilRingbufBlockNextGet(ringbuf, data_index);
        }
    }

    return occ_index;
}

U32_t RingbufDataCountGet(Id_t ringbuf_id)
{
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        return 0;
    }

    return ringbuf->dcount;
}

U32_t RingbufDataSpaceGet(Id_t ringbuf_id)
{
    pRingbuf_t ringbuf = UtilRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        return 0;
    }

    return (ringbuf->size - ringbuf->dcount);
}


static pRingbuf_t UtilRingbufFromId(Id_t ringbuf_id)
{
    ListNode_t *node = ListSearch(&RingbufList, ringbuf_id);
    if(node != NULL) {
        return (pRingbuf_t)ListNodeChildGet(node);
    }
    return NULL;

}


static U32_t UtilRingbufBlockNextGet(Ringbuf_t *ringbuf, U32_t index)
{
    if(index == (ringbuf->size - 1)) {
        return index = 0;
    } else {
        return (index + 1);
    }
}


static S8_t UtilRingbufLockWrite(Ringbuf_t *ringbuf)
{
    if((ringbuf->rw_lock & RINGBUF_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringbuf->rw_lock |= RINGBUF_LOCK_WRITE_MASK;

    return 0;
}

static S8_t UtilRingbufUnlockWrite(Ringbuf_t *ringbuf)
{
    if(!(ringbuf->rw_lock & RINGBUF_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringbuf->rw_lock &= ~(RINGBUF_LOCK_WRITE_MASK);

    return 0;
}


static S8_t UtilRingbufLockRead(Ringbuf_t *ringbuf)
{
    if((ringbuf->rw_lock & RINGBUF_LOCK_READ_MASK)) {
        return -1;
    }

    ringbuf->rw_lock |= RINGBUF_LOCK_READ_MASK;

    return 0;
}

static S8_t UtilRingbufUnlockRead(Ringbuf_t *ringbuf)
{
    if(!(ringbuf->rw_lock & RINGBUF_LOCK_READ_MASK)) {
        return -1;
    }

    ringbuf->rw_lock &= ~(RINGBUF_LOCK_READ_MASK);

    return 0;
}
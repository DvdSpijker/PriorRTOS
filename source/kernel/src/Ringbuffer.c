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
#include <MemoryDef.h>
#include <List.h>
#include <Event.h>
#include <RingbufferDef.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

static S8_t IRingbufLockWrite(Ringbuf_t *ringbuf);
static S8_t IRingbufUnlockWrite(Ringbuf_t *ringbuf);

static S8_t IRingbufLockRead(Ringbuf_t *ringbuf);
static S8_t IRingbufUnlockRead(Ringbuf_t *ringbuf);

static pRingbuf_t IRingbufFromId(Id_t ringbuf_id);
static U32_t IRingbufBlockNextGet(Ringbuf_t *ringbuf,  U32_t index);

LinkedList_t RingbufList;


OsResult_t KRingbufInit(void)
{
    //RingbufPool = MemPoolCreate(CONFIG_RINGBUFFER_MEMSIZE);
    ListInit(&RingbufList, ID_TYPE_RINGBUF);
    return OS_RES_OK;
}


Id_t RingbufCreate(RingbufBase_t *buffer, U32_t size)
{
    pRingbuf_t new_ringbuf = NULL;
    void *int_buffer = NULL;
    if(buffer == NULL) {
        new_ringbuf = (pRingbuf_t)KMemAllocObject(sizeof(Ringbuf_t), (size*sizeof(RingbufBase_t)), &int_buffer);
        if(new_ringbuf != NULL) {
            new_ringbuf->buffer = (RingbufBase_t *)int_buffer;
            new_ringbuf->ext_buffer = false;
        } else {
            return OS_ID_INVALID;
        }
    } else {
        new_ringbuf = (pRingbuf_t)KMemAllocObject(sizeof(Ringbuf_t), 0, NULL);
        if(new_ringbuf != NULL) {
            new_ringbuf->buffer = buffer;
            new_ringbuf->ext_buffer = true;
        } else {
            return OS_ID_INVALID;
        }
    }

    ListNodeInit(&new_ringbuf->list_node, (void*)new_ringbuf);
    if(ListNodeAddSorted(&RingbufList, &new_ringbuf->list_node) != OS_RES_OK) {
        KMemFreeObject((void **)&new_ringbuf, &int_buffer);
        return OS_ID_INVALID;
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
    pRingbuf_t ringbuf = IRingbufFromId(*ringbuf_id);
    if(ringbuf == NULL) {
        return OS_RES_ERROR;
    }

    OsResult_t result = OS_RES_OK;
    void **buffer = NULL;
    if(ringbuf->ext_buffer == false) {
        buffer = (void **)&ringbuf->buffer;
    }
    ListNodeDeinit(&RingbufList, &ringbuf->list_node);
    KMemFreeObject((void **)&ringbuf, buffer);
    *ringbuf_id = OS_ID_INVALID;

    return result;
}

OsResult_t RingbufWrite(Id_t ringbuf_id, RingbufBase_t *data, U32_t *length, U32_t timeout)
{
    OsResult_t result = OS_RES_OK;
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_RES_ERROR;
    }

    if(ringbuf->dcount == ringbuf->size) {
        EventEmit(ringbuf_id, RINGBUF_EVENT_FULL, EVENT_FLAG_NONE);
        result = OS_RES_FAIL;
    }

    U32_t i = 0;

    if(result == OS_RES_OK) {
        if(IRingbufLockWrite(ringbuf) == 0) {

            while((i < *length)) { //Check if length is reached and if pWrite if still ahead of pRead
                ringbuf->buffer[ringbuf->write_index] = data[i]; //Copy data
                ringbuf->dcount++;
                i++;
                ringbuf->write_index = IRingbufBlockNextGet(ringbuf, ringbuf->write_index);
                if(ringbuf->write_index == ringbuf->read_index && ringbuf->dcount != 0) {
                    break;
                }

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_DATA_IN_OUT
            EventEmit(ringbuf_id, RINGBUF_EVENT_DATA_IN, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_EMPTY_FULL
            if(ringbuf->dcount == ringbuf->size) {
                EventEmit(ringbuf_id, RINGBUF_EVENT_FULL, EVENT_FLAG_NONE);
            }
#endif
            result = OS_RES_OK;
            IRingbufUnlockWrite(ringbuf);
        } else {
            result = OS_RES_LOCKED;
        }
    }


    *length = i;
    return result;
}

OsResult_t RingbufRead(Id_t ringbuf_id, RingbufBase_t *target, U32_t *amount, U32_t timeout)
{
    OsResult_t result = OS_RES_OK;
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_RES_ERROR;
    }

    if(ringbuf->dcount == 0) {
        EventEmit(ringbuf_id, RINGBUF_EVENT_EMPTY, EVENT_FLAG_NONE);
        result = OS_RES_FAIL;
    }

    U32_t i = 0;

    if(result == OS_RES_OK) {
        if(IRingbufLockRead(ringbuf) == 0) {
            while((i < *amount) ) { //Check if amount is reached and if read index is still lagging write index
                ringbuf->read_index = IRingbufBlockNextGet(ringbuf, ringbuf->read_index);
                if(ringbuf->read_index == ringbuf->write_index && ringbuf->dcount == 0) {
                    break;
                }
                target[i] =  ringbuf->buffer[ringbuf->read_index] ; //Copy new data
                ringbuf->dcount--;
                i++;

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_DATA_IN_OUT
            EventEmit(ringbuf_id, RINGBUF_EVENT_DATA_OUT, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_EMPTY_FULL
            if(ringbuf->dcount == 0) {
                EventEmit(ringbuf_id, RINGBUF_EVENT_EMPTY, EVENT_FLAG_NONE);
            }
#endif

            result = OS_RES_OK;
            IRingbufUnlockRead(ringbuf);
        } else {
            result = OS_RES_LOCKED;
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
        RingbufRead(ringbuf_id, &target[count], &amount, OS_RES_TIMEOUT_INFINITE);
        count++;
    } while (amount);
    return count;
}

OsResult_t RingbufFlush(Id_t ringbuf_id)
{
    OsResult_t result = OS_RES_OK;
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        result = OS_RES_ERROR;
    }

    if(result == OS_RES_OK) {
        if((IRingbufLockRead(ringbuf) == 0) && (IRingbufLockWrite(ringbuf) == 0)) {
            ringbuf->dcount = 0;
            ringbuf->write_index = 0;
            ringbuf->read_index = ringbuf->size - 1;
#ifdef PRTOS_CONFIG_USE_EVENT_RINGBUF_FLUSH
            result = EventEmit(ringbuf_id, RINGBUF_EVENT_FLUSHED, EVENT_FLAG_NONE);
#endif
            IRingbufUnlockRead(ringbuf);
            IRingbufUnlockWrite(ringbuf);
        }
    } else {
        result = OS_RES_LOCKED;
    }

    return result;
}

U32_t RingbufSearch(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length);

/* TODO: Fix bug in RingbufSearchIndex. Somehow gets stuck in inf. loop. */
U32_t RingbufSearchIndex(Id_t ringbuf_id, RingbufBase_t *query, U32_t query_length)
{
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
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
                data_index = IRingbufBlockNextGet(ringbuf, data_index);
            }
            i += j;
            if(found == true) {
                occ_index = data_index;
            }
            data_index = IRingbufBlockNextGet(ringbuf, data_index);
        }
    }

    return occ_index;
}

U32_t RingbufDataCountGet(Id_t ringbuf_id)
{
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        return 0;
    }

    return ringbuf->dcount;
}

U32_t RingbufDataSpaceGet(Id_t ringbuf_id)
{
    pRingbuf_t ringbuf = IRingbufFromId(ringbuf_id);
    if(ringbuf == NULL) {
        return 0;
    }

    return (ringbuf->size - ringbuf->dcount);
}


static pRingbuf_t IRingbufFromId(Id_t ringbuf_id)
{
    ListNode_t *node = ListSearch(&RingbufList, ringbuf_id);
    if(node != NULL) {
        return (pRingbuf_t)ListNodeChildGet(node);
    }
    return NULL;

}


static U32_t IRingbufBlockNextGet(Ringbuf_t *ringbuf, U32_t index)
{
    if(index == (ringbuf->size - 1)) {
        return index = 0;
    } else {
        return (index + 1);
    }
}


static S8_t IRingbufLockWrite(Ringbuf_t *ringbuf)
{
    if((ringbuf->rw_lock & RINGBUF_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringbuf->rw_lock |= RINGBUF_LOCK_WRITE_MASK;

    return 0;
}

static S8_t IRingbufUnlockWrite(Ringbuf_t *ringbuf)
{
    if(!(ringbuf->rw_lock & RINGBUF_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringbuf->rw_lock &= ~(RINGBUF_LOCK_WRITE_MASK);

    return 0;
}


static S8_t IRingbufLockRead(Ringbuf_t *ringbuf)
{
    if((ringbuf->rw_lock & RINGBUF_LOCK_READ_MASK)) {
        return -1;
    }

    ringbuf->rw_lock |= RINGBUF_LOCK_READ_MASK;

    return 0;
}

static S8_t IRingbufUnlockRead(Ringbuf_t *ringbuf)
{
    if(!(ringbuf->rw_lock & RINGBUF_LOCK_READ_MASK)) {
        return -1;
    }

    ringbuf->rw_lock &= ~(RINGBUF_LOCK_READ_MASK);

    return 0;
}
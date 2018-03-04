/**********************************************************************************************************************************************
 *  File: Ringpacket.c
 *
 *  Description: Prior Ringpacket module
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


#include <Ringpacket.h>
#include <Types.h>
#include <MemoryDef.h>
#include <List.h>
#include <Event.h>
#include <RingpacketDef.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

static S8_t IRingpacketLockWrite(Ringpacket_t *ringpacket);
static S8_t IRingpacketUnlockWrite(Ringpacket_t *ringpacket);

static S8_t IRingpacketLockRead(Ringpacket_t *ringpacket);
static S8_t IRingpacketUnlockRead(Ringpacket_t *ringpacket);

static pRingpacket_t IRingpacketFromId(Id_t ringpacket_id);
static U32_t IRingpacketBlockNextGet(Ringpacket_t *ringpacket,  U32_t index);

LinkedList_t RingpacketList;


OsResult_t KRingpacketInit(void)
{
    //RingpacketPool = MemPoolCreate(CONFIG_RINGpacket_MEMSIZE);
    ListInit(&RingpacketList, ID_TYPE_RINGpacket);
    return OS_RES_OK;
}


Id_t RingpacketCreate(RingpacketBase_t *packet, U32_t size)
{
    pRingpacket_t new_ringpacket = NULL;
    void *int_packet = NULL;
    if(packet == NULL) {
        new_ringpacket = (pRingpacket_t)KMemAllocObject(sizeof(Ringpacket_t), (size*sizeof(RingpacketBase_t)), &int_packet);
        if(new_ringpacket != NULL) {
            new_ringpacket->packet = (RingpacketBase_t *)int_packet;
            new_ringpacket->ext_packet = false;
        } else {
            return OS_ID_INVALID;
        }
    } else {
        new_ringpacket = (pRingpacket_t)KMemAllocObject(sizeof(Ringpacket_t), 0, NULL);
        if(new_ringpacket != NULL) {
            new_ringpacket->packet = packet;
            new_ringpacket->ext_packet = true;
        } else {
            return OS_ID_INVALID;
        }
    }

    ListNodeInit(&new_ringpacket->list_node, (void*)new_ringpacket);
    if(ListNodeAddSorted(&RingpacketList, &new_ringpacket->list_node) != OS_RES_OK) {
        KMemFreeObject((void **)&new_ringpacket, &int_packet);
        return OS_ID_INVALID;
    }

    new_ringpacket->size = size;
    new_ringpacket->head = new_ringpacket->packet;
    new_ringpacket->tail = &new_ringpacket->packet[size-1];

    new_ringpacket->read_index = size - 1;
    new_ringpacket->write_index = 0;

    return ListNodeIdGet(&new_ringpacket->list_node);
}

OsResult_t RingpacketDelete(Id_t *ringpacket_id)
{
    pRingpacket_t ringpacket = IRingpacketFromId(*ringpacket_id);
    if(ringpacket == NULL) {
        return OS_RES_ERROR;
    }

    OsResult_t result = OS_RES_OK;
    void **packet = NULL;
    if(ringpacket->ext_packet == false) {
        packet = (void **)&ringpacket->packet;
    }
    ListNodeDeinit(&RingpacketList, &ringpacket->list_node);
    KMemFreeObject((void **)&ringpacket, packet);
    *ringpacket_id = OS_ID_INVALID;

    return result;
}

OsResult_t RingpacketWrite(Id_t ringpacket_id, RingpacketBase_t *data, U32_t *length, U32_t timeout)
{
    OsResult_t result = OS_RES_OK;
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    if(ringpacket == NULL) {
        result = OS_RES_ERROR;
    }

    if(ringpacket->dcount == ringpacket->size) {
        EventEmit(ringpacket_id, RINGpacket_EVENT_FULL, EVENT_FLAG_NONE);
        result = OS_RES_FAIL;
    }

    U32_t i = 0;

    if(result == OS_RES_OK) {
        if(IRingpacketLockWrite(ringpacket) == 0) {

            while((i < *length)) { //Check if length is reached and if pWrite if still ahead of pRead
                ringpacket->packet[ringpacket->write_index] = data[i]; //Copy data
                ringpacket->dcount++;
                i++;
                ringpacket->write_index = IRingpacketBlockNextGet(ringpacket, ringpacket->write_index);
                if(ringpacket->write_index == ringpacket->read_index && ringpacket->dcount != 0) {
                    break;
                }

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_DATA_IN_OUT
            EventEmit(ringpacket_id, RINGpacket_EVENT_DATA_IN, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_EMPTY_FULL
            if(ringpacket->dcount == ringpacket->size) {
                EventEmit(ringpacket_id, RINGpacket_EVENT_FULL, EVENT_FLAG_NONE);
            }
#endif
            result = OS_RES_OK;
            IRingpacketUnlockWrite(ringpacket);
        } else {
            result = OS_RES_LOCKED;
        }
    }


    *length = i;
    return result;
}

OsResult_t RingpacketRead(Id_t ringpacket_id, RingpacketBase_t *target, U32_t *amount, U32_t timeout)
{
    OsResult_t result = OS_RES_OK;
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    if(ringpacket == NULL) {
        result = OS_RES_ERROR;
    }

    if(ringpacket->dcount == 0) {
        EventEmit(ringpacket_id, RINGpacket_EVENT_EMPTY, EVENT_FLAG_NONE);
        result = OS_RES_FAIL;
    }

    U32_t i = 0;

    if(result == OS_RES_OK) {
        if(IRingpacketLockRead(ringpacket) == 0) {
            while((i < *amount) ) { //Check if amount is reached and if read index is still lagging write index
                ringpacket->read_index = IRingpacketBlockNextGet(ringpacket, ringpacket->read_index);
                if(ringpacket->read_index == ringpacket->write_index && ringpacket->dcount == 0) {
                    break;
                }
                target[i] =  ringpacket->packet[ringpacket->read_index] ; //Copy new data
                ringpacket->dcount--;
                i++;

            }

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_DATA_IN_OUT
            EventEmit(ringpacket_id, RINGpacket_EVENT_DATA_OUT, EVENT_FLAG_NONE);
#endif

#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_EMPTY_FULL
            if(ringpacket->dcount == 0) {
                EventEmit(ringpacket_id, RINGpacket_EVENT_EMPTY, EVENT_FLAG_NONE);
            }
#endif

            result = OS_RES_OK;
            IRingpacketUnlockRead(ringpacket);
        } else {
            result = OS_RES_LOCKED;
        }
    }

    *amount = i;
    return result;
}


U32_t RingpacketDump(Id_t ringpacket_id, RingpacketBase_t* target)
{
    U32_t count = 0;
    U32_t amount  = 0;
    do {
        RingpacketRead(ringpacket_id, &target[count], &amount, OS_TIMEOUT_INFINITE);
        count++;
    } while (amount);
    return count;
}

OsResult_t RingpacketFlush(Id_t ringpacket_id)
{
    OsResult_t result = OS_RES_OK;
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    if(ringpacket == NULL) {
        result = OS_RES_ERROR;
    }

    if(result == OS_RES_OK) {
        if((IRingpacketLockRead(ringpacket) == 0) && (IRingpacketLockWrite(ringpacket) == 0)) {
            ringpacket->dcount = 0;
            ringpacket->write_index = 0;
            ringpacket->read_index = ringpacket->size - 1;
#ifdef PRTOS_CONFIG_USE_EVENT_RINGpacket_FLUSH
            result = EventEmit(ringpacket_id, RINGpacket_EVENT_FLUSHED, EVENT_FLAG_NONE);
#endif
            IRingpacketUnlockRead(ringpacket);
            IRingpacketUnlockWrite(ringpacket);
        }
    } else {
        result = OS_RES_LOCKED;
    }

    return result;
}

U32_t RingpacketSearch(Id_t ringpacket_id, RingpacketBase_t *query, U32_t query_length);

/* TODO: Fix bug in RingpacketSearchIndex. Somehow gets stuck in inf. loop. */
U32_t RingpacketSearchIndex(Id_t ringpacket_id, RingpacketBase_t *query, U32_t query_length)
{
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    U32_t data_index = 0;
    U32_t occ_index = 0;
    bool found = false;
    U32_t j;
    if(ringpacket != NULL) {
        data_index = ringpacket->read_index;
        for (U32_t i = 0; i <= ringpacket->dcount; i++) {
            for (j = 0; j < query_length; j++) {
                if(ringpacket->packet[data_index]== query[j]) {
                    found = true;
                } else {
                    found = false;
                    break;
                }
                data_index = IRingpacketBlockNextGet(ringpacket, data_index);
            }
            i += j;
            if(found == true) {
                occ_index = data_index;
            }
            data_index = IRingpacketBlockNextGet(ringpacket, data_index);
        }
    }

    return occ_index;
}

U32_t RingpacketDataCountGet(Id_t ringpacket_id)
{
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    if(ringpacket == NULL) {
        return 0;
    }

    return ringpacket->dcount;
}

U32_t RingpacketDataSpaceGet(Id_t ringpacket_id)
{
    pRingpacket_t ringpacket = IRingpacketFromId(ringpacket_id);
    if(ringpacket == NULL) {
        return 0;
    }

    return (ringpacket->size - ringpacket->dcount);
}


static pRingpacket_t IRingpacketFromId(Id_t ringpacket_id)
{
    ListNode_t *node = ListSearch(&RingpacketList, ringpacket_id);
    if(node != NULL) {
        return (pRingpacket_t)ListNodeChildGet(node);
    }
    return NULL;

}


static U32_t IRingpacketBlockNextGet(Ringpacket_t *ringpacket, U32_t index)
{
    if(index == (ringpacket->size - 1)) {
        return index = 0;
    } else {
        return (index + 1);
    }
}


static S8_t IRingpacketLockWrite(Ringpacket_t *ringpacket)
{
    if((ringpacket->rw_lock & RINGpacket_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringpacket->rw_lock |= RINGpacket_LOCK_WRITE_MASK;

    return 0;
}

static S8_t IRingpacketUnlockWrite(Ringpacket_t *ringpacket)
{
    if(!(ringpacket->rw_lock & RINGpacket_LOCK_WRITE_MASK)) {
        return -1;
    }

    ringpacket->rw_lock &= ~(RINGpacket_LOCK_WRITE_MASK);

    return 0;
}


static S8_t IRingpacketLockRead(Ringpacket_t *ringpacket)
{
    if((ringpacket->rw_lock & RINGpacket_LOCK_READ_MASK)) {
        return -1;
    }

    ringpacket->rw_lock |= RINGpacket_LOCK_READ_MASK;

    return 0;
}

static S8_t IRingpacketUnlockRead(Ringpacket_t *ringpacket)
{
    if(!(ringpacket->rw_lock & RINGpacket_LOCK_READ_MASK)) {
        return -1;
    }

    ringpacket->rw_lock &= ~(RINGpacket_LOCK_READ_MASK);

    return 0;
}
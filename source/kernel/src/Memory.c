/**********************************************************************************************************************************************
 *  File: Prior_mm.c
 *
 *  Description: Prior Memory Management module
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


#include <Memory.h>
#include <MemoryDef.h>
#include <CoreDef.h>
#include <Logger.h>

LOG_FILE_NAME("Mem.c");

#if PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION==1
#include <Fletcher.h>
#define POOL_PADDING_START 0xCADEADFE
#define POOL_PADDING_END   0xDECAFBEE
static U16_t IPoolChecksumGenerate(pPmb_t pool);
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#define ALLOC_DATA_OFFSET   sizeof(U32_t) / sizeof(MemBase_t)

extern void OsCritSectBegin(void);
extern void OsCritSectEnd(void);

Pmb_t PoolTable[N_POOLS];

MemBase_t OsHeap[(PRTOS_CONFIG_OS_HEAP_SIZE_BYTES / sizeof(MemBase_t))];

U16_t TotalHeapPools;
U32_t TotalHeapSize;
U16_t HeapIndexEnd;
MemBase_t* OsHeapStart;
MemBase_t* OsHeapEnd;
Id_t KernelHeapId;
Id_t ObjectHeapId;

U16_t IPoolIdFromPointer(MemBase_t* ptr);

/************OS Memory Management************/

OsResult_t KMemInit(void)
{
    TotalHeapPools = (U16_t)(N_POOLS);
    TotalHeapSize = (U32_t)sizeof(OsHeap);
    HeapIndexEnd = sizeof(OsHeap) / sizeof(MemBase_t) - 1;
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        PoolTable[i].start_addr = NULL;
        PoolTable[i].end_addr = NULL;
    }
    for (U32_t j = 0; j<(HeapIndexEnd); j++) {
        OsHeap[j] = 0;
    }

    OsHeapStart = &OsHeap[0];
    OsHeapEnd = &OsHeap[HeapIndexEnd];

    return OS_OK;
}

OsResult_t KMemKernelHeapSet(Id_t kernel_heap)
{
    if(KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }

    KernelHeapId = kernel_heap;

    return OS_OK;
}

OsResult_t KMemObjectHeapSet(Id_t object_heap)
{
    if(KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }

    ObjectHeapId = object_heap;

    return OS_OK;
}

Id_t MemPoolCreate(U32_t pool_size)
{
    if((pool_size == 0) || (pool_size > TotalHeapSize)) {
        return OS_ID_INVALID;
    }

    OsCritSectBegin();

    U16_t pool_id = 0;
    U16_t tmp_pool_id = 0;
    pPmb_t pmb = NULL;
    U32_t mem_req = 0;
    U32_t mem_found = 1;
    MemBase_t* mem_ptr;
    U32_t mem_offset = 0;

    /* Search for an unused PMB */
    while(PoolTable[pool_id].start_addr != NULL && pool_id < (TotalHeapPools)) {
        pool_id++;
    }
    /* Check if one was found. */
    if(PoolTable[pool_id].start_addr != NULL) {
        LOG_ERROR_NEWLINE("Pool ID %04x is invalid.");
        OsCritSectEnd();
        return OS_ID_INVALID;
    }

    pmb = &(PoolTable[pool_id]);
    mem_req = pool_size;

    mem_ptr = OsHeapStart;

    while(mem_found < mem_req && (mem_ptr + mem_offset) < OsHeapEnd) {
        tmp_pool_id = IPoolIdFromPointer((mem_ptr + mem_offset));
        if(tmp_pool_id == OS_ID_INVALID) {
            mem_offset += 1;
            mem_found += sizeof(MemBase_t);
        } else {
            mem_ptr = (PoolTable[tmp_pool_id].end_addr + 1);
            mem_offset = 0;
            mem_found = 1;
        }
    }
    if(mem_found >= mem_req) {
        pmb->end_addr = (mem_ptr+mem_offset);
        pmb->start_addr = (mem_ptr);
        pmb->mem_left = pool_size;
        pmb->pool_size = pool_size;
        pmb->N = 0;
#if PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION==1
        pmb->start_padding = pmb->start_addr;
        pmb->end_padding = pmb->end_addr;
        *(pmb->start_padding) = POOL_PADDING_START;
        *(pmb->end_padding) = POOL_PADDING_END;
        pmb->checksum = IPoolChecksumGenerate(pmb);
#endif
    } else {
        LOG_ERROR_NEWLINE("Not enough memory available (%u bytes) to create a new pool (%u bytes).", mem_found, mem_req);
        pool_id = OS_ID_INVALID;    //No memory available for the requested pool size
    }

    OsCritSectEnd();
    return pool_id;
}

OsResult_t MemPoolDelete(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return OS_INVALID_ID;
    }
    if(pool_id == KernelHeapId || pool_id == ObjectHeapId) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();
    MemPoolFormat(pool_id);
    PoolTable[pool_id].pool_size = 0;
    PoolTable[pool_id].end_addr = NULL;
    PoolTable[pool_id].start_addr = NULL;
    PoolTable[pool_id].mem_left = 0;
    OsCritSectEnd();
    return OS_OK;
}

OsResult_t MemPoolFormat(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return OS_INVALID_ID;
    }
    if((pool_id == KernelHeapId || pool_id == ObjectHeapId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();
    pPmb_t mempool = &PoolTable[pool_id];
    MemBase_t* mem_ptr = mempool->start_addr;
    for (U32_t i=0; i<mempool->pool_size; i+=sizeof(MemBase_t)) {
        *mem_ptr = 0;
        mem_ptr++;
    }
    mempool->mem_left = mempool->pool_size;
    mempool->N=0;
    OsCritSectEnd();

    return OS_OK;
}


U32_t MemPoolFreeSpaceGet(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return 0;
    }
    U32_t mem_left = 0;
    OsCritSectBegin();
    mem_left = PoolTable[pool_id].mem_left;
    OsCritSectEnd();

    return mem_left;
}

U32_t MemPoolUsedSpaceGet(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return 0;
    }

    U32_t mem_used = 0;
    OsCritSectBegin();
    mem_used = (PoolTable[pool_id].pool_size - PoolTable[pool_id].mem_left);
    OsCritSectEnd();
    return mem_used;
}

U32_t MemOsHeapFreeSpaceGet(void)
{
    volatile U32_t total_data=0;
    OsCritSectBegin();
    for (U8_t i=0; i<TotalHeapPools; i++) {
        if(PoolTable[i].start_addr != NULL) {
            total_data += (PoolTable[i].pool_size - PoolTable[i].mem_left);
        }
    }
    OsCritSectEnd();
    return (TotalHeapSize - total_data);
}


void *MemAlloc(Id_t pool_id, U32_t size)
{
    if(size == 0) {
        return NULL;
    }
    if(pool_id>=TotalHeapPools) {
        return NULL;
    }
    /* Only allow allocations in Kernel pool/Object pool in Kernel mode. */
    if((pool_id == KernelHeapId || pool_id == ObjectHeapId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return NULL;
    }

    size += sizeof(U32_t); /* Reserve extra space to save the size. */
    //if(size % sizeof(U32_t)) {
        //size += sizeof(U32_t) - (size % sizeof(U32_t));
    //}
    if(PoolTable[pool_id].mem_left < size) {

        return NULL;
    }

    OsCritSectBegin();
    MemBase_t* start_address;
    MemBase_t* end_address;

    /* Start at the start address of the pool. */
    start_address = end_address = PoolTable[pool_id].start_addr;

    U8_t mem_found = 0;

    /* Loop through the pool's memory space. */
    while (mem_found < (size) && start_address < PoolTable[pool_id].end_addr) {
        /* If the current address is 0, increase the amount of memory found.
         * If not 0, it is the size of an allocation. This enables skipping
         * that part of the memory since it is in use. */
        if(*(end_address) == 0) {
            mem_found += sizeof(MemBase_t);
            end_address++;
        } else { /* If in use, skip. */
            mem_found = 0;
            if (*(start_address) != 0) { /* If not 0, address = allocation size. */
                start_address += (U32_t)((*(start_address) / sizeof (MemBase_t))); /* Skip block. */
                end_address = start_address;
            } else {
                start_address = end_address; /* If 0, continue searching at the end address i.e.
                                              * the last non-zero address i.e. the size of an allocation. */
            }
        }
    }
    void* return_address = NULL;
    if(mem_found >= size) {
        *(U32_t*)(start_address) = size;
        return_address = (start_address + ALLOC_DATA_OFFSET);
        PoolTable[pool_id].mem_left-= size;
        PoolTable[pool_id].N++;
    } else {
        LOG_ERROR_NEWLINE("Not enough memory available (%u bytes) to allocate %u bytes.", mem_found, size);
        OsCritSectEnd();
        return NULL;
    }
    OsCritSectEnd();
    return return_address;

}



OsResult_t MemReAlloc(Id_t cur_pool_id, Id_t new_pool_id, void **ptr, U32_t new_size)
{

    if(cur_pool_id > TotalHeapPools || new_pool_id > TotalHeapPools) {
        return OS_OUT_OF_BOUNDS;
    }

    if((cur_pool_id == KernelHeapId || new_pool_id == KernelHeapId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    } else if((cur_pool_id ==ObjectHeapId || new_pool_id == ObjectHeapId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }



    if(new_pool_id == OS_ID_INVALID) {
        new_pool_id = cur_pool_id;
    }
    OsCritSectBegin();

    void* tmp_ptr = MemAlloc(new_pool_id,new_size);

    if(tmp_ptr == NULL) {
        OsCritSectEnd();
        return OS_NULL_POINTER;
    }

//for (U32_t i = 0; (i < size) && (i < new_size); i++) {
//(MemBase_t *)tmp_ptr[i] = (MemBase_t *)(*ptr)[i];
//}
    /* TODO: Copy data in MemReAlloc. */

    MemFree(ptr);

    *ptr = tmp_ptr;

    PoolTable[cur_pool_id].N--;
    PoolTable[new_pool_id].N++;
    OsCritSectEnd();
    return OS_OK;
}



OsResult_t MemFree(void **ptr)
{
    if(*ptr == NULL) {
        return OS_NULL_POINTER;
    }

    MemBase_t* tmp_ptr = (MemBase_t*)(*ptr);
    Id_t pool_id = IPoolIdFromPointer(tmp_ptr);

    if(pool_id == OS_ID_INVALID) { /* Pool not found. */
        return OS_INVALID_ID;
    }
    if((pool_id == KernelHeapId || pool_id == ObjectHeapId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();

    OsResult_t result = OS_OK;
    tmp_ptr = tmp_ptr - ALLOC_DATA_OFFSET;
    U32_t size = *(U32_t*)tmp_ptr; /* Allocation size in bytes. */
    U32_t phy_size = size / sizeof(MemBase_t); /* Physical size in nr of array nodes. */

    if(result == OS_OK) {
        /* Zero memory block. */
        for (U32_t i=0; i<phy_size; i++) {
            *((MemBase_t*)((MemBase_t*)(*ptr) - ALLOC_DATA_OFFSET + i)) = 0;
        }

        PoolTable[pool_id].mem_left+= size;
        PoolTable[pool_id].N--;

        *ptr = NULL; /* Set pointer to NULL, avoids reusing it in app by accident. */
    }

    OsCritSectEnd();
    return result;
}

U32_t MemAllocSizeGet(void *ptr)
{
    if(ptr == NULL) {
        return 0;
    }

    U32_t size = 0;
    MemBase_t* tmp_ptr = (MemBase_t*)(ptr);

    /*Test if the pointer is in a pool address space.
    If the pool ID equals 0 the pointer is either not
    in a pool space or within the kernel pool.
    Operations that access the kernel pool may only
    be executed if kernel-mode flag is set*/
    U16_t pool_id = IPoolIdFromPointer(tmp_ptr);
    if(pool_id == KernelHeapId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return 0;
    } else if(pool_id == OS_ID_INVALID) {
        return 0;
    }

    tmp_ptr = tmp_ptr - ALLOC_DATA_OFFSET;
    size = (*(U32_t*)tmp_ptr - sizeof(U32_t));
    return size;
}


/*******************************/


U16_t IPoolIdFromPointer(MemBase_t* ptr)
{
    U16_t id = OS_ID_INVALID;
    //OsCritSectBegin();
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        if(ptr >= PoolTable[i].start_addr && ptr <= PoolTable[i].end_addr) {
            id = i;
        }
    }
    //OsCritSectEnd();
    return id;
}


#if PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION==1
static U16_t IPoolChecksumGenerate(pPmb_t pool)
{
    U8_t checksum_feed[(2 * sizeof(U32_t) + pool->N)];
    ConvertIntToBytes(*(pool->start_padding), 4, checksum_feed);
    ConvertIntToBytes(*(pool->end_padding), 4, (checksum_feed + 4));
    if(pool->N > 0) {

    }
    return (FletcherGenerate(checksum_feed, sizeof(checksum_feed)));
}

#endif


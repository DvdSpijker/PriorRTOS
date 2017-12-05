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

LOG_FILE_NAME("Memory.c");

#if PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION==1
#include <Fletcher.h>
#define POOL_PADDING_START 0xCADEADFE
#define POOL_PADDING_END   0xDECAFBEE
static U16_t IPoolChecksumGenerate(pPmb_t pool);
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#define ALLOC_DATA_INDEX_OFFSET   1

extern void OsCritSectBegin(void);
extern void OsCritSectEnd(void);

#define BLOCK_SIZE_BYTES 16
#define BLOCK_SIZE	     ( (BLOCK_SIZE_BYTES %  sizeof(MemBase_t)) ? (BLOCK_SIZE_BYTES / sizeof(MemBase_t)) + 1 : (BLOCK_SIZE_BYTES / sizeof(MemBase_t)) ) 

Pmb_t PoolTable[N_POOLS];

MemBase_t OsHeap[(PRTOS_CONFIG_OS_HEAP_SIZE_BYTES / sizeof(MemBase_t))];

U16_t TotalHeapPools;
U32_t TotalHeapSize;
U16_t HeapIndexEnd;

Id_t KernelPoolId;
Id_t ObjectPoolId; 

U16_t IPoolIdFromIndex(U32_t index);
U16_t IPoolIdFromPointer(MemBase_t *ptr);

/************OS Memory Management************/

OsResult_t KMemInit(void)
{
    TotalHeapPools = (U16_t)(N_POOLS);
    TotalHeapSize = (U32_t)sizeof(OsHeap);
    HeapIndexEnd = TotalHeapSize - 1;
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        PoolTable[i].start_index = 0;
        PoolTable[i].end_index = 0;
		PoolTable[i].pool_size = 0;
		PoolTable[i].mem_left = 0;
    }
    for (U32_t j = 0; j<(HeapIndexEnd); j++) {
        OsHeap[j] = 0;
    }
	
	KernelPoolId = MemPoolCreate(KERNEL_POOL_SIZE_BYTES);
	ObjectPoolId = MemPoolCreate(PRTOS_CONFIG_OS_HEAP_SIZE_BYTES - PRTOS_CONFIG_USER_HEAP_SIZE_BYTES);
	
    return OS_OK;
}

OsResult_t KMemObjectHeapSet(Id_t object_heap)
{
    if(KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }

    ObjectPoolId = object_heap;

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
    U32_t index = 0;
    U32_t index_offset = 0;

    /* Search for an unused PMB */
    while(PoolTable[pool_id].pool_size != 0 && pool_id < TotalHeapPools) {
        pool_id++;
    }
    /* Check if one was found. */
    if(PoolTable[pool_id].pool_size != 0) {
        LOG_ERROR_NEWLINE("Pool ID %04x is invalid.");
        OsCritSectEnd();
        return OS_ID_INVALID;
    }

    pmb = &(PoolTable[pool_id]);
    mem_req =  (pool_size % BLOCK_SIZE_BYTES) ? (pool_size / BLOCK_SIZE_BYTES) + 1 : (pool_size / BLOCK_SIZE_BYTES) );

    while(mem_found < mem_req && (index + index_offset) < HeadIndexEnd) {
        tmp_pool_id = IPoolIdFromIndex(index + index_offset);
        if(tmp_pool_id == OS_ID_INVALID) {
            index_offset += 1;
            mem_found += sizeof(MemBase_t);
        } else {
            index = (PoolTable[tmp_pool_id].end_index + 1);
            index_offset = 0;
            mem_found = 0;
        }
    }
    if(mem_found >= mem_req) {
        pmb->end_index = index + index_offset;
        pmb->start_index = index;
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
    if(pool_id == KernelPoolId || pool_id == ObjectPoolId) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();
    MemPoolFormat(pool_id);
    PoolTable[pool_id].pool_size = 0;
    PoolTable[pool_id].end_index = 0;
    PoolTable[pool_id].start_index = 0;
    PoolTable[pool_id].mem_left = 0;
    OsCritSectEnd();
    return OS_OK;
}

OsResult_t MemPoolFormat(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return OS_INVALID_ID;
    }
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();
    pPmb_t mempool = &PoolTable[pool_id];
    for (U32_t i = mempool->start_index; i<mempool->end_index; i++) {
		OsHeap[i] = 0;
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
        if(PoolTable[i].pool_size != 0) {
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
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return NULL;
    }
	
	U32_t size_blocks = ( (size % BLOCK_SIZE_BYTES) ? (size / BLOCK_SIZE_BYTES) + 1 : (size / BLOCK_SIZE_BYTES) );
	
    size += sizeof(U32_t); /* Reserve extra space to save the size. */
    //if(size % sizeof(U32_t)) {
        //size += sizeof(U32_t) - (size % sizeof(U32_t));
    //}
    if(PoolTable[pool_id].mem_left < size) {

        return NULL;
    }

    OsCritSectBegin();
    U32_t start_index;
    U32_t end_index;

    /* Start at the start address of the pool. */
    start_index = end_index = PoolTable[pool_id].start_index;

    U32_t blocks_found = 0;

    /* Loop through the pool's memory space. */
    while (blocks_found < size_blocks && start_index < PoolTable[pool_id].end_index) {
        /* If the current address is 0, increase the amount of memory found.
         * If not 0, it is the size of an allocation. This enables skipping
         * that part of the memory since it is in use. */
        if(OsHeap[end_index] == 0) {
            blocks_found++;
            end_index += BLOCK_SIZE;
        } else { /* If in use, skip. */
            blocks_found = 0;
            if (OsHeap[start_index] != 0) { /* If not 0, address = allocation size. */
                start_index += (U32_t)*((U32_t *)&OsHeap[start_index])/* Skip block. */
                end_index = start_index;
            } else {
                start_index = end_index; /* If 0, continue searching at the end address i.e.
                                              * the last non-zero address i.e. the size of an allocation. */
            }
        }
    }
    void* return_address = NULL;
    if(blocks_found >= size_blocks) {
        *((U32_t*)&OsHeap[start_index]) = size_blocks;
        return_address = (void *) &OsHeap[start_index + ALLOC_DATA_INDEX_OFFSET];
        PoolTable[pool_id].mem_left-= size;
        PoolTable[pool_id].N++;
    } else {
        LOG_ERROR_NEWLINE("Not enough memory available (%u blocks) to allocate %u blocks.", blocks_found, size_blocks);
        OsCritSectEnd();
        return NULL;
    }
    OsCritSectEnd();
    return return_address;

}

void *KMemAlloc(U32_t size)
{
	return MemAlloc(KernelPoolId, size);
}



OsResult_t MemReAlloc(Id_t cur_pool_id, Id_t new_pool_id, void **ptr, U32_t new_size)
{

    if(cur_pool_id > TotalHeapPools || new_pool_id > TotalHeapPools) {
        return OS_OUT_OF_BOUNDS;
    }

    if((cur_pool_id == KernelPoolId || new_pool_id == KernelPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    } else if((cur_pool_id ==ObjectPoolId || new_pool_id == ObjectPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
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
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RESTRICTED;
    }
    OsCritSectBegin();

    OsResult_t result = OS_OK;
	pPmb_t pool = &PoolTable[pool_id];
	
    U32_t size_blocks = *((U32_t*)&OsHeap[pool->start_index]); /* Allocation size in blocks. */
    U32_t size = size_blocks * BLOCK_SIZE_BYTES ; /* Physical size in bytes. */

    if(result == OS_OK) {
        /* Zero memory block. */
        for (U32_t i = pool->start_index; i < pool->end_index; i++) {
			OsHeap[i] = 0;
        }

        PoolTable[pool_id].mem_left += size;
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
    if(pool_id == KernelPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return 0;
    } else if(pool_id == OS_ID_INVALID) {
        return 0;
    }
	pPmb_t pool = &PoolTable[pool_id];
    U32_t size = *((U32_t*)&OsHeap[pool->start_index]) * BLOCK_SIZE_BYTES ; /* Physical size in bytes. */
	
    return size;
}


/*******************************/


U16_t IPoolIdFromIndex(U32_t index)
{
    U16_t id = OS_ID_INVALID;
    //OsCritSectBegin();
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        if(index >= PoolTable[i].start_index && index <= PoolTable[i].end_index) {
            id = i;
        }
    }
    //OsCritSectEnd();
    return id;
}

U16_t IPoolIdFromPointer(MemBase_t *ptr)
{
    U16_t id = OS_ID_INVALID;
    //OsCritSectBegin();
	MemBase_t *start_addr = NULL;
	MemBase_t *end_addr = NULL;
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
		start_addr = &OsHeap[PoolTable[i].start_index];
		end_addr = &OsHeap[PoolTable[i].end_index];
        if(ptr >= start_addr && ptr < end_addr) {
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


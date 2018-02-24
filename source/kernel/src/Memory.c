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
#include <LoggerDef.h>

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

#define ALLOC_DATA_OFFSET   sizeof(U32_t) / sizeof(U8_t)

extern void OsCritSectBegin(void);
extern void OsCritSectEnd(void);

#define BLOCK_SIZE       16

#define BYTES_TO_BLOCKS(bytes) ( (bytes % BLOCK_SIZE) ? (bytes / BLOCK_SIZE) + 1 : (bytes / BLOCK_SIZE) )

Pmb_t PoolTable[N_POOLS];

U8_t OsHeap[PRTOS_CONFIG_OS_HEAP_SIZE_BYTES];

U16_t TotalHeapPools;
U32_t TotalHeapSize;
U16_t HeapIndexEnd;

Id_t KernelPoolId;
Id_t ObjectPoolId;

static U16_t IPoolIdFromIndex(U32_t index);
static U16_t IPoolIdFromPointer(MemBase_t *ptr);

/************OS Memory Management************/

OsResult_t KMemInit()
{
    OsResult_t result = OS_RES_ERROR;

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

    LOG_INFO_NEWLINE("Creating Kernel pool...");
    KernelPoolId = MemPoolCreate(KERNEL_POOL_SIZE_BYTES);
    if(KernelPoolId == OS_ID_INVALID) {
        result = OS_RES_CRIT_ERROR;
        LOG_ERROR_NEWLINE("Invalid pool ID returned");
        goto exit;
    } else {
        LOG_INFO_APPEND("ok");
    }

    LOG_INFO_NEWLINE("Creating Object pool..");
    ObjectPoolId = MemPoolCreate(PRTOS_CONFIG_OS_HEAP_SIZE_BYTES - PRTOS_CONFIG_USER_HEAP_SIZE_BYTES);
    if(ObjectPoolId == OS_ID_INVALID) {
        result = OS_RES_CRIT_ERROR;
        LOG_ERROR_NEWLINE("Invalid pool ID returned");
        goto exit;
    } else {
        LOG_INFO_APPEND("ok");
    }

exit:
    return result;
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
    U32_t mem_found = 0;
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
    mem_req = BYTES_TO_BLOCKS(pool_size);

    while(mem_found < mem_req && (index + index_offset) < HeapIndexEnd) {
        tmp_pool_id = IPoolIdFromIndex(index + index_offset);
        if(tmp_pool_id == OS_ID_INVALID) {
            index_offset += BLOCK_SIZE;
            mem_found += 1;
        } else {
            index = (PoolTable[tmp_pool_id].end_index + 1);
            index_offset = 0;
            mem_found = 0;
        }
    }
    if(mem_found >= mem_req) {
        pmb->end_index = (index + index_offset - 1);
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
        return OS_RES_ID_INVALID;
    }
    if(pool_id == KernelPoolId || pool_id == ObjectPoolId) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }
    OsCritSectBegin();
    MemPoolFormat(pool_id);
    PoolTable[pool_id].pool_size = 0;
    PoolTable[pool_id].end_index = 0;
    PoolTable[pool_id].start_index = 0;
    PoolTable[pool_id].mem_left = 0;
    OsCritSectEnd();
    return OS_RES_OK;
}

OsResult_t MemPoolFormat(Id_t pool_id)
{
    if(pool_id >= N_POOLS) {
        return OS_RES_ID_INVALID;
    }
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }
    OsCritSectBegin();
    pPmb_t mempool = &PoolTable[pool_id];
    for (U32_t i = mempool->start_index; i<mempool->end_index; i++) {
        OsHeap[i] = 0;
    }
    mempool->mem_left = mempool->pool_size;
    mempool->N=0;
    OsCritSectEnd();

    return OS_RES_OK;
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
    OsCritSectBegin();
    U8_t valid_alloc = 1;
    void *alloc_address = NULL;

    if(size == 0) {
        valid_alloc = 0;
    }
    if(pool_id>=TotalHeapPools) {
        valid_alloc = 0;
    }
    /* Only allow allocations in Kernel pool/Object pool in Kernel mode. */
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool.");
        valid_alloc = 0;
    }

    if(PoolTable[pool_id].mem_left < size) {
        valid_alloc = 0;
    }
    if(valid_alloc == 1) {
        /* Reserve extra space to save the size, then calculate the
         * required size in blocks.
         * Actual required size in bytes is calculated afterwards. */
        size += sizeof(U32_t);
        U32_t size_blocks = BYTES_TO_BLOCKS(size);
        size = size_blocks * BLOCK_SIZE;
        U32_t index;
        U32_t index_offset = 0;

        /* Start at the start address of the pool. */
        index = PoolTable[pool_id].start_index;

        U32_t blocks_found = 0;

        /* Loop through the pool's memory space. */
        while ( (blocks_found < size_blocks) && ((index + index_offset) < PoolTable[pool_id].end_index) ) {
            /* If the current address is 0, increase the amount of memory found.
             * If not 0, it is the size of an allocation. This enables skipping
             * that part of the memory since it is in use. */
            if(*((U32_t *)&OsHeap[index + index_offset]) == 0) {
                blocks_found++;
                index_offset += BLOCK_SIZE;
            } else { /* If in use, skip. */
                blocks_found = 0;
                index = index + index_offset + (U32_t)*((U32_t *)&OsHeap[index + index_offset]);/* Skip block. */
                index_offset = 0;
            }
        }

        if(blocks_found >= size_blocks) {
            *((U32_t*)&OsHeap[index]) = size;
            alloc_address = (void *) &OsHeap[index + ALLOC_DATA_OFFSET];
            PoolTable[pool_id].mem_left-= size;
            PoolTable[pool_id].N++;
        } else {
            LOG_ERROR_NEWLINE("Not enough memory available (%u blocks) to allocate %u blocks.", blocks_found, size_blocks);
        }

        if(PoolTable[pool_id].mem_left > PoolTable[pool_id].pool_size) {
            while(1);
        }

        if(alloc_address == NULL) {
            while(1);
        }

    } else {
        LOG_ERROR_NEWLINE("Allocation error.");
    }

    OsCritSectEnd();
    return alloc_address;

}

void *KMemAlloc(U32_t size)
{
    void *addr = NULL;
    KCoreKernelModeEnter();
    addr = MemAlloc(KernelPoolId, size);
    KCoreKernelModeExit();
    return addr;
}

void *KMemAllocObject(U32_t obj_size, U32_t obj_data_size, void **obj_data)
{
    OsCritSectBegin();
    KCoreKernelModeEnter(); /* Enter Kernel Mode to access the object heap. */
    /* Check if there is memory available and the object count has not reached its max. value. */
    if( MemPoolFreeSpaceGet(ObjectPoolId) < (obj_size + obj_data_size) ) {
        goto error;
    }

    /* Allocate the memory for the object and check its validity. */
    void *obj = MemAlloc(ObjectPoolId, obj_size);
    if(obj != NULL) {
        if(obj_data_size != 0) {
            if(obj_data == NULL) {
                MemFree(&obj);
                goto error;
            }
            *obj_data = MemAlloc(ObjectPoolId, obj_data_size);
            if(*obj_data != NULL) {
                goto valid;
            } else { /* Object data allocation failed => Free the object. */
                MemFree(&obj);
                goto error;
            }
        } else { /* No object data has to be allocated. */
            goto valid;
        }
    }

error:
    KCoreKernelModeExit();
    OsCritSectEnd();
    while(1);
    /* TODO: Throw exception. */
    return NULL;

valid:
    KCoreKernelModeExit();
    OsCritSectEnd();
    return obj;
}

OsResult_t KMemFreeObject(void **obj, void **obj_data)
{
    OsResult_t result = OS_RES_ERROR;

    KCoreKernelModeEnter();
    result = MemFree(obj_data);
    result = MemFree(obj);
    KCoreKernelModeExit();
    return result;
}


OsResult_t MemReAlloc(Id_t cur_pool_id, Id_t new_pool_id, void **ptr, U32_t new_size)
{

    if(cur_pool_id > TotalHeapPools || new_pool_id > TotalHeapPools) {
        return OS_RES_OUT_OF_BOUNDS;
    }

    if((cur_pool_id == KernelPoolId || new_pool_id == KernelPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    } else if((cur_pool_id ==ObjectPoolId || new_pool_id == ObjectPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }

    if(new_pool_id == OS_ID_INVALID) {
        new_pool_id = cur_pool_id;
    }
    OsCritSectBegin();

    void* tmp_ptr = MemAlloc(new_pool_id,new_size);

    if(tmp_ptr == NULL) {
        OsCritSectEnd();
        return OS_RES_NULL_POINTER;
    }

//for (U32_t i = 0; (i < size) && (i < new_size); i++) {
//(MemBase_t *)tmp_ptr[i] = (MemBase_t *)(*ptr)[i];
//}
    /* TODO: Copy data in MemReAlloc. */

    MemFree(ptr);

    *ptr = tmp_ptr;

    OsCritSectEnd();
    return OS_RES_OK;
}



OsResult_t MemFree(void **ptr)
{
    if(*ptr == NULL) {
        return OS_RES_NULL_POINTER;
    }

    U8_t *tmp_ptr = (U8_t*)(*ptr);
    Id_t pool_id = IPoolIdFromPointer(tmp_ptr);

    if(pool_id == OS_ID_INVALID) { /* Pool not found. */
        return OS_RES_ID_INVALID;
    }
    if((pool_id == KernelPoolId || pool_id == ObjectPoolId) && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }
    OsCritSectBegin();

    OsResult_t result = OS_RES_OK;
    pPmb_t pool = &PoolTable[pool_id];
    tmp_ptr = &tmp_ptr[-ALLOC_DATA_OFFSET]; /* Allocation size is located at pointer - ALLOC_DATA_INDEX_OFFSET */
    U32_t size = *((U32_t *)tmp_ptr);

    if(result == OS_RES_OK) {
        /* Zero memory block. */
        for (U32_t i = 0; i < size; i++) {
            tmp_ptr[i] = 0;
        }
        PoolTable[pool_id].mem_left += size;
        PoolTable[pool_id].N--;

        *ptr = NULL; /* Set pointer to NULL, avoids reusing it in app by accident. */
    }

    if(PoolTable[pool_id].mem_left > PoolTable[pool_id].pool_size) {
        while(1);
    }
    OsCritSectEnd();
    return result;
}

U32_t MemAllocSizeGet(void *ptr)
{
    if(ptr == NULL) {
        return 0;
    }

    MemBase_t* tmp_ptr = (MemBase_t*)(ptr);

    /* Test if the pointer is in a pool address space.
    If the pool ID equals 0 the pointer is either not
    in a pool space or within the kernel pool.
    Operations that access the kernel pool may only
    be executed if kernel-mode flag is set. */
    U16_t pool_id = IPoolIdFromPointer(tmp_ptr);
    if(pool_id == KernelPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return 0;
    } else if(pool_id == OS_ID_INVALID) {
        return 0;
    }
    pPmb_t pool = &PoolTable[pool_id];
    U32_t size = *((U32_t*)&OsHeap[pool->start_index]); /* Physical size in bytes. */

    return size;
}


/*******************************/


static U16_t IPoolIdFromIndex(U32_t index)
{
    U16_t id = OS_ID_INVALID;
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        if(index >= PoolTable[i].start_index && index <= PoolTable[i].end_index && PoolTable[i].pool_size > 0) {
            id = i;
            break;
        }
    }
    return id;
}

static U16_t IPoolIdFromPointer(MemBase_t *ptr)
{
    U16_t id = OS_ID_INVALID;
    MemBase_t *start_addr = NULL;
    MemBase_t *end_addr = NULL;
    for (U16_t i = 0; i < (TotalHeapPools); i++) {
        start_addr = ((MemBase_t *)&OsHeap[PoolTable[i].start_index]);
        end_addr =  ((MemBase_t *)&OsHeap[PoolTable[i].end_index]);
        if(ptr >= start_addr && ptr < end_addr) {
            id = i;
        }
    }
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


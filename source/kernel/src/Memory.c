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
 *  Copyright� 2017    D. van de Spijker
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


#include "include/Memory.h"
#include "kernel/inc/MemoryDef.h"

#include "include/Os.h"
#include "kernel/inc/CoreDef.h"
#include "kernel/inc/LoggerDef.h"

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

static Pmb_t *PoolTable = NULL;

static U8_t *OsHeap = NULL;

static Id_t TotalHeapPools;
static U32_t TotalHeapSize;
static U32_t HeapIndexEnd;

static Id_t ObjectPoolId;

static Id_t IPoolIdFromIndex(U32_t index);
static Id_t IPoolIdFromPointer(MemBase_t *ptr);

/************OS Memory Management************/

OsResult_t KMemInit(U8_t *heap, U32_t heap_size, U32_t user_heap_size, Pmb_t *pool_table)
{
	if(heap == NULL || heap_size == 0 || pool_table == NULL) {
		return OS_RES_INVALID_ARGUMENT;
	}
	
    OsResult_t result = OS_RES_OK;

    OsHeap = heap;
    PoolTable = pool_table;
    TotalHeapPools = (Id_t)(MEM_NUM_POOLS);
    TotalHeapSize = heap_size;
    HeapIndexEnd = TotalHeapSize - 1;

    for (Id_t i = 0; i < (TotalHeapPools); i++) {
        PoolTable[i].start_index = 0;
        PoolTable[i].end_index = 0;
        PoolTable[i].pool_size = 0;
        PoolTable[i].mem_left = 0;
    }
    for (U32_t j = 0; j< TotalHeapSize; j++) {
        OsHeap[j] = 0;
    }


	LOG_INFO_NEWLINE("Creating Object pool..");
	ObjectPoolId = MemPoolCreate(TotalHeapSize - user_heap_size);
	if(ObjectPoolId == ID_INVALID) {
		result = OS_RES_CRIT_ERROR;
		LOG_ERROR_NEWLINE("Invalid pool ID returned");
	} else {
		LOG_INFO_APPEND("ok");
	}

    return result;
}


Id_t MemPoolCreate(U32_t pool_size)
{
    if((pool_size == 0) || (pool_size > TotalHeapSize)) {
        return ID_INVALID;
    }

    OsCritSectBegin();

    Id_t pool_id = 0;
    Id_t tmp_pool_id = 0;
    pPmb_t pmb = NULL;
    U32_t blocks_req = 0;
    U32_t blocks_found = 0;
    U32_t index = 0;
    U32_t index_offset = 0;

    /* Search for an unused PMB */
    while(PoolTable[pool_id].pool_size > 0 && pool_id < TotalHeapPools) {
        pool_id++;
    }
    
    /* Check if one was found. */
    if(pool_id >= TotalHeapPools) {
        LOG_ERROR_NEWLINE("Pool ID %04x is invalid.");
        OsCritSectEnd();
        return ID_INVALID;   	
    }

    pmb = &(PoolTable[pool_id]);
    blocks_req = MEM_BYTES_TO_BLOCKS(pool_size);

    while(blocks_found < blocks_req && (index + index_offset) < HeapIndexEnd) {
        tmp_pool_id = IPoolIdFromIndex(index + index_offset);
        if(tmp_pool_id == ID_INVALID) {
            index_offset += MEM_BLOCK_SIZE;
            blocks_found += 1;
        } else {
            index = (PoolTable[tmp_pool_id].end_index + 1);
            index_offset = 0;
            blocks_found = 0;
        }
    }
    if(blocks_found >= blocks_req) {
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
        LOG_ERROR_NEWLINE("Not enough memory available (%u bytes) to create a new pool (%u bytes).", blocks_found, blocks_req);
        pool_id = ID_INVALID;    //No memory available for the requested pool size
    }

    OsCritSectEnd();
    return pool_id;
}

OsResult_t MemPoolDelete(Id_t pool_id)
{
    if(pool_id >= TotalHeapPools) {
        return OS_RES_INVALID_ID;
    }
    if(pool_id == ObjectPoolId) {
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
    if(pool_id >= TotalHeapPools) {
        return OS_RES_INVALID_ID;
    }
    if(pool_id == ObjectPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }
    OsCritSectBegin();
    
    pPmb_t mempool = &PoolTable[pool_id];
    for (U32_t i = mempool->start_index; i<mempool->end_index; i++) {
        OsHeap[i] = 0;
    }
    mempool->mem_left = mempool->pool_size;
    mempool->N = 0;
    
    OsCritSectEnd();

    return OS_RES_OK;
}


U32_t MemPoolFreeSpaceGet(Id_t pool_id)
{
    if(pool_id >= TotalHeapPools) {
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
    if(pool_id >= TotalHeapPools) {
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
    U32_t total_data = 0;
    
    OsCritSectBegin();
    for (U8_t i = 0; i < TotalHeapPools; i++) {
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
    if(pool_id >= TotalHeapPools) {
        return NULL;
    }
    if(PoolTable[pool_id].mem_left < size) {
        return NULL;
    }
    /* Only allow allocations in the Object pool in Kernel mode. */
    if(pool_id == ObjectPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        LOG_ERROR_NEWLINE("Restricted pool.");
        return NULL;
    }
    
    void *alloc_address = NULL;
    U32_t size_blocks = 0;
    U32_t index = 0;
    U32_t index_offset = 0; 
    U32_t blocks_found = 0;
    
	 OsCritSectBegin();
	 
	/* Reserve extra space to save the size, then calculate the
	 * required size in blocks.
	 * Actual required size in bytes is calculated afterwards. */
	size += MEM_ALLOC_SIZE_SIZE_BYTES;
	size_blocks = MEM_BYTES_TO_BLOCKS(size);
	size = size_blocks * MEM_BLOCK_SIZE;
	
	/* Start at the start address of the pool. */
	index = PoolTable[pool_id].start_index;

	/* Loop through the pool's memory space. */
	while ( (blocks_found < size_blocks) && ((index + index_offset) < PoolTable[pool_id].end_index) ) {
		/* If the current address is 0, increase the amount of memory found.
		 * If not 0 it contains the size of an allocation, the allocation can be skipped. */
		if(*((U32_t *)&OsHeap[index + index_offset]) == 0) {
			blocks_found++;
			index_offset += MEM_BLOCK_SIZE;
		} else { /* If in use, skip. */
			blocks_found = 0;
			index += index_offset + *((U32_t *)&OsHeap[index + index_offset]);/* Skip allocation. */
			index_offset = 0;
		}
	}

	if(blocks_found >= size_blocks) {
		*((U32_t*)&OsHeap[index]) = size;
		alloc_address = (void *) &OsHeap[index + MEM_ALLOC_DATA_OFFSET];
		PoolTable[pool_id].mem_left-= size;
		PoolTable[pool_id].N++;
	} else {
		LOG_ERROR_NEWLINE("Not enough memory available (%u blocks) to allocate %u blocks.", blocks_found, size_blocks);
	}
	
	OsCritSectEnd();

    return alloc_address;

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
        return OS_RES_INVALID_ARGUMENT;
    }

    if((cur_pool_id == ObjectPoolId || new_pool_id == ObjectPoolId) && (KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0)) {
        LOG_ERROR_NEWLINE("Restricted pool ID.");
        return OS_RES_RESTRICTED;
    }

    if(new_pool_id == ID_INVALID) {
        new_pool_id = cur_pool_id;
    }
    
    OsCritSectBegin();

    U32_t size = MemAllocSizeGet(*ptr);
    void *tmp_ptr = MemAlloc(new_pool_id, new_size);

    if(tmp_ptr == NULL) {
        OsCritSectEnd();
        return OS_RES_FAIL;
    }
    
	for (U32_t i = 0; (i < size) && (i < new_size); i++) {
		((U8_t *)tmp_ptr)[i] = ((U8_t *)(*ptr))[i];
	}

    MemFree(ptr);

    *ptr = tmp_ptr;

    OsCritSectEnd();
    
    return OS_RES_OK;
}



OsResult_t MemFree(void **ptr)
{
    if(ptr == NULL) {
        return OS_RES_INVALID_ARGUMENT;
    }

    U8_t *tmp_ptr = (U8_t *)(*ptr);
    Id_t pool_id = IPoolIdFromPointer((MemBase_t *)tmp_ptr);

    if(pool_id == ID_INVALID) { /* Pool not found. */
        return OS_RES_ERROR;
    }
    if(pool_id == ObjectPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return OS_RES_RESTRICTED;
    }
    
    OsCritSectBegin();

    pPmb_t pool = &PoolTable[pool_id];
    tmp_ptr = &tmp_ptr[-MEM_ALLOC_DATA_OFFSET]; /* Allocation size is located at pointer - ALLOC_DATA_INDEX_OFFSET */
    U32_t size = *((U32_t *)tmp_ptr);

	/* Zero memory block. */
	for (U32_t i = 0; i < size; i++) {
		tmp_ptr[i] = 0;
	}
	pool->mem_left += size;
	pool->N--;

	*ptr = NULL; /* Set pointer to NULL, avoids reusing it in app by accident. */

    OsCritSectEnd();
    return OS_RES_OK;
}

U32_t MemAllocSizeGet(void *ptr)
{
    if(ptr == NULL) {
        return 0;
    }

    MemBase_t *tmp_ptr = (MemBase_t *)ptr;

    /* Test if the pointer is in a pool address space.
    If the pool ID equals 0 the pointer is either not
    in a pool space or within the kernel pool.
    Operations that access the kernel pool may only
    be executed if kernel-mode flag is set. */
    Id_t pool_id = IPoolIdFromPointer(tmp_ptr);
    if(pool_id == ObjectPoolId && KCoreFlagGet(CORE_FLAG_KERNEL_MODE) == 0) {
        return 0;
    } else if(pool_id == ID_INVALID) {
        return 0;
    }
    
    pPmb_t pool = &PoolTable[pool_id];
    U32_t size = *((U32_t*)&OsHeap[pool->start_index]) - MEM_ALLOC_SIZE_SIZE_BYTES; /* Physical size in bytes. */

    return size;
}


/*******************************/


static Id_t IPoolIdFromIndex(U32_t index)
{
    Id_t id = ID_INVALID;
    
    for (Id_t i = 0; i < (TotalHeapPools); i++) {
        if(index >= PoolTable[i].start_index && index <= PoolTable[i].end_index && PoolTable[i].pool_size > 0) {
            id = i;
            break;
        }
    }
    
    return id;
}

static Id_t IPoolIdFromPointer(MemBase_t *ptr)
{
    Id_t id = ID_INVALID;
    MemBase_t *start_addr = NULL;
    MemBase_t *end_addr = NULL;
    
    for (Id_t i = 0; i < (TotalHeapPools); i++) {
    	if(PoolTable[i].pool_size > 0) {
			start_addr = ((MemBase_t *)&OsHeap[PoolTable[i].start_index]);
			end_addr =  ((MemBase_t *)&OsHeap[PoolTable[i].end_index]);
			if(ptr >= start_addr && ptr < end_addr) {
				id = i;
				break;
			}
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


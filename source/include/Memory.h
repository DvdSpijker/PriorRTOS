/******************************************************************************************************************************************
 *  File: Memory.h
 *  Description: Memory Management API.

 *  OS Version: V0.4
 *
 *  Author(s)
 *  -----------------
 *  D. van de Spijker
 *  -----------------
 *
 *  Copyright� 2017    D. van de Spijker
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

#ifndef MEMORY_H_
#define MEMORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "OsTypes.h"

/******************************************************************************
 * @func: Id_t MemPoolCreate(U32_t size)
 *
 * @desc: Creates a memory pool on the OS heap if the amount of space
 * specified is available. MemPoolCreate will return an invalid ID if
 * the operation failed.
 *
 * Arguments:
 * @argin: (U32_t) size; Size to allocate for the pool.
 *
 * @rettype:  (Id_t); Pool ID.
 * @retval:   ID_INVALID; if the pool was not created.
 * @retval:   Other; Valid ID if successful
 ******************************************************************************/
Id_t MemPoolCreate(U32_t size);


/******************************************************************************
 * @func: void MemPoolDelete(Id_t pool_id)
 *
 * @desc: Formats and deletes the pool.
 *
 * Arguments:
 * @argin: (Id_t) pool_id; Pool to delete.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the pool was deleted.
 * @retval:   OS_RES_INVALID_ID; if the pool ID does not exist.
 * @retval:   OS_RES_RESTRICTED; if a pool was accessed without the correct privileges.
 ******************************************************************************/
OsResult_t MemPoolDelete(Id_t pool_id);


/******************************************************************************
 * @func: OsResult_t MemPoolFormat(Id_t pool_id)
 *
 * @desc: Formats the pool. All data will be lost.
 *
 * @argin: (Id_t) pool_id; Pool to format.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the pool was formatted.
 * @retval:   OS_RES_INVALID_ID; if the pool ID does not exist.
 * @retval:   OS_RES_RESTRICTED; if a pool was accessed without the correct privileges.
 ******************************************************************************/
OsResult_t MemPoolFormat(Id_t pool_id);


/******************************************************************************
 * @func: void MemPoolDefrag(Id_t pool_id)
 *
 * @desc: De-fragments the pool. This will result in a more continuous
 * pool, allowing for larger allocations.
 * NOTE: Pool Allocation table required.
 *
 * @argin: (Id_t) pool_id; Pool to de-fragment.
 ******************************************************************************/
//void MemPoolDefrag(Id_t pool_id);



/******************************************************************************
 * @func: OsResult_t MemPoolMove (Id_t src_pool, Id_t dst_pool)
 *
 * @desc: Moves the source pool to the destination pool. The source pool
 * will be zeroed. Returns OS_RES_OK if operation was successful, OS_RES_FAIL if
 * the destination pool is too small.
 *
 * @argin: (Id_t) src_pool; To-move pool ID
 * @argin: (Id_t) dst_pool; Destination pool ID
 *
 * @rettype:  (OsResult_t); sys call status:
 * @retval:   OS_RES_OK; if move was successful.
 * @retval:   OS_RES_RESTRICTED; if a pool was accessed without the correct privileges.
 ******************************************************************************/
//OsResult_t MemPoolMove(Id_t src_pool_id, Id_t dst_pool_id);


/******************************************************************************
 * @func: U32_t MemPoolFreeSpaceGet(Id_t pool_id)
 *
 * @desc: Returns the amount of space (in bytes) available in the pool.
 *
 * @argin: (Id_t) pool_id; Pool ID.
 *
 * @rettype:  (U32_t); Free pool space.
 ******************************************************************************/
U32_t MemPoolFreeSpaceGet(Id_t pool_id);

/******************************************************************************
 * @func: U32_t MemPoolUsedSpaceGet(Id_t pool_id)
 *
 * @desc: Returns the amount of used space (in bytes) in the pool.
 *
 * @argin: (Id_t) pool_id; Pool ID.
 *
 * @rettype:  (U32_t); Used pool space.
 ******************************************************************************/
U32_t MemPoolUsedSpaceGet(Id_t pool_id);


/******************************************************************************
 * @func: U32_t MemOsHeapFreeSpaceGet(void)
 *
 * @desc: Returns the amount of space (in bytes) available on the
 * OS Heap.
 *
 * @rettype:  (U32_t); Free OS heap space.
 ******************************************************************************/
U32_t MemOsHeapFreeSpaceGet(void);


/******************************************************************************
 * @func: void* MemAlloc (Id_t pool_id, U32_t size)
 *
 * @desc: Dynamically allocates memory in given pool with specified size.
 * Allocated memory may be freed or reallocated.
 *
 * @argin: (Id_t) pool_id; ID of the pool where the memory will be allocated.
 * @argin: (U32_t) size; Size to allocate
 *
 * @rettype:  (void *); pointer to memory.
 * @retval:   NULL; if allocation failed.
 * @retval:   Other; Valid pointer if successful
 ******************************************************************************/
void *MemAlloc(Id_t pool_id, U32_t size);


/******************************************************************************
 * @func: OsResult_t MemReAlloc (Id_t cur_pool_id, Id_t new_pool_id,
 * void **ptr, U32_t new_size)
 *
 * @desc: Re-Allocates memory in given
 * pool with specified size. The allocation may be
 * moved to another pool by passing a different pool ID. 
 * Any data stored at the old allocation address will be copied, if the 
 * new allocation is smaller the data is truncated. If the new allocation
 * fails the data will remain intact.
 *
 * @argin: (Id_t) cur_pool_id; Current pool ID of the to-reallocate memory
 * @argin: (Id_t) new_pool_id; New pool ID of the to-reallocate memory.
 * If moving the allocation across memory pools is not desired, pass the same
 * value as cur_pool_id.
 * @argin: (void **) ptr; Pointer to memory location.
 * @argin: (U32_t) new_size; Size to allocate
 * @argout: (void **) ptr; Pointer to new memory location.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if re-allocation was successful.
 * @retval:   OS_RES_FAIL; if the requested block was too large.
 * @retval:   OS_RES_INVALID_ARGUMENT; if ptr is NULL or new_size is 0.
 * @retval:   OS_RES_INVALID_ID; if an invalid pool ID was passed.
 * @retval:   OS_RES_RESTRICTED; if a pool was accessed without the correct privileges.
 ******************************************************************************/
OsResult_t MemReAlloc(Id_t cur_pool_id, Id_t new_pool_id, void **ptr, U32_t new_size);

/******************************************************************************
 * @func: OsResult_t MemFree (void **ptr)
 *
 * @desc: Frees the specified piece of allocated memory, returning it to
 * the pool. Freed memory will be set to 0. The pointer to the freed memory
 * is set to NULL.
 *
 * @argin:  (void**) ptr; Pointer to the allocation pointer
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if freeing was successful.
 * @retval:   OS_RES_INVALID_ARGUMENT; if the memory pointer equals NULL.
 * @retval:   OS_RES_ERROR; if the pointer points to memory which is not allocated within
 * a valid pool.
 * @retval:   OS_RES_RESTRICTED; if a pool was accessed without the correct privileges.
 ******************************************************************************/
OsResult_t MemFree(void **ptr);

/******************************************************************************
 * @func: U32_t MemAllocSizeGet(void *ptr)
 *
 * @desc: Returns the size of the allocation in bytes.
 *
 * @argin: (void *) ptr; Pointer to allocated memory.
 *
 * @rettype:  (U32_t); allocation size.
 * @retval:   0; if the operation failed.
 * @retval:   Other; Valid allocation size.
 ******************************************************************************/
U32_t MemAllocSizeGet(void *ptr);


#ifdef __cplusplus
}
#endif
#endif /* MEMORY_H_ */

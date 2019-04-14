/*
 * MemDef.h
 *
 * Created: 27-9-2017 23:07:23
 *  Author: Dorus
 */


#ifndef MEM_DEF_H_
#define MEM_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "PriorRTOSConfig.h"

#if PRTOS_CONFIG_OS_HEAP_SIZE_BYTES==0
#error "PriorRTOS: No heap-space reserved; PRTOS_CONFIG_OS_HEAP_SIZE_BYTES = 0."
#endif

#if PRTOS_CONFIG_N_USER_POOLS==0
#warning "PriorRTOS: No user pools defined; PRTOS_CONFIG_N_USER_POOLS = 0."
#endif

#if (PRTOS_CONFIG_N_USER_POOLS >= 1) && (PRTOS_CONFIG_OS_HEAP_SIZE_BYTES==0 || PRTOS_CONFIG_USER_HEAP_SIZE_BYTES==0)
#error "PriorRTOS: User pools defined but no heap-space reserved; PRTOS_CONFIG_N_USER_POOLS > 0, PRTOS_CONFIG_OS_HEAP_SIZE_BYTES = 0."
#endif

#define MEM_NUM_OS_POOLS 1
#define MEM_NUM_POOLS PRTOS_CONFIG_N_USER_POOLS + MEM_NUM_OS_POOLS

#define MEM_ALLOC_SIZE_OFFSET	0
#define MEM_ALLOC_DATA_OFFSET	sizeof(U32_t)
#define MEM_ALLOC_SIZE_SIZE_BYTES sizeof(U32_t)
#define MEM_BLOCK_SIZE			16
#define MEM_BYTES_TO_BLOCKS(bytes) ( (bytes % MEM_BLOCK_SIZE) ? (bytes / MEM_BLOCK_SIZE) + 1 : (bytes / MEM_BLOCK_SIZE) )


/* Pool Management Block definition. */
typedef struct Pmb_t {

    U32_t start_index;
    U32_t end_index;

    U32_t pool_size;
    U32_t mem_left;
    U32_t N;

#if PRTOS_CONFIG_ENABLE_MEMORY_PROTECTION==1
    U16_t checksum;
    U32_t *start_padding;
    U32_t *end_padding;
#endif

} Pmb_t;

typedef struct Pmb_t *  pPmb_t;

/******************************************************************************
 * @func: OsResult_t KMemInit(U8_t *heap, U32_t heap_size, U32_t user_heap_size, 
 * Pmb_t *pool_table)
 *
 * @desc: Initializes Memory management.
 *
 * Arguments:
 * @argin: (U8_t *) heap; Pointer to heap memory.
 * @argin: (U32_t) heap_size; Total heap size.
 * @argin: (U32_t) user_heap_size; Size of the user heap.
 * @argin: (Pmb_t *) pool_table; Pointer to the pool table. Must be of size 
 * MEM_NUM_POOLS.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if initialization was successful.
 * @retval:   OS_RES_INVALID_ARGUMENT; if heap or pool_table was NULL or heap_size was 0.
 * @retval:   OS_RES_CRIT_ERROR; if the Kernel or Object pool could not be created.
 ******************************************************************************/
OsResult_t KMemInit(U8_t *heap, U32_t heap_size, U32_t user_heap_size, Pmb_t *pool_table);

void *KMemAllocObject(U32_t obj_size, U32_t obj_data_size, void **obj_data);
OsResult_t KMemFreeObject(void **obj, void **obj_data);

#ifdef __cplusplus
}
#endif
#endif /* MEM_DEF_H_ */

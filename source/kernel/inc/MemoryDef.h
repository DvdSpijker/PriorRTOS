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

#include <PriorRTOSConfig.h>

#if PRTOS_CONFIG_OS_HEAP_SIZE_BYTES==0
#error "PriorRTOS: No heap-space reserved; CONFIG_OS_HEAP_SIZE = 0."
#endif

#if PRTOS_CONFIG_N_USER_POOLS==0
#warning "PriorRTOS: No user pools defined; PRTOS_CONFIG_N_USER_POOLS = 0."
#endif

#if (PRTOS_CONFIG_N_USER_POOLS >= 1) && (PRTOS_CONFIG_OS_HEAP_SIZE_BYTES==0 || PRTOS_CONFIG_USER_HEAP_SIZE_BYTES==0)
#error "PriorRTOS: User pools defined but no heap-space reserved; PRTOS_CONFIG_N_USER_POOLS > 0, CONFIG_OS_HEAP_SIZE = 0."
#endif

#define N_POOLS (PRTOS_CONFIG_N_USER_POOLS + 2) /* User pools + kernel pool + object pool. */


/* Pool Management Block definition. */
typedef struct Pmb_t {

    MemBase_t* start_addr;
    MemBase_t* end_addr;

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


OsResult_t KMemInit(void);
OsResult_t KMemKernelHeapSet(Id_t kernel_heap);
OsResult_t KMemObjectHeapSet(Id_t object_heap);




#ifdef __cplusplus
}
#endif
#endif /* MEM_DEF_H_ */
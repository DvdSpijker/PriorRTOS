/*
 * MemoryTest.c
 *
 * Created: 17-12-2017 15:43:17
 *  Author: Dorus
 */

#include <PriorRTOS.h>
#include "MemoryTest.h"

LOG_FILE_NAME("MemoryTest.c");

void MemoryTest(const void *p_args, U32_t v_arg)
{
    static U32_t mem_pool_size = 0;
    static Id_t mem_pool = OS_RES_ID_INVALID;
    static U8_t alloc_mult = 2;

    TASK_INIT_BEGIN() {
        mem_pool_size = v_arg;
        mem_pool = MemPoolCreate(mem_pool_size);
        if(mem_pool == OS_RES_ID_INVALID) {
            LOG_ERROR_NEWLINE("Failed to created pool.");
            TaskSuspendSelf();
        } else {
            LOG_DEBUG_NEWLINE("Created memory pool of size %u.", mem_pool_size);
        }
    }
    TASK_INIT_END();


    U8_t *mem_alloc_bytes = NULL;
    U32_t *mem_alloc_words = NULL;

    mem_alloc_bytes = (U8_t *)MemAlloc(mem_pool, sizeof(U8_t) * alloc_mult);
    if(mem_alloc_bytes == NULL) {
        LOG_ERROR_NEWLINE("Failed to allocate.")
    } else {
        mem_alloc_bytes[0] = 0xAB;
        mem_alloc_bytes[alloc_mult - 1] = 0xFF;
    }



    mem_alloc_words = (U32_t *)MemAlloc(mem_pool, sizeof(U32_t) * alloc_mult);
    if(mem_alloc_words != NULL) {
        mem_alloc_words[0] = 0xDEADBEEF;
        mem_alloc_words[1] = 0xCAFEFACE;
    }

    if(mem_alloc_bytes != NULL) {
        MemFree((void **)&mem_alloc_bytes);
    }
    if(mem_alloc_words != NULL) {
        MemFree((void **)&mem_alloc_words);
    }

    alloc_mult++;
    if(alloc_mult > 10) {
        alloc_mult = 2;
    }


    TaskSleep(500);
}
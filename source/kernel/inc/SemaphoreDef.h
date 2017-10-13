/*
 * Prior_osdef_smx.h
 *
 * Created: 2-10-2016 12:55:24
 *  Author: Dorus
 */


#ifndef SEMAPHORE_DEF_H_
#define SEMAPHORE_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Semaphore.h>
#include <List.h>

/* TODO: Implement Semaphore types below and move to public .h file. */
#define SEMAPHORE_TYPE_MUTEX_RECURSIVE    0x02
#define SEMAPHORE_TYPE_COUNTING           0x03

typedef struct  Sem_t {
    ListNode_t list_node;

    U8_t        type;
    U8_t        max_cnt;
    U8_t        aq_cnt;

    Id_t        *owner_task_ids; /* List of tasks that successfully acquired the semaphore. */

    void        *resource;

} Sem_t;

typedef struct Sem_t * pSem_t;

OsResult_t SemaphoreInit(void);

#ifdef __cplusplus
}
#endif

#endif /*SEMAPHORE_DEF_H_ */
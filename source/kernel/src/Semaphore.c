#include <Types.h>
#include <Semaphore.h>
#include <SemaphoreDef.h>
#include <TaskDef.h>
#include <CoreDef.h>

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#define SEM_ID_BUFFER_SIZE 4

LinkedList_t SemList;
Id_t SemIdBuffer[SEM_ID_BUFFER_SIZE];

static pSem_t UtilSemaphoreFromId(Id_t sem_id);

OsResult_t SemaphoreInit(void)
{
    ListInit(&SemList, ID_TYPE_SEMAPHORE, SemIdBuffer, SEM_ID_BUFFER_SIZE);

    return OS_OK;
}

Id_t SemaphoreCreate(U8_t sem_type, void *resource, U8_t max_count)
{
    pSem_t new_sem = NULL;
    Id_t sem_id = INVALID_ID;

    void *new_sem_data = NULL;
    new_sem = (pSem_t)CoreObjectAlloc(sizeof(Sem_t), (sizeof(Id_t) * max_count), &new_sem_data); //malloc(sizeof(Sem_t));
    if(new_sem != NULL) {
        if(ListNodeInit(&new_sem->list_node, new_sem) == OS_OK) {
            if(ListNodeAddSorted(&SemList, &new_sem->list_node) == OS_OK) {
                new_sem->type = sem_type;
                new_sem->resource = resource;
                if(sem_type == SEMAPHORE_TYPE_MUTEX_BINARY || SEMAPHORE_TYPE_MUTEX_RECURSIVE) {
                    new_sem->max_cnt = 1;
                } else {
                    new_sem->max_cnt = max_count;
                }
                new_sem->aq_cnt = 0;
                new_sem->owner_task_ids = (Id_t *)new_sem_data;
                sem_id = new_sem->list_node.id;
            } else {
                CoreObjectFree((void **)&new_sem, (void **)&new_sem_data);
            }
        }
    }

    return sem_id;

}

/* TODO: Implementation SemaphoreDelete. */
OsResult_t SemaphoreDelete(Id_t *sem_id);

OsResult_t SemaphoreAcquire(Id_t sem_id)
{
    OsResult_t result = OS_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            if(sem->type == SEMAPHORE_TYPE_MUTEX_BINARY) {
                if(sem->aq_cnt == 0) {
                    sem->aq_cnt++;
                    *(sem->owner_task_ids) = TcbRunning->list_node.id;
                    result = OS_OK;
                } else {
                    result = OS_LOCKED;
                }
            }
        } else {
            result = OS_ERROR;
        }
    }
    LIST_NODE_ACCESS_WRITE_END();

    return result;
}

void *SemaphoreAcquireResource(Id_t sem_id)
{
    return NULL;
}

OsResult_t SemaphoreRelease(Id_t sem_id)
{
    OsResult_t result = OS_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            if(sem->type == SEMAPHORE_TYPE_MUTEX_BINARY) {
                if(sem->aq_cnt == 1) {
                    sem->aq_cnt--;
                    sem->owner_task_ids = NULL;
                    result = OS_OK;
                } else {
                    result = OS_LOCKED;
                }
            }
        } else {
            result = OS_ERROR;
        }
    }
    LIST_NODE_ACCESS_WRITE_END();

    return result;
}

OsResult_t SemaphoreCountSet(Id_t sem_id, U8_t count);

U8_t SemaphoreCountGet(Id_t sem_id)
{
    U8_t count = 0;
    LIST_NODE_ACCESS_READ_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        count = sem->aq_cnt;
    }
    LIST_NODE_ACCESS_READ_END();
    return count;
}

OsResult_t SemaphoreCountReset(Id_t sem_id)
{
    OsResult_t result = OS_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            sem->aq_cnt = 0;
            /* TODO: Remove owner task IDs. */
            result = OS_OK;
        }
    }
    LIST_NODE_ACCESS_WRITE_END();
    return result;
}


static pSem_t UtilSemaphoreFromId(Id_t sem_id)
{
    pSem_t sem = NULL;
    ListNode_t *node = ListSearch(&SemList, sem_id);
    if(node != NULL) {
        sem = (pSem_t)ListNodeChildGet(node);
    }
    return sem;
}

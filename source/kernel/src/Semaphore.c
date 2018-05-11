#include <Semaphore.h>
#include <SemaphoreDef.h>
#include <TaskDef.h>
#include <CoreDef.h>
#include <MemoryDef.h>
#include <SystemCall.h>

#include <stdlib.h>
#include <inttypes.h>
#include <OsTypes.h>
#include <stdio.h>

LinkedList_t SemList;

static pSem_t ISemaphoreFromId(Id_t sem_id);

OsResult_t KSemaphoreInit(void)
{
    ListInit(&SemList, ID_TYPE_SEMAPHORE);

    return OS_RES_OK;
}

Id_t SemaphoreCreate(U8_t sem_type, U8_t max_count)
{
    pSem_t new_sem = NULL;
    Id_t sem_id = OS_ID_INVALID;

    void *new_sem_data = NULL;
    new_sem = (pSem_t)KMemAllocObject(sizeof(Sem_t), (sizeof(Id_t) * max_count), &new_sem_data); //malloc(sizeof(Sem_t));
    if(new_sem != NULL) {
        if(ListNodeInit(&new_sem->list_node, new_sem) == OS_RES_OK) {
            if(ListNodeAddSorted(&SemList, &new_sem->list_node) == OS_RES_OK) {
                new_sem->type = sem_type;
                if(sem_type == SEMAPHORE_TYPE_MUTEX_BINARY || SEMAPHORE_TYPE_MUTEX_RECURSIVE) {
                    new_sem->max_cnt = 1;
                } else {
                    new_sem->max_cnt = max_count;
                }
                new_sem->aq_cnt = 0;
                new_sem->owner_task_ids = (Id_t *)new_sem_data;
                sem_id = new_sem->list_node.id;
            } else {
                KMemFreeObject((void **)&new_sem, (void **)&new_sem_data);
            }
        }
    }

    return sem_id;

}

/* TODO: Implementation SemaphoreDelete. */
OsResult_t SemaphoreDelete(Id_t *sem_id);

OsResult_t SemaphoreAcquire(Id_t sem_id, U32_t timeout)
{
    OsResult_t result = OS_RES_LOCKED;

#ifdef PRTOS_CONFIG_USE_EVENT_SEM_ACQUIRE_RELEASE

    SYSTEM_CALL_WAIT_HANDLE_EVENT;

    SYSTEM_CALL_POLL_HANDLE_EVENT(sem_id, SEM_EVENT_RELEASE, &result) {
        /* Do nothing. Normal execution flow. */
    }
    SYSTEM_CALL_POLL_HANDLE_TIMEOUT(&result) {
        return result;
    }
    SYSTEM_CALL_POLL_HANDLE_POLL(&result) {
        return result;
    }

#endif

    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            if(sem->type == SEMAPHORE_TYPE_MUTEX_BINARY) {
                if(sem->aq_cnt == 0) {
                    sem->aq_cnt++;
                    *(sem->owner_task_ids) = TcbRunning->list_node.id;
                    result = OS_RES_OK;
                } else {
#ifdef PRTOS_CONFIG_USE_EVENT_SEM_ACQUIRE_RELEASE
                    SYSTEM_CALL_POLL_WAIT_EVENT(node, sem_id, SEM_EVENT_RELEASE, &result, timeout);
#else
                    result = OS_RES_LOCKED;
#endif
                }
            }
        } else {
            result = OS_RES_ERROR;
        }
    }
    LIST_NODE_ACCESS_END();

    return result;
}

OsResult_t SemaphoreRelease(Id_t sem_id)
{
    OsResult_t result = OS_RES_LOCKED;

    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            switch(sem->type) {
            case SEMAPHORE_TYPE_MUTEX_BINARY: {
                if(sem->aq_cnt == 1) {
                    sem->aq_cnt--;
                    sem->owner_task_ids = NULL;
                    result = OS_RES_OK;
                } else {
                    result = OS_RES_LOCKED;
                }
                break;
            }

            case SEMAPHORE_TYPE_MUTEX_RECURSIVE: {
                result = OS_RES_ERROR;
                break;
            }

            case SEMAPHORE_TYPE_COUNTING: {
                result = OS_RES_ERROR;
                break;
            }

            }
        } else {
            result = OS_RES_ERROR;
        }
    }
    LIST_NODE_ACCESS_END();

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
    LIST_NODE_ACCESS_END();
    return count;
}

OsResult_t SemaphoreCountReset(Id_t sem_id)
{
    OsResult_t result = OS_RES_LOCKED;
    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            sem->aq_cnt = 0;
            /* TODO: Remove owner task IDs. */
            result = OS_RES_OK;
        }
    }
    LIST_NODE_ACCESS_END();
    return result;
}


static pSem_t ISemaphoreFromId(Id_t sem_id)
{
    pSem_t sem = NULL;
    ListNode_t *node = ListSearch(&SemList, sem_id);
    if(node != NULL) {
        sem = (pSem_t)ListNodeChildGet(node);
    }
    return sem;
}

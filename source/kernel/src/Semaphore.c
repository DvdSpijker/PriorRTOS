#include "include/Semaphore.h"
#include "kernel/inc/SemaphoreDef.h"

#include "kernel/inc/TaskDef.h"
#include "kernel/inc/CoreDef.h"
#include "kernel/inc/MemoryDef.h"
#include "kernel/inc/SystemCall.h"

#include "include/OsTypes.h"

#include <stdlib.h>

LinkedList_t SemList;

Sem_t *ISemaphoreFromId(Id_t sem_id);

OsResult_t KSemaphoreInit(void)
{
    ListInit(&SemList, ID_GROUP_SEMAPHORE);

    return OS_RES_OK;
}

Id_t SemaphoreCreate(U8_t sem_type, U8_t max_count)
{
    pSem_t new_sem = NULL;
    Id_t sem_id = ID_INVALID;

    void *new_sem_data = NULL;
    new_sem = (pSem_t)KMemAllocObject(sizeof(Sem_t), (sizeof(Id_t) * max_count), &new_sem_data);
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

OsResult_t SemaphoreDelete(Id_t *sem_id)
{
    Sem_t *sem = ISemaphoreFromId(*sem_id);
    if (sem != NULL) {
        ListNodeDeinit(&SemList, &sem->list_node);

        KMemFreeObject((void **)&sem, (void **)sem->owner_task_ids);
        *sem_id = ID_INVALID;
        return OS_RES_OK;
    }
    return OS_RES_ERROR;
}

OsResult_t SemaphoreAcquire(Id_t sem_id, U32_t timeout)
{
    OsResult_t result = OS_RES_LOCKED;

#ifdef PRTOS_CONFIG_USE_SEM_EVENT_ACQUIRE_RELEASE
    SYS_CALL_EVENT_HANDLE(sem_id, SEM_EVENT_RELEASE, &result) {
        /* Do nothing. Normal execution flow. */
    }
    SYS_CALL_EVENT_HANDLE_TIMEOUT(&result) {
        return result;
    }
    SYS_CALL_EVENT_HANDLE_POLL(&result) {
        return result;
    }

#endif

    LIST_NODE_ACCESS_WRITE_BEGIN(&SemList, sem_id) {
        pSem_t sem = (pSem_t)ListNodeChildGet(node);
        if(sem != NULL) {
            if(sem->type == SEMAPHORE_TYPE_MUTEX_BINARY) {
                if(sem->aq_cnt == 0) {
                    sem->aq_cnt++;
                    *(sem->owner_task_ids) = KCoreTaskRunningGet();
                    result = OS_RES_OK;
                } else {
#ifdef PRTOS_CONFIG_USE_SEM_EVENT_ACQUIRE_RELEASE
                    SYS_CALL_EVENT_REGISTER(node, sem_id, SEM_EVENT_RELEASE, &result, timeout);
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

Sem_t *ISemaphoreFromId(Id_t sem_id)
{
    ListNode_t *node = ListSearch(&SemList, sem_id);
    if(node != NULL) {
        return (Sem_t *)ListNodeChildGet(node);
    } else {
        return NULL;
    }
}

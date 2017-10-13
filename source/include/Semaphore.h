/*
 * Prior_smx.h
 *
 * Created: 27-9-2016 22:31:42
 *  Author: Dorus
 */


#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>
#include <EventDef.h>

/* Semaphore Types */
#define SEMAPHORE_TYPE_MUTEX_BINARY       0x01



/* Semaphore Event macros */
#define SEM_EVENT_CREATE         EVENT_TYPE_CREATE
#define SEM_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_EVENT_SEM_ACQUIRE_RELEASE
#define SEM_EVENT_ACQUIRE        (EVENT_TYPE_ACCESS | 0x00001000)
#define SEM_EVENT_RELEASE        (EVENT_TYPE_ACCESS | 0x00002000)
#endif

/******************************************************************************
 * @func: Id_t SemaphoreCreate(U8_t sem_type, void *resource,
 *              U8_t max_count)
 *
 * @desc: Creates a semaphore of the specified type. If this type is not
 * SEM_TYPE_MUTEX_BINARY the maximum count will be set to max_count. A resource
 * can be attached to the semaphore.
 *
 * Arguments:
 * @argin: (U8_t) sem_type; Semaphore type. Currently only SEM_TYPE_MUTEX_BINARY.
 * @argin: (void *) resource; Resource to protect.
 * @argin: (U8_t) max_count; Maximum allowed recursive acquires.
 *
 * @rettype:  (Id_t) Semaphore ID.
 * @retval:   INVALID_ID; if the creation failed.
 * @retval:   Other; valid ID if the semaphore was created.
 ******************************************************************************/
Id_t       SemaphoreCreate(U8_t sem_type, void *resource, U8_t max_count);


/******************************************************************************
 * @func: OsResult_t SemaphoreDelete(Id_t *sem_id)
 *
 * @desc: Deletes the semaphore and sets sem_id to INVALID_ID.
 *
 * Arguments:
 * @argin: (Id_t *) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_OK; if the semaphore was deleted.
 * @retval:   OS_LOCKED; if the Semaphore is still acquired by one or multiple
 *            users.
 * @retval:   OS_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreDelete(Id_t *sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreAcquire(Id_t sem_id)
 *
 * @desc: Attempts to acquire the semaphore.
 *
 * Arguments:
 * @argin: (Id_t ) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_OK; if the semaphore was acquired.
 * @retval:   OS_LOCKED; if the Semaphore is still acquired by one or multiple
 *            users.
 * @retval:   OS_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreAcquire(Id_t sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreAcquireResource(Id_t sem_id)
 *
 * @desc: Attempts to acquire the semaphore. If successful it return the
 * coupled resource.
 *
 * Arguments:
 * @argin: (Id_t ) sem_id; Semaphore ID.
 *
 * @rettype:  (void *) resource
 * @retval:   NULL; the semaphore is still acquired by one or multiple
 *            users or the semaphore could not be found.
 * @retval:   !NULL; pointer to the resource coupled to this semaphore.
 ******************************************************************************/
void       *SemaphoreAcquireResource(Id_t sem_id);


/******************************************************************************
 * @func: OsResult_t SemaphoreRelease(Id_t sem_id)
 *
 * @desc: Releases the acquired semaphore.
 *
 * Arguments:
 * @argin: (Id_t ) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_OK; if the semaphore was released.
 * @retval:   OS_LOCKED; if the semaphore was not acquired.
 * @retval:   OS_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreRelease(Id_t sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreCountSet(Id_t sem_id, U8_t count)
 *
 * @desc: Sets a new maximum acquire count for the semaphore.
 *
 * Arguments:
 * @argin: (Id_t) sem_id; Semaphore ID.
 * @argin: (U8_t) count; New max. acquire count.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_OK; if the new count was set.
 * @retval:   OS_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreCountSet(Id_t sem_id, U8_t count);

/******************************************************************************
 * @func: OsResult_t SemaphoreCountGet(Id_t sem_id)
 *
 * @desc: Returns the current acquire count of the semaphore.
 *
 * Arguments:
 * @argin: (Id_t) sem_id; Semaphore ID.
 *
 * @rettype:  (U8_t) acquire count
 ******************************************************************************/
U8_t       SemaphoreCountGet(Id_t sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreCountReset(Id_t sem_id)
 *
 * @desc: Forces the semaphore to be released by all users, setting the
 * acquire count to 0. Tasks that have acquired the semaphore at that moment
 * will be suspended.
 *
 * Arguments:
 * @argin: (Id_t) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t) sys call result
 * @retval:   OS_OK; if the count was reset.
 * @retval:   OS_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreCountReset(Id_t sem_id);


#ifdef __cplusplus
}
#endif
#endif /* SEMAPHORE_H_ */
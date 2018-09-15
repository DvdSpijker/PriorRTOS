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

#include "OsTypes.h"
#include "EventDef.h"

/* Semaphore Types */
#define SEMAPHORE_TYPE_MUTEX_BINARY       0x01

/* Semaphore Event macros */
#define SEM_EVENT_CREATE         EVENT_TYPE_CREATE
#define SEM_EVENT_DELETE         EVENT_TYPE_DELETE

#ifdef PRTOS_CONFIG_USE_SEM_EVENT_ACQUIRE_RELEASE
#define SEM_EVENT_ACQUIRE        (EVENT_TYPE_ACCESS | 0x00001000)
#define SEM_EVENT_RELEASE        (EVENT_TYPE_ACCESS | 0x00002000)
#endif

/******************************************************************************
 * @func: Id_t SemaphoreCreate(U8_t sem_type, U8_t max_count)
 *
 * @desc: Creates a semaphore of the specified type. If this type is not
 * SEM_TYPE_MUTEX_BINARY the maximum count will be set to max_count.
 *
 * @argin: (U8_t) sem_type; Semaphore type. Currently only SEM_TYPE_MUTEX_BINARY.
 * @argin: (U8_t) max_count; Maximum allowed recursive acquires.
 *
 * @rettype:  (Id_t); Semaphore ID.
 * @retval:   ID_INVALID; if the creation failed.
 * @retval:   Other; valid ID if the semaphore was created.
 ******************************************************************************/
Id_t SemaphoreCreate(U8_t sem_type, U8_t max_count);


/******************************************************************************
 * @func: OsResult_t SemaphoreDelete(Id_t *sem_id)
 *
 * @desc: Deletes the semaphore and sets sem_id to ID_INVALID.
 *
 * @argin: (Id_t *) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the semaphore was deleted.
 * @retval:   OS_RES_LOCKED; if the Semaphore is still acquired by one or multiple
 *            users.
 * @retval:   OS_RES_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreDelete(Id_t *sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreAcquire(Id_t sem_id, U32_t timeout)
 *
 * @desc: Attempts to acquire the semaphore.
 *
 * @argin: (Id_t ) sem_id; Semaphore ID.
 * @argin: (U32_t) timeout; Timeout in ms.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the semaphore was acquired.
 * @retval:   OS_RES_LOCKED; if the Semaphore is still acquired by one or multiple
 *            users.
 * @retval:   OS_RES_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreAcquire(Id_t sem_id, U32_t timeout);

/******************************************************************************
 * @func: OsResult_t SemaphoreRelease(Id_t sem_id)
 *
 * @desc: Releases the acquired semaphore.
 *
 * @argin: (Id_t ) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the semaphore was released.
 * @retval:   OS_RES_LOCKED; if the semaphore was not acquired.
 * @retval:   OS_RES_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreRelease(Id_t sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreCountSet(Id_t sem_id, U8_t count)
 *
 * @desc: Sets a new maximum acquire count for the semaphore.
 *
 * @argin: (Id_t) sem_id; Semaphore ID.
 * @argin: (U8_t) count; New max. acquire count.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the new count was set.
 * @retval:   OS_RES_ERROR; if the semaphore was not found.
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
 * @rettype:  (U8_t); acquire count
 * @retval: Any; valid count.
 ******************************************************************************/
U8_t SemaphoreCountGet(Id_t sem_id);

/******************************************************************************
 * @func: OsResult_t SemaphoreCountReset(Id_t sem_id)
 *
 * @desc: Forces the semaphore to be released by all users, setting the
 * acquire count to 0. Tasks that have acquired the semaphore at that moment
 * will be suspended.
 *
 * @argin: (Id_t) sem_id; Semaphore ID.
 *
 * @rettype:  (OsResult_t); sys call result
 * @retval:   OS_RES_OK; if the count was reset.
 * @retval:   OS_RES_ERROR; if the semaphore was not found.
 ******************************************************************************/
OsResult_t SemaphoreCountReset(Id_t sem_id);


#ifdef __cplusplus
}
#endif
#endif /* SEMAPHORE_H_ */

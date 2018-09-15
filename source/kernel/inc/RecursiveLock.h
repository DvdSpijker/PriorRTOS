#ifndef RECURSIVE_LOCK_H_
#define RECURSIVE_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "include/OsTypes.h"
#include "include/EventDef.h"

#include <stdbool.h>

#define RECURSIVE_LOCK_EVENT_LOCK(tid)   	(EVENT_TYPE_STATE_CHANGE | 0x00001000 | (U32_t)(0xF000 & tid)) /* ID is of the task that locked the lock. */
#define RECURSIVE_LOCK_EVENT_UNLOCK(tid)	(EVENT_TYPE_STATE_CHANGE | 0x00002000 | (U32_t)(0xF000 & tid)) /* ID is of the task that unlocked the lock. */
#define RECURSIVE_LOCK_EVENT_LOCKED(tid)    (EVENT_TYPE_STATE_CHANGE | 0x00003000 | (U32_t)(0xF000 & tid)) /* ID is of the task attempting to access the lock. */

#define RECURSIVE_LOCK_OWNER_NONE   0x0000
#define RECURSIVE_LOCK_OWNER_KERNEL 0xFFFF


typedef struct RecursiveLock {
    Id_t            owner;  /* Task that owns the lock. */
    Id_t            object; /* Object which this lock is protecting. */
	volatile U8_t   lock;	/* Recursive lock counter. */
}RecursiveLock_t;

typedef enum {
	LOCK_MODE_READ,
	LOCK_MODE_WRITE,
}LockMode_t;

void RecursiveLockInit(RecursiveLock_t *lock, Id_t object_id);

OsResult_t RecursiveLockLock(RecursiveLock_t *lock, LockMode_t mode, Id_t owner_id);

OsResult_t RecursiveLockUnlock(RecursiveLock_t *lock);

bool RecursiveLockIsLocked(RecursiveLock_t *lock);

U8_t RecursiveLockCountGet(RecursiveLock_t *lock);

#ifdef __cplusplus
}
#endif

#endif

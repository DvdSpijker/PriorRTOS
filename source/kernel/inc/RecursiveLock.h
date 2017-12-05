#ifndef RECURSIVE_LOCK_H_
#define RECURSIVE_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>
#include <EventDef.h>

#include <stdint.h>
#include <stdbool.h>

#define RECURSIVE_LOCK_EVENT_LOCK   (EVENT_TYPE_STATE_CHANGE | 0x00000001)
#define RECURSIVE_LOCK_EVENT_UNLOCK (EVENT_TYPE_STATE_CHANGE | 0x00000002)
#define RECURSIVE_LOCK_EVENT_LOCKED (EVENT_TYPE_STATE_CHANGE | 0x00000003)

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

OsResult_t RecursiveLockLock(RecursiveLock_t *lock, LockMode_t mode, Id_t task_id);

OsResult_t RecursiveLockUnlock(RecursiveLock_t *lock);

bool RecursiveLockIsLocked(RecursiveLock_t *lock);

U8_t RecursiveLockCountGet(RecursiveLock_t *lock);

#ifdef __cplusplus
}
#endif

#endif
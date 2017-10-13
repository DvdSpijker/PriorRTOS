#ifndef RECURSIVE_LOCK_H_
#define RECURSIVE_LOCK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <Types.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct RecursiveLock {
	volatile U8_t lock;	
}RecursiveLock_t;

typedef enum {
	LOCK_MODE_READ,
	LOCK_MODE_WRITE,
}LockMode_t;

void RecursiveLockInit(RecursiveLock_t *lock);

OsResult_t RecursiveLockLock(RecursiveLock_t *lock, LockMode_t mode);

OsResult_t RecursiveLockUnlock(RecursiveLock_t *lock);

bool RecursiveLockIsLocked(RecursiveLock_t *lock);

U8_t RecursiveLockCountGet(RecursiveLock_t *lock);

#ifdef __cplusplus
}
#endif

#endif
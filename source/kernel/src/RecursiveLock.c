/*
* RecursiveLock.c
*
* Created: 5-10-2017 19:36:48
*  Author: Dorus
*/

#include <RecursiveLock.h>

#define LOCK_MASK_CHECKED          0b01000000
#define LOCK_MASK_MODE             0b10000000
#define LOCK_MASK_COUNT            0b00111111
#define LOCK_COUNT_MAX_VALUE       63

#define LOCK_MODE_SET_WRITE(lock) (lock |= LOCK_MASK_MODE)
#define LOCK_MODE_SET_READ(lock) (lock &= ~(LOCK_MASK_MODE))
#define LOCK_CHECKED_BIT_SET(lock) (lock |= LOCK_MASK_CHECKED)
#define LOCK_CHECKED_BIT_GET(lock) ((lock & LOCK_MASK_CHECKED) ? true : false)
#define LOCK_CHECKED_BIT_CLEAR(lock) (lock &= ~(LOCK_MASK_CHECKED))
#define LOCK_COUNT_INC(lock) if((lock & LOCK_MASK_COUNT) < LOCK_COUNT_MAX_VALUE)++lock
#define LOCK_COUNT_DEC(lock) if((lock & LOCK_MASK_COUNT) > 0)--lock
#define LOCK_COUNT_GET(lock)(lock & LOCK_MASK_COUNT)

#define LOCK_MODE_IS_WRITE(lock)((lock & LOCK_MASK_MODE) ? true : false)

#define LOCK_MODE_IS_READ(lock)((lock & LOCK_MASK_MODE) ? true : false)

extern void OsCritSectBegin(void);
extern void OsCritSectEnd(void);

void RecursiveLockInit(RecursiveLock_t *lock)
{
    lock->lock = 0;
}

OsResult_t RecursiveLockLock(RecursiveLock_t *lock, LockMode_t mode)
{
    OsCritSectBegin();
    OsResult_t result = OS_ERROR;

    if(mode == LOCK_MODE_READ) {

        /* If not locked, check value of the lock counter.
        * If the counter has not reached its max. value
        * increment, otherwise return. */
        if(LOCK_COUNT_GET(lock->lock) == LOCK_COUNT_MAX_VALUE) {
            result = OS_LOCKED;
            goto exit;
            } else {
            LOCK_COUNT_INC(lock->lock);
            result = OS_OK;
            goto exit;
        }
        } else if(mode == LOCK_MODE_WRITE) {

        if(LOCK_CHECKED_BIT_GET(lock->lock)) {
            if(LOCK_COUNT_GET(lock->lock) == LOCK_COUNT_MAX_VALUE) {
                result = OS_LOCKED;
                goto exit;
                } else {
                LOCK_COUNT_INC(lock->lock);
                result = OS_OK;
                goto exit;
            }
            } else {
            if(LOCK_COUNT_GET(lock->lock) == LOCK_COUNT_MAX_VALUE) {
                result = OS_LOCKED;
                goto exit;
                } else {
                /* Locking in write mode is not allowed when
                * the lock is in read mode and the counter != 0,
                * since there are active readers which are not
                * protected by a critical section. */
                if(LOCK_MODE_IS_READ(lock->lock) && LOCK_COUNT_GET(lock->lock) != 0) {
                    result = OS_LOCKED;
                    goto exit;
                }
                LOCK_MODE_SET_WRITE(lock->lock);
                LOCK_CHECKED_BIT_SET(lock->lock);
                LOCK_COUNT_INC(lock->lock);
                OsCritSectBegin();
                result = OS_OK;
                goto exit;
            }
        }
        } else { /* Invalid mode. */
        result = OS_ERROR;
    }

    exit:
    OsCritSectEnd();
    return result;
}

OsResult_t RecursiveLockUnlock(RecursiveLock_t *lock)
{
    OsCritSectBegin();
    OsResult_t result = OS_ERROR;
    
    LOCK_COUNT_DEC(lock->lock);
    if(LOCK_COUNT_GET(lock->lock) == 0) {
        LOCK_CHECKED_BIT_CLEAR(lock->lock);
        LOCK_MODE_SET_READ(lock->lock);
        OsCritSectEnd();
    }
    result = OS_OK;

    OsCritSectEnd();
    return result;
}

bool RecursiveLockIsLocked(RecursiveLock_t *lock)
{
    return (lock->lock & LOCK_MASK_COUNT);
}

U8_t RecursiveLockCountGet(RecursiveLock_t *lock)
{
    U8_t lock_count = LOCK_COUNT_GET(lock->lock);
    return lock_count;
}

#ifndef OS_RESULT_H_
#define OS_RESULT_H_

typedef enum {
    
    /* The system call has caused a critical error. The reason is
     * further specified in the API description of
     * the caller. */
    OS_RES_CRIT_ERROR       = -6,

    /* The system call has caused an error. The reason is
     * further specified in the API description of
     * the caller. */
    OS_RES_ERROR            = -5,

    /* The system call violated access rights. */
    OS_RES_RESTRICTED       = -4,
 
     /* The system call failed. The reason is
     * further specified in the API description of
     * the caller. */
    OS_RES_FAIL             = -3,
            
    /* One or more arguments passed to the system call had invalid
     * values. */
    OS_RES_INVALID_ARGUMENT = -2,

    /* One or more IDs passed to the system call were invalid.
     * This could either be a non-existing object or the wrong
     * ID type. */
    OS_RES_INVALID_ID       = -1,
            
    /* The system call was successful. */
    OS_RES_OK               = 0,

    /* The system call attempted to access a resource that is locked. */
    OS_RES_LOCKED           = 1,
            
    /* The system call has timed out. */
    OS_RES_TIMEOUT          = 2,
            
    /* The system call's caller is now polling for a event.  */
    OS_RES_POLL             = 3,

    /* An event has occurred.  */
    OS_RES_EVENT            = 4,

} OsResult_t;

#endif
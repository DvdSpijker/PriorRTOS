#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "OsTypes.h"
#include "TaskDef.h"

typedef struct {
	U32_t prio_inherit;	/* Priority inherited from another task. */
	U32_t prio_boost;	/* Priority boost based on age or deadline. */
} SchedAttributes_t;

OsResult_t KSchedulerInit(LinkedList_t *event_list);
OsResult_t KSchedulerCycle(LinkedList_t *tcb_lists[], U8_t num_lists);
pTcb_t KSchedulerQueueTaskPop(void);
U32_t KSchedulerQueueSizeGet(void);

#endif

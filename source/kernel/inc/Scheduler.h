#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "OsTypes.h"
#include "TaskDef.h"

void KSchedulerInit(LinkedList_t *event_list);
void KSchedulerCycle(LinkedList_t **tcb_lists, U8_t num_lists);
pTcb_t KSchedulerQueueTaskPop(void);
U32_t KSchedulerQueueSizeGet(void);

#endif

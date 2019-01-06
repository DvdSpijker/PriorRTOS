#ifndef SCHEDULER_H_
#define SCHEDULER_H_


void KSchedulerInit(void);
void KSchedulerCycle(void);
pTcb_t KSchedulerTaskGet(void);

#endif
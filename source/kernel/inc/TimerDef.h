/*
 * TimerDef.h
 *
 * Created: 27-9-2017 23:03:32
 *  Author: Dorus
 */


#ifndef TIMER_DEF_H_
#define TIMER_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "Timer.h"

#include "kernel/inc/List.h"

typedef struct Timer_t {
    ListNode_t       list_node;
    
    TimerOverflowCallback_t ovf_callback;
    void 				*context;

    TmrState_t          state;
    U8_t                parameter;
    U32_t               T_us;
    U32_t               ticks;
} Timer_t;

typedef struct Timer_t*  pTimer_t;
LinkedList_t TimerList;

OsResult_t KTimerInit(void);
void KTimerUpdateAll(U32_t t_us);



#ifdef __cplusplus
}
#endif
#endif /* TIMER_DEF_H_ */

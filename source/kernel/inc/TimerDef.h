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

#include <List.h>
#include <Timer.h>

typedef struct Timer_t {
    ListNode_t       list_node;

    TmrState_t          state;
    U8_t                parameter;
    U32_t               T_us;
    U32_t               ticks;
} Timer_t;

typedef struct Timer_t*  pTimer_t;
LinkedList_t TimerList;

OsResult_t TimerInit(void);
void TimerUpdateAll(U32_t t_accu);



#ifdef __cplusplus
}
#endif
#endif /* TIMER_DEF_H_ */
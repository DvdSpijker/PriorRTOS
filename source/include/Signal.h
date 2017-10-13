/*
 * Prior_signal.h
 *
 * Created: 27-9-2016 22:32:39
 *  Author: Dorus
 */ 


#ifndef PRIOR_SIGNAL_H_
#define PRIOR_SIGNAL_H_


#include "Prior_types.h"
#include "Prior_task.h"

/* ASR_t definition */
typedef		void			(*ASR_t)(void);
ASR_t* SignalVect_Table;

Stat_t sig_RoutineAssign(U8_t sigvect, ASR_t sighandler, Prio_t Plevel);
Stat_t sig_Catch(U8_t sigvect);
Stat_t sig_Release(U8_t sigvect);
Stat_t sig_Broadcast(U8_t sigvect);
Stat_t sig_Send(U8_t sigvect, Task_t send_to);
Stat_t sig_Ignore(U8_t sigvect);
Stat_t sig_Block(U8_t sigvect);
Stat_t sig_Unblock(U8_t sigvect);


#endif /* PRIOR_SIGNAL_H_ */
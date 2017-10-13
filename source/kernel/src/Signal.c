/**********************************************************************************************************************************************
 *  File: Prior_signals.c
 *
 *  Description: Prior Asynchronous Signal module
 *
 *	OS Version: V0.3
 *  Date: 13/03/2015
 *
 *	Author(s)
 *  -----------------
 *  D. van de Spijker
 *	-----------------
 *
 *
 *  Copyright© 2016	D. van de Spijker
 *
 *	Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software AND associated documentation files (the "Software"), to deal
 *	in the Software without restriction, including without limitation the rights to use,
 *	copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 *	and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *	1. Redistributions of source code must retain the above copyright notice,
 *  this list of conditions and the following disclaimer.
 *
 *  2. The name of Prior RTOS may not be used to endorse or promote products derived
 *    from this Software without specific prior written permission.
 *
 *  3. This Software may only be redistributed and used in connection with a
 *    product in which Prior RTOS is integrated. Prior RTOS shall not be
 *    distributed, under a different name or otherwise, as a standalone product.
 *
 *
 *	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *	OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *	LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *	CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************************************************************************************/
 
 

#include "../Include/Prior_types.h"
#include "../OSDefinitions/Prior_osdef_core.h"

#include "../Include/Prior_signal.h"
#include "../OSDefinitions/Prior_osdef_signal.h"

#include "../Include/Prior_task.h"
#include "../OSDefinitions/Prior_osdef_task.h"

#include "../Include/Prior_mm.h"


#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

ID_t signal_pool;


/*
Signal Parameter

bit 7: Routine Attached		1 when TRUE
bit 6: Signal Caught		1 when TRUE
bit 5: Signal Arrived		1 when TRUE
bit 4: Signal Blocked		1 when TRUE

bit 3-0: Signal priority level	
ranging from 0-15


*/

void sig_Init(void)
{
	signal_pool = mm_PoolCreate((sizeof(ASR_t)*32 + sizeof(U8_t)*32 + 5));
    SignalVect_Table = (ASR_t*)mm_AllocDynamic(signal_pool,sizeof(ASR_t)*32); //Replace with AllocStatic when implemented
	SignalParam_Table = (U8_t*)mm_AllocDynamic(signal_pool,sizeof(U8_t)*32);
}

Stat_t sig_RoutineAssign(U8_t sigvect, ASR_t routine, Prio_t Plevel)
{
	if(sigvect>31) return e_error;
	*(SignalVect_Table + sigvect) = routine;
	*(SignalParam_Table + sigvect) |= (0x80) | Plevel;	
	return e_ok;
}

Stat_t sig_PriorityLevelSet(U8_t sigvect,Prio_t Plevel)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) || (Plevel>15)) return e_error;
	
	*(SignalParam_Table + sigvect) |= (0x0F & Plevel);
	return e_ok;
}

Prio_t sig_PriorityLevelGet(U8_t sigvect)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return 0xFF;
	
	return (*(SignalParam_Table + sigvect) & 0xF0);
}

Stat_t sig_CatchNext(void)
{
	
	return e_error;
}

Stat_t sig_Catch(U8_t sigvect)
{

	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80)) return e_error; //Signal vector invalid
	if ( (*(SignalParam_Table + sigvect) & 0x10) ) return e_fail; //Signal has been blocked
	
	
	U8_t sig_vld=0;
	U8_t i;
	for (i =2;i<*(TCB_running->signals);i++)
	{
		if(*(TCB_running->signals + i) == sigvect) 
		{
			sig_vld=1;
			break;
		}
	}
	if(sig_vld && (	*(SignalParam_Table + sigvect) )) 
	{
		*(SignalParam_Table + sigvect)  |= 0x40;
	}
	
	else if( (*(SignalParam_Table + sigvect)  & 0x40) && sig_vld)
	{
		task_Wait(TCB_running->handler,(Sig_id | sigvect),0x00); 
		return e_busy;//Signal already caught by other task. ASR to be executed
	} 
	else if(!sig_vld)
	{
		task_Wait(TCB_running->handler,(Sig_id | sigvect),0x00); 
		return e_busy;//Signal already caught by other task. ASR to be executed
	}
	

	
	return e_ok;
}

Stat_t sig_Release(U8_t sigvect)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return e_error;
	*(SignalParam_Table + sigvect) &= ~0x40;
	TCB_running->task_wait_ID=0x0000;
	return e_ok;	
}

Stat_t sig_Broadcast(U8_t sigvect)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return e_error;
	util_BroadcastWaitList((Sig_id | sigvect));
	return e_ok;
}

Stat_t sig_Send(U8_t sigvect, Task_t receiver)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return e_error;
	
	U8_t* sig_reg = util_GetTCBFromTaskEntry(receiver)->signals;
	if(*sig_reg == 11) return e_fail;

	*(sig_reg++);
	sig_reg += *(sig_reg);
	*sig_reg = sigvect;
  
	*(SignalParam_Table + sigvect) |= 0x20;
	
	task_Wake(receiver,NULL);
	util_TaskStateSet(util_GetTCBFromTaskEntry(receiver),task_critical);
	
	return e_ok;
}

Stat_t sig_Ignore(U8_t sigvect);

Stat_t sig_Block(U8_t sigvect)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return e_error;
	*(SignalParam_Table + sigvect) |= 0x10;
	return e_ok;
}

Stat_t sig_Unblock(U8_t sigvect)
{
	if(sigvect>31 || !(*(SignalParam_Table + sigvect) & 0x80) ) return e_error;
	*(SignalParam_Table + sigvect) &= ~0x10;	
	return e_ok;
}



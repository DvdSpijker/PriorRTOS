/**********************************************************************************************************************************************
 *  File: Prior_tasktrace.c
 *
 *  Description: Prior tasktrace front-end module
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
#include "../Include/Prior_task.h"
#include "../Include/Prior_ringbuf.h"
#include "../Include/Prior_eventgroup.h"
#include "../OSDefinitions/Prior_osdef_core.h"
#include "../OSDefinitions/Prior_osdef_task.h"

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>


/**************Prior Task Trace***************/

#if CONFIG_ENABLE_TASKTRACE>0

void TtInit(void)
{
	tasktrace_rxbuffer = RingbufCreate(10);
	tasktrace_txbuffer = RingbufCreate(100);
	//os boot
	//send tcb list
	//send tcb running
}

void TtUpdate(void)
{
	OsCritSectEnter();
	
	
	
	if ((CoreFlagGet(all) & (tick_flag | scheduler_flag | tmrupdate_flag)) == tick_flag)
	{
		//ID tick
		if(CoreFlagGet(tickwait_flag))
		{
			//Halt
			//Wait for specified amount of ticks
		}
		
	}
	else if (CoreFlagGet(scheduler_flag))
	{
		//Scheduler ID
		//tcb list state changes
		//ExeQ
	}
	else if (CoreFlagGet(tmrupdate_flag))
	{
		//
	}
	else if(CoreFlagGet(dispatch_flag))
	{
		//switch task ID
		//dispatched task ID
		//send runtime
		if (CoreFlagGet(TASK_FLAG_DELETE))
		{
			
		}
		else
		{
			//if no => new location task ID (tcb wait or tcb list)	
		}
		//tcb running is already known, since exeQ was sent
	}
	
	
	
	OsCritSectExit();
}

void TtTraceTask(pTcb_t tcb)
{
	
}

void TtTraceTimer(pTimer_t timer)
{
	
}

void TtTraceEvent(pEventGrp_t evnt)
{
	
}

void TtTraceSmx(pSem_t smx)
{
	
}

void TtTraceSignal(U8_t sigvector)
{
	
}

void TtTraceMem(MEMBASE_t* address, U32_t size, Id_t MB)
{
	
}

void TtTaskGenericNameSet(Task_t handler, char* generic_name);




#endif

/*******************************/
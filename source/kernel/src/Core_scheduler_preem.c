#include <avr/interrupt.h>
#include "Prior_defs.h"


ISR(TIMER1_COMPA_vect) __attribute__ ((SIGNAL,naked))
{
	core_Tick();
	asm volatile("reti");	
}

void core_Tick(void) __attribute__ ( ( naked ) );
void core_Tick(void)
{
	PORTB |= (1 << PINB3); //For debugging purposes
	
	//wdt_Reset();
	
	port_ContextSave;
	
		
	if(!core_FlagGet(tick_flag))
	{
		core_FlagSet(tick_flag); //Set tick flag 1
		
		core_OSTimeUpdate(); //Update run time
		timer_UpdateAll(); //Update all timers
		
		//Check UART registers
		if(port_CheckUARTRx())
		event_FlagSet(UART_event);

		//Check stack overflow
		if (core_FlagGet(stkovf_flag)) //overflow occurred?
		{
			util_TaskStateSet(traced_task,disabled); //suspend task causing overflow
			core_FlagClear(stkovf_flag);
		}
		
		//Update task schedule
		if(*(OS_reg.lock) == 0) //scheduler not locked?
		core_SchedulerCycle();
		
		core_SwitchTask();
		
		#if ENABLE_TASKTRACE>0
		tt_Update();
		#endif

		//Clear flags
		core_FlagClear(tick_flag);
		core_FlagClear(idle_flag);
	}
	
	port_ContextRestore;
	
	PORTB &= ~(1 << PINB3); //For debugging
	
	asm volatile("ret");
}


pStack_t core_StackNew(U16 depth)
{
	pStack_t top_stack = (pStack_t)mm_Alloc(OS_MemBlock,(depth+6)*sizeof(StackWidth_t)); //6 padding bytes included
	
	*top_stack = (StackWidth_t)0x01;
	top_stack--;
	
	
	
}

/*
 * PortWdt.c
 *
 * Created: 4-3-2018 1:01:30
 *  Author: Dorus
 */ 

#include "PortWdt.h"

#include "iwdg.h"

void PortWdtInit(U8_t wdt_mode, IrqPriority_t wdt_irq_prio)
{
	(void)wdt_mode;
	MX_IWDG_Init();
}

//void PortWdtEnable(U8_t wdt_expire_opt)
//{
//PortGlobalIntDisable();
//
//PortGlobalIntEnable();
//}

void PortWdtDisable()
{
	char x = 0;
	x++;
}

void PortWdtKick()
{
	char x = 0;
	x++;
}

//ISR(WDT_vect)
//{
//CoreWdtIsr();
//}


#include "PortCore.h"

#include "stm32f1xx_hal.h"
#include "core_cm3.h"
#include "tim.h"
#include "tim_config.h"

#define TIMER_INSTANCE_OSTICK	TIM3
#define TIMER_IRQN_OSTICK		TIM3_IRQn

void ITimerCallbackElapsedOsTick(void);

void PortSuperVisorModeEnable(void)
{
	/* Cortex M3 has no support for supervisor mode. */
}

void PortSuperVisorModeDisable(void)
{
	/* Cortex M3 has no support for supervisor mode. */
}

void PortGlobalIntDisable(void)
{
	__disable_irq();
}

void PortGlobalIntEnable(void)
{
	__enable_irq();
}

void PortOsIntDisable(void)
{
	TimerHwIntEnablePeriodElapsed(TIMER_OSTICK, 0);
}

void PortOsIntEnable(void)
{
	TimerHwIntEnablePeriodElapsed(TIMER_OSTICK, 1);
}

void PortOsIntFlagClear(void)
{

}

void PortOsTimerInit(U16_t prescaler, U16_t ovf)
{
	TimerConfig_t tmr_os_cfg = {
		.timer = TIMER_OSTICK,
		.instance = TIMER_INSTANCE_OSTICK,
		.irqn_elapsed = TIMER_IRQN_OSTICK,
		.cb_elapsed = ITimerCallbackElapsedOsTick,
		.irq_prio_elapsed = 0,
		.irq_subprio_elapsed = 0,
	};

	TimerHwConfigSet(&tmr_os_cfg);
	TimerHwInit(TIMER_OSTICK, (uint32_t)prescaler, (uint32_t)ovf);
	PortOsIntDisable();
}


void PortOsTimerDisable(void)
{
	TimerHwStop(TIMER_OSTICK);
}

void PortOsTimerEnable(void)
{
	TimerHwStart(TIMER_OSTICK);
}

U32_t PortOsTimerTicksGet(void)
{
    return TimerHwCountGet(TIMER_OSTICK);
}

void PortOsTimerTicksReset(void)
{
	TimerHwCountSet(TIMER_OSTICK, 0);
}

void PortOsTimerTicksSet(U32_t ticks)
{
	TimerHwCountSet(TIMER_OSTICK, ticks);
}

void ITimerCallbackElapsedOsTick(void)
{
	OsTick();
}


void PortOsIntInit(IrqPriority_t os_tick_irq_prio)
{
	TimerHwIntPrioSetPeriodElapsed(TIMER_OSTICK, os_tick_irq_prio);
}








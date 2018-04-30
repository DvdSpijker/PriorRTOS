
#include "PortCore.h"

#include "stm32f1xx_hal_cortex.h"
#include "core_cm3.h"

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
	/* TODO: Implementation of PortGlobalIntDisable. */
}

void PortGlobalIntEnable(void)
{
	/* TODO: Implementation of PortGlobalIntEnable. */
}

void PortOsIntDisable(void)
{
	HAL_NVIC_DisableIRQ(SysTick_IRQn);
}

void PortOsIntEnable(void)
{
	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void PortOsIntFlagClear(void)
{
	HAL_NVIC_EnableIRQ(SysTick_IRQn);
}

void PortOsTimerInit(uint16_t prescaler, uint16_t ovf)
{
	uint32_t ticks = (uint32_t)(prescaler * ovf);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
	HAL_SYSTICK_Config(ticks);
	PortOsIntDisable();
}


void PortOsTimerStop(void)
{
	SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

void PortOsTimerStart(void)
{
	SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk);
}

uint32_t PortOsTimerTicksGet(void)
{
    uint32_t tc_val = SysTick->VAL;
    return (uint32_t)tc_val;
}

void PortOsTimerTicksReset(void)
{
	SysTick->VAL = 0;
}

void PortOsTimerTicksSet(uint32_t ticks)
{
	SysTick->VAL = ticks;
}

void HAL_SYSTICK_Callback(void)
{
	OsTick();
}


void PortOsTickInit(IrqPriority_t os_tick_irq_prio)
{
	NVIC_SetPriority (SysTick_IRQn, (1UL << os_tick_irq_prio) - 1UL); /* set Priority for Systick Interrupt */
}







